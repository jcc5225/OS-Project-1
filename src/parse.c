#include <string.h>
#include <stdlib.h>
#include "parse.h"

// File: parse.c
// Author: Jarrad Cisco
// UT eid: jcc5225
// Description:
// parses user input into tokens and command arguments

void getArgs(char *tokens[], char *args[]) {
    char **arg = args;
    char **token = tokens;
    while (*token != NULL) {
    	if (*arg != NULL)
			free(*arg);
		*arg = (char *) malloc(31*sizeof(char));
        strcpy(*arg, *token);
        arg++;
        token++;
    }
    *arg = (char *) NULL;
}

char **getTokens(char *input) {
	char **tokens = (char **) calloc(67, sizeof(char *));
	char *tok;
	int i = 0;
	// tokenize user input
	tok = strtok(input, " ");
	while (tok != NULL &&  i < 67) {
		tokens[i] = (char *) malloc(31*sizeof(char));
		strcpy(tokens[i], tok);
		i++;
		tok = strtok(NULL, " \n");
    }
	return tokens;
}

