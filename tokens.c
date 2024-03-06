#include "tokens.h"

/* 	tokenizes the line
	also turns each non-empty quotation (double quotes) into one token */
char ** tokenize_line(char *line)
{
	char ** tokens = calloc(MAX_TOKENS, sizeof(char *));
	char token[MAX_TOKEN_LEN+1];
	int tokens_iter = 0;
	int token_iter = 0;
	int in_quote = 0;

	while (*line) {
		switch (*line) {
			case '"':
				if (strlen(token) > 0) {
					tokens[tokens_iter] = calloc((strlen(token)+1), sizeof(char));
					strncpy(tokens[tokens_iter], token, strlen(token)+1);
					tokens_iter++;
					token_iter = 0;
					token[0] = '\0';
				}
				in_quote = !in_quote;
				break;
			case ' ':
			case '\t':
			case '\r':
			case '\n':
			case '\a':
				if (in_quote) {
					token[token_iter] = *line;
					token[token_iter+1] = '\0';
					token_iter++;
				}
				else if (strlen(token) > 0) {
					tokens[tokens_iter] = calloc((strlen(token)+1), sizeof(char));
					strncpy(tokens[tokens_iter], token, strlen(token)+1);
					tokens_iter++;
					token_iter = 0;
					token[0] = '\0';
				}
				break;
			default:
				token[token_iter] = *line;
				token[token_iter+1] = '\0';
				token_iter++;
		}
		line++;
	}

	if (strlen(token) > 0) {
		tokens[tokens_iter] = calloc((strlen(token)+1), sizeof(char));
		strncpy(tokens[tokens_iter], token, strlen(token)+1);
	}

	print_tokens(tokens);

	return tokens;
}

void free_tokens(char ** tokens)
{
	char ** t = tokens;
	while (*t) {
		free(*t);
		t++;
	}
	free(tokens);
}

void print_tokens(char ** tokens)
{
	char ** t = tokens;
	printf("TOKENS:\n");
	while (*t) {
		printf("%s\n",*t);
		t++;
	}
	printf("END TOKENS\n");
}