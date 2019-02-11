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
#include "jobs.h"
// File: yash.c
// Author: Jarrad Cisco
// UT eid: jcc5225
// Description:
// This is the main file for yet another shell (yash).
// yash supports basic Linux commands (ls, echo, cat, touch, pwd, sleep).
// yash also supports file redirection and piping, along with foreground and
// background job management.

extern job_t fgJob;
extern process_buffer_t bgJobs;

static void sig_handler(int signo) {
	switch(signo) {
	case SIGINT:
		printf("\n");
		if (fgJob.mainPid > 0) {
			kill(fgJob.mainPid, SIGINT);
			if (fgJob.subPid > 0)
				kill(fgJob.subPid, SIGINT);
			// push foreground job to background buffer
			if (pushToBg() == -1)
				printf("too many jobs running (%d), killing foreground job\n", NUM_JOBS);
		}
		else printf("# ");
		break;
	case SIGTSTP:
		printf("\n");
		if (fgJob.mainPid > 0) {
			kill(fgJob.mainPid, SIGTSTP);
			if (fgJob.subPid > 0)
				kill(fgJob.subPid, SIGTSTP);
			// push foreground job to background
			if (pushToBg() == -1)
				printf("too many jobs running (%d), killing foreground job\n", NUM_JOBS);
		}
		else printf("# ");
		break;
    case SIGCHLD:
        break;
	}
}


int main(int argc, char * argv[]) {
	char *input;
	char *args1[ARGS_SIZE] = {0};
	char *args2[ARGS_SIZE] ={0};
	char *tokens[TOKENS_SIZE] = {0};
	int status;

	if (signal(SIGINT, sig_handler) == SIG_ERR)
		printf("signal(SIGINT) error");
	if (signal(SIGTSTP, sig_handler) == SIG_ERR)
		printf("signal(SIGTSTP) error");
	if (signal(SIGHUP, sig_handler) == SIG_ERR)
        printf("signal(SIGHUP) error");

    // initialize foreground job
	fgJob.command = malloc(INPUT_SIZE*sizeof(char));
	clearMainJob();

	while(1) {
		// print prompt and get ust indicates an EOF (end oer input
		input = readline("# ");

		if (input == 0){
			printf("\n");
			free(fgJob.command);
			exit(0);
		}
		if (strcmp(input, "") == 0) {
			//user didn't enter anything, don't do anything
		}
		else {
		    // tokenize input
            getTokens(input, tokens);

            // execute command
            status = cmd(tokens, args1, args2);
            if (status == -1)
          	    printf("\n");

		}
	}
}
