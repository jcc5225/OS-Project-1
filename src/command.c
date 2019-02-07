#include "command.h"
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include "parse.h"
// File: command.h
// Author: Jarrad Cisco
// UT eid: jcc5225
// Description:
// Contains functions for executing user commands

// @func redirect
// @brief redirects file input/output
// @param path filename to redirect input from/output to
// @param dir the direction of the redirection
// @return -1 on failure, new fd otherwise
static int redirect(char *path, direction dir) {
	int fd;
	int status;
	switch (dir) {
	case IN:
		// open the file for reading
		fd = open(path, O_RDONLY | O_CLOEXEC, 0666);
		if (fd == -1)
			return fd;
		status = dup2(fd, STDIN_FILENO);
		return status;
	case OUT:
		// open the file for writing
		fd = open(path, O_RDWR | O_APPEND | O_CREAT | O_CLOEXEC, 0777);
		if (fd == -1)
			return fd;
		status = dup2(fd, STDOUT_FILENO);
		return status;
	default:
		// unexpected enum value
		return -1;
	}
}

int cmd(char *tokens[], char *args[]) {
	int status, cpid;
	int outputLoc;
	int inputLoc;
	
	// get args for exec call
	getArgs(tokens, args);
	outputLoc = findOutputRedirect(tokens);
	inputLoc = findInputRedirect(tokens);

	// create child process
	cpid = fork();
	
	if (cpid != 0) {
		// parent process
		wait(&status);
	}
	else {
		// Child Process
		if (outputLoc != -1) {
			redirect(tokens[outputLoc + 1], OUT);
		} 
		if (inputLoc != -1) {
			redirect(tokens[inputLoc + 1], IN);
		}
		execvp(args[0], args);		
	}
	return status;
}




int pipeDreams(char *args1[], char *args2[]) {
	int pfd[2];
	pid_t cpid[2];
	int status[2];

	if (pipe(pfd) == -1) {
		// pipe failed
		return -1;
	}

	// create "Writer" child process
	cpid[0] = (pid_t) fork();
	if (cpid[0] == -1) {
		// fork failed
		return -1;
	}
	else if (cpid[0] == 0) { // Child 1 Process ("Writer")
		// close read end of pipe
		close(pfd[PIPE_RD]);
		// set stdout to pipe writer
		dup2(pfd[PIPE_WR], STDOUT_FILENO);
		// execute command
		execvp(args1[0], args1);
	}
	else { // Parent Process
		// create another child ("Reader")
		cpid[1] = (pid_t) fork();

		if (cpid[1] == -1) {
			// fork failed
			return -1;
		}
		else if (cpid[1] == 0) { // Child 2 ("Reader")
			// close writer end of pipe
			close(pfd[PIPE_WR]);
			// set stdin to pipe reader	// close read end of pipe
			dup2(pfd[PIPE_RD], STDIN_FILENO);
			// execute command
			execvp(args2[0], args2);
		}
		else { // Parent Process
			// wait for children to finish
			waitpid(cpid[0], &status[0], 0);
			waitpid(cpid[1], &status[1], 0);
		}
	}
	return status[0] | status[1];
}

