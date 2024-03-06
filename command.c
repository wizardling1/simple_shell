#include "command.h"

void interpret_tokens(char ** tokens)
{
    // cd and exit are shell commmands
	if (strcmp(tokens[0], "cd") == 0) {
        cd(tokens);
    }
	else if (strcmp(tokens[0], "exit") == 0) {
		free(tokens);
		exit(0);
	}
    // if it's not cd or exit, it's a system file being executed.
    else {
        run_command_in_child(tokens);
    }
}

void cd(char ** tokens)
{
	char path[PATH_MAX];
	char absolute_path[PATH_MAX];

	// get the path and absolute path we are going to
	// if no arguments are provided, go home
	if (tokens[1] == NULL || strcmp(tokens[1], "~") == 0) {
		strncpy(path, getenv("HOME"), PATH_MAX-1);
		strncpy(absolute_path, path, PATH_MAX-1);
	}
	// if arguments are provided, store provided path in path
	// and absolute path in absolute
	else {
		strncpy(path, tokens[1], PATH_MAX-1);
		// if path doesn't exist, print error and leave cd
		if (realpath(path, absolute_path) == NULL) {
			fprintf(stderr, "cd: %s: %s\n", strerror(errno), path);
			return;			
		}
	}
	
	// change directories, handling error
	if (chdir(path) == -1) {
		fprintf(stderr, "cd: %s: %s\n", strerror(errno), path);
		return;
	}
	
	// update PWD
	char cmd[PATH_MAX+4];
	strcpy(cmd, "PWD=");
	strcat(cmd+4, absolute_path);
	putenv(cmd);
}

// run the user's command with execvp.
int run_command_in_child(char **args)
{
	int pid, status;

	// create a child process
	pid = fork();
	if (pid < 0) {
		perror("fork");
		exit(1);
	}
	
	// child does execvp, but we pass it to pipe in case pipe is present.
	if (pid == 0) {
		check_for_pipes(get_arg_length(args), args);
	}

	// the shell will ignore control-C while child is running, 
    // so only the child is effected.
	signal(SIGINT, SIG_IGN);
	// shell waits for child to finish executing.
	waitpid(pid, &status, 0);
	// print newline if child was killed by controlc
	if (WIFSIGNALED(status))
		switch(WTERMSIG(status)) {
			case SIGINT: printf("\n");
		}
	// restore normal signal behavior
	signal(SIGINT, SIG_DFL);
	// return status
	return status;  		
}

void check_for_pipes(int argc, char *argv[]) {
	// Find location of the colon in argv. If there's no colon, try executing 
	// argv[0].
	int pipeIndex = 0;
	while (strcmp(argv[pipeIndex], PIPE) != 0) {
		pipeIndex++;
		if (pipeIndex >= argc)
		    exec_command(argv);
	}

	// If colon is present, determine if arg1 and arg2 have been specified.
	int arg1Exists = !(pipeIndex == 0);
	int arg2Exists = !(pipeIndex == argc-1);

	// Deal with argument cases. If neither arg1 nor arg2 are present, exit.
	// If one exists, try to execute it. 
	if (!arg1Exists && !arg2Exists) 
		exit(0);
	else if (arg1Exists && !arg2Exists) {
		argv[pipeIndex] = NULL;
		exec_command(argv);
    }
	else if (!arg1Exists && arg2Exists) {
		exec_command(argv+1);
	}
	
	// At this point we know arg1 and arg2 are specified. We will try to pipe
	// arg1 into arg2. First, set up the arguments we will pass to execvp.
	argv[pipeIndex] = NULL;
	char ** arg1 = argv;
	char ** arg2 = argv+pipeIndex+1;

	// create pipe
	int fd[2];
	int rfd, wfd;
	pipe(fd);
	rfd = fd[0];
	wfd = fd[1];

	// Fork; child runs arg1, redirecting stdout into read end of the pipe, 
	// and parent runs arg2, redirecting the write end of pipe into stdin.
    int pid = fork();

	if (pid) { 
		close(STDIN_FILENO);   // close stdin 
        dup(rfd); // make arg1 think that write end of pipe is stdout
		close(rfd); // close both ends of pipe
		close(wfd);
        check_for_pipes(argc-pipeIndex-1, arg2);
	}
	else { 
		close(STDOUT_FILENO);   // close stdout
        dup(wfd); // make arg1 think that read end of pipe is stdout
		close(rfd); // close both ends of pipe
		close(wfd);
        exec_command(arg1);
	}
}

void exec_command(char ** args) {
    char * path = args[0];
	if (execvp(path, args) == -1) { 
		if (errno == 2) { 
			fprintf(stderr, "shell: command not found: %s\n", args[0]); 
		} 
		else { 
			fprintf(stderr, "shell: %s: %s\n", strerror(errno), args[0]); 
		} 
		exit(0);
	}
}

int get_arg_length(char ** args) {
	int len = 0;
	while (*args++) len++;
	return len;
}