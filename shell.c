#include "shell.h"
#include "command.h"
#include "tokens.h"

int main(int argc, char ** argv)
{
	load_default_settings();
	loop();
	return 0;
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

void loop(void)
{
	char * line;
	char ** tokens;
	while (1) {
		print_prompt();

		line = read_line();
		if (line == NULL) { 
			printf("\n");
			break;
		}
		
		tokens = tokenize_line(line);
		if (tokens[0] == NULL) {
			continue;
		}
		free(line);

		interpret_tokens(tokens);

		free_tokens(tokens);
	}
}

void print_prompt(void)
{
	printf(MAG "%s" BLK "%s" COLOR_RESET, getenv("PWD"), getenv("PS1"));
}

char * read_line(void)
{
	size_t maxlen = MAX_LEN;
	char * line = malloc(sizeof(char) * MAX_LEN);
	
	if (getline(&line, &maxlen, stdin) == -1) {
		if (feof(stdin)) {
			free(line);
			return NULL;
		}
		else {
			perror("getline in read_line");
			exit(1);
		}
	}

	return line;	
}