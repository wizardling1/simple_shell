#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <limits.h>
#include <signal.h>

#include "color.h"

#define MAX_LEN 1024
#define DELIM " \t\r\n\a" 

void load_default_settings(void);
void handle_command(char * line, char ** split);
void cd(char ** split);
void print_prompt(void);

void loop(void);
char *read_line(void);
char **split_line(char *line);
int launch(char **args);
