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
		if (fgJob.pgid != 0) {
			kill(fgJob.pgid, SIGINT);
		}
		else printf("# ");
		break;
	case SIGTSTP:
		printf("\n");
		if (fgJob.pgid != 0) {
			kill(fgJob.pgid, SIGTSTP);
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
	char *args2[ARGS_SIZE] = {0};
	char *tokens[TOKENS_SIZE] = {0};
	job_t *doneJobs[NUM_JOBS] = {0};
	char buf[INPUT_SIZE];
	int status;

	if (signal(SIGINT, sig_handler) == SIG_ERR)
		printf("signal(SIGINT) error");
	if (signal(SIGTSTP, sig_handler) == SIG_ERR)
		printf("signal(SIGTSTP) error");

	// initialize job DS
	initJobs();

	while(1) {
		// print prompt and get ust indicates an EOF (end oer input
		input = readline("# ");

		if (input == 0){
			printf("\n");
			free(fgJob.command);
			exit(0);
		}
		// check for background jobs that have completed
		updateJobs(doneJobs);
		for (int i = 0; i < NUM_JOBS && doneJobs[i] != NULL; i++) {
			jobToStr(doneJobs[i], buf);
			printf("%s\n", buf);
		}
		if (strcmp(input, "") == 0) {
			//user didn't enter anything, don't do anything
		}
		else {
		    // tokenize input
            getTokens(input, tokens);

            bool bg = findBg(tokens);

            // execute command
            status = cmd(tokens, args1, args2, bg);
            if (status == -1 && strcmp(tokens[0], "bg"))
          	    printf("\n");
            else
			    updatePID(status, bg);
		}
	}
}
