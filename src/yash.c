#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>
#include "yash.h"
#include "parse.h"
#include "command.h"

// File: yash.c
// Author: Jarrad Cisco
// UT eid: jcc5225
// Description:
//		This is the main file for yet another shell (yash).


int main(int argc, char * argv[]) {
	char *input;
	char *args[ARGS_SIZE] = {0};
	char *tokens[TOKENS_SIZE] = {0};
	int status;
	while(1) {
		// print prompt and get user input
		input = readline("# ");

		if (strcmp(input, "") == 0) {
			//user didn't enter anything, don't do anything
		}
		else {
            // tokenize input
            getTokens(input, tokens);
            // if we're exiting, don't bother doing anything else
            if (strcmp(tokens[0], "exit") == 0) {
                exit(0);
            }
			// execute children
           
            status = cmd(tokens, args);
            if (status == -1)
                printf("\n");

        }
	}
}	
