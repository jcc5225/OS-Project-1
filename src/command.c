#include "command.h"
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

// File: command.h
// Author: Jarrad Cisco
// UT eid: jcc5225
// Description:
// Contains functions for executing user commands

int cmd(char *args[]) {
	int *status = NULL;
	if (fork()) {
		wait(status);
	}
	else {
		exit(execvp(args[0], args));			
	}
	return *status;
}


