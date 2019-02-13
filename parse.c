#include <string.h>
#include <stdlib.h>
#include "parse.h"
#include <stdbool.h>
#include <stdio.h>

// File: parse.c
// Author: Jarrad Cisco
// UT eid: jcc5225
// Description:
// parses user input into tokens and command arguments

// @func freeArr
// @brief frees all strings in an array
// @param arr array of strings to be freed
// @param len length of the array
// @return void
static void freeArr(char **arr, int len) {
	for (int i=0; i < len; i++) {
		if (*arr != NULL) {
			free(*arr);
			*arr = NULL;
		}
		arr++;
	}
}

// @func tokcmp
// @brief compares a token to a list of yash tokens (>>, <<, 2>, |, NULL)
// @param token the token to compare
// @return true if one of yash tokens, false otherwise
static bool tokcmp(char *token) {
	return (token == NULL				||
			strcmp(token, ">") == 0		||
			strcmp(token, "<") == 0		||
			strcmp(token, "2>") == 0	||
			strcmp(token, "|")  ==	0	||
			strcmp(token, "&") == 0		);
}

int argLen(char **args) {
	int count = 0;
	while (*args != NULL) {
		count++;
		args++;
	}
	return count;
}

void getArgs(char *tokens[], char *args[]) {
    const size_t TOKEN_SZ = TOKEN_LEN*sizeof(char);
    char **arg = args;
    char **token = tokens;
	// free previous args
	freeArr(args, ARGS_SIZE);
	// copy each argument token
    while (!tokcmp(*token)) {
		*arg = (char *) malloc(TOKEN_SZ);
        snprintf(*arg, TOKEN_SZ, "%s", *token);
        arg++;
        token++;
    }
	// null terminate args
    *arg = (char *) NULL;
}

void getTokens(char *input, char *tokens[]) {
    const size_t TOKEN_SZ = TOKEN_LEN*sizeof(char);
    char *tok;
	int i = 0;
	char **token = tokens;
	// free previous tokens
	freeArr(tokens, TOKENS_SIZE);
	// tokenize user input
	tok = strtok(input, " ");
	while (tok != NULL &&  i < TOKENS_SIZE) {
		*token = (char *) malloc(TOKEN_SZ);
        snprintf(*token, TOKEN_SZ, "%s", tok);
        token++;
		i++;
		tok = strtok(NULL, " \n");
    }
	// getting some weird behavior with last token pointing to previous one
	*token = NULL;
}

int findOutputRedirect(char *tokens[]) {
	for (int i=1; tokens[i] != NULL && i < TOKENS_SIZE; i++) {
		if (strcmp(tokens[i], ">") == 0)
			return i;
	}
	return -1;
}

int findInputRedirect(char *tokens[]) {
	for (int i=1; tokens[i] != NULL && i < TOKENS_SIZE; i++) {
		if (strcmp(tokens[i], "<") == 0)
			return i;
	}
	return -1;
}

int findPipe(char *tokens[]) {
	for (int i=1; tokens[i] != NULL && i < TOKENS_SIZE; i++) {
		if (strcmp(tokens[i], "|") == 0)
			return i;
	}
	return -1;
}

int findErr(char *tokens[]) {
	for (int i=1; tokens[i] != NULL && i < TOKENS_SIZE; i++) {
		if (strcmp(tokens[i], "2>") == 0)
			return i;
	}
	return -1;
}

void joinArgs(char *dest, char *args[]) {
	char **arg = args;
	// clear destination string
	for (int i = 0; i < strlen(dest); i++)
		dest[i] = '\0';
	// append args
	while (*arg != NULL) {
		strcat(dest, *arg);
		strcat(dest, " ");
		arg++;
	}
	// get rid of trailing space
	dest[strlen(dest) - 1] = 0;
}

bool findBg(char **tokens) {
    char **cur = tokens;
    char **next = tokens + 1;
    while (*cur != 0 && *next != 0) {
        next++;
        cur++;
    }
    if (cur != NULL) {
        return !strcmp(*cur, "&");
    }
    return false;
}
