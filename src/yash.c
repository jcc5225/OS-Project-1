#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
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
	char **tokens;
	char *args[67];
	int status;
	while(1) {
		// print prompt and get user input
		input = readline("# ");

		tokens = getTokens(input);
		if (strcmp(input, "") == 0) {
			//user didn't enter anything, don't do anything
		}
		// if we're exiting, don't bother doing anything else
		else if (strcmp(tokens[0], "exit") == 0) {
			exit(0);
		}
		else {
			// execute children
			getArgs(tokens, args);
			status = cmd(args);
			if (status == -1)
				printf("\n");

		}
	}	
}
