/* acts as a pipe, using ":" to seperate programs
   as many arguments as you want allowed.
   this is basically identical to bash |  */

#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <errno.h>

#define PIPE "|"

#define er_cmd(cmd) do { \
    if (cmd == -1) { \
        fprintf(stderr, "shell: command not found: %s\n", argv[0]); \
        return; \
    } \
} while (0)

// just call this recursively till there's no colon.
void pipe_implementation(int argc, char *argv[]) {
	// if there aren't at least 1 arguments, nothing to do; exit.
	if (argc < 1) exit(0);

	// Find location of the colon in argv. If there's no colon, try executing 
	// argv[0].
	int colonIndex = 0;
	while (strcmp(argv[colonIndex], PIPE) != 0) {
        // printf("here\n");
		colonIndex++;
		if (colonIndex >= argc)
            if (execvp(argv[0], argv) == -1) {
                fprintf(stderr, "shell: command not found: %s\n", argv[0]);
                return;
            }
	}

	// If colon is present, determine if arg1 and arg2 have been specified.
	int arg1Exists = !(colonIndex == 0);
	int arg2Exists = !(colonIndex == argc-1);

	// Deal with argument cases. If neither arg1 nor arg2 are present, exit.
	// If one exists, try to execute it. 
	if (!arg1Exists && !arg2Exists) 
		exit(0);
	else if (arg1Exists && !arg2Exists) {
		argv[colonIndex] = NULL;
		if (execvp(argv[0], argv) == -1) {
                fprintf(stderr, "shell: command not found: %s\n", argv[0]);
                return;
            }
    }
	else if (!arg1Exists && arg2Exists) {
		argv[colonIndex] = NULL;
        er_cmd(execvp(argv[colonIndex], argv+colonIndex));
	}
	
	// At this point we know arg1 and arg2 are specified. We will try to pipe
	// arg1 into arg2. First, set up the arguments we will pass to execvp.
	argv[colonIndex] = NULL;
	char ** arg1 = argv;
	char ** arg2 = argv+colonIndex+1;

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
		close(STDOUT_FILENO);   // close stdout
        dup(wfd); // make arg1 think that read end of pipe is stdout
		close(rfd); // close both ends of pipe
		close(wfd);
        er_cmd(execvp(*arg1, arg1));
        int status;
        waitpid(pid, &status, 0);
	}
	else { 
		close(STDIN_FILENO);   // close stdin 
        dup(rfd); // make arg1 think that read end of pipe is stdout
		close(rfd); // close both ends of pipe
		close(wfd);
        pipe_implementation(argc-colonIndex-1, arg2);
	}
}

int main(int argc, char *argv[]) {
	argv++;
	argc--;

    pipe_implementation(argc, argv);
}
