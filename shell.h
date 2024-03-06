#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>
#include <limits.h>
#include <signal.h>
#include "color.h"

#define MAX_LEN 1024
#define DELIM " \t\r\n\a" 

void loop(void);
void load_default_settings(void);
void print_prompt(void);
char *read_line(void);