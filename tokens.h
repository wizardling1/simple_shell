#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_TOKEN_LEN 1024
#define MAX_TOKENS 1024

char ** tokenize_line(char *line);
void free_tokens(char ** tokens);
void print_tokens(char ** tokens);