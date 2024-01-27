#include "shell.h"

int main(int argc, char ** argv)
{
	load_default_settings();

	// read and handle user input loop
	loop();

	return 0;
}

// read user's input loop
void loop(void)
{
	char * line;
	char ** split;
	while (1) {
		print_prompt();

		line = read_line();
		if (line == NULL) { 
			printf("\n");
			break;
		}

		split = split_line(line);
		if (split[0] == NULL) {
			free(line);
			continue;
		}
		
		handle_command(line, split);

		free(split);
		free(line);
	}
}

void handle_command(char * line, char ** split)
{
	if (strcmp(split[0], "cd") == 0) {
		cd(split);
	}
	// automatically add -G to ls
	else if (strcmp(split[0], "ls") == 0) {
		int i;
		for (i = 0; split[i] != NULL; i++) ;
		split[i] = "-G";
		split[i+1] = NULL;
		launch(split);
	}
	else if (strcmp(line, "exit") == 0) {
		free(split);
		free(line);
		exit(0);
	}
	else {
		launch(split);
	}
}

void load_default_settings(void)
{
	// load default settings
	char cur_pwd[PATH_MAX] = {0};
	getcwd(cur_pwd, PATH_MAX);
	setenv("PWD", cur_pwd, 1);
	setenv("PS1", "$ ", 1);
	setenv("PATH", 
		"/opt/homebrew/bin:/usr/local/bin:/usr/bin:/bin:/usr/sbin:/sbin",
		1);	
}

void print_prompt(void)
{
	printf(WHT "%s" MAG "%s" COLOR_RESET, getenv("PWD"), getenv("PS1"));
}

void cd(char ** split)
{
	char path[PATH_MAX];
	char absolute_path[PATH_MAX];

	// get the path and absolute path we are going to
	// if no arguments are provided, go home
	if (split[1] == NULL || strcmp(split[1], "~") == 0) {
		strncpy(path, getenv("HOME"), PATH_MAX-1);
		strncpy(absolute_path, path, PATH_MAX-1);
	}
	// if arguments are provided, store provided path in path
	// and absolute path in absolute
	else {
		strncpy(path, split[1], PATH_MAX-1);
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

char * read_line(void)
{
	size_t maxlen = MAX_LEN;
	char * command = malloc(sizeof(char) * MAX_LEN);
	
	if (getline(&command, &maxlen, stdin) == -1) {
		if (feof(stdin)) {
			free(command);
			return NULL;
		}
		else {
			perror("getline in read_line");
			exit(1);
		}
	}

	return command;	
}

char ** split_line(char *line)
{
	char ** tokens = malloc(sizeof(char *) * MAX_LEN);
	char * token;
	int position = 0;

	token = strtok(line, DELIM);
	while (token != NULL) {
		tokens[position] = token;
		position++;
		token = strtok(NULL, DELIM);
	}
	tokens[position] = NULL;
	return tokens;
}

// run the user's command with execvp.
int launch(char **args)
{
	int pid, status;

	// create a child process
	pid = fork();
	if (pid < 0) {
		perror("fork");
		exit(1);
	}
	
	// child does execvp
	if (pid == 0) {
		if (execvp(args[0], args) == -1) {
			if (errno == 2) {
				fprintf(stderr, "shell: command not found: %s\n", args[0]);
			}
			else {
				fprintf(stderr, "shell: %s: %s\n", strerror(errno), args[0]);
			}
		}
		exit(0);
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
