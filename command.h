#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>
#include <limits.h>
#include <signal.h>

#define PIPE "|"

void interpret_tokens(char ** tokens);
void cd(char ** split);
void check_for_pipes(int argc, char *argv[]);
int run_command_in_child(char **args);
void exec_command(char ** args);
int get_arg_length(char ** args);