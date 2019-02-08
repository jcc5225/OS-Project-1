#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include "command.h"
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
static int redirect(int fd, direction dir) {
	int status;
	switch (dir) {
	case IN:
		// open the file for reading
		status = dup2(fd, STDIN_FILENO);
		return status;
	case OUT:
		// open the file for writing
		if (fd == -1)
			return fd;
		status = dup2(fd, STDOUT_FILENO);
		return status;
	default:
		// unexpected enum value
		return -1;
	}
}

static void execute(char *tokens[], char *args[]) {
	int fd;
	int outputLoc = findOutputRedirect(tokens);
	int inputLoc = findInputRedirect(tokens);

	if (outputLoc != -1) {
		fd = open(tokens[outputLoc + 1], O_RDWR | O_APPEND | O_CREAT | O_CLOEXEC, 0777);
		redirect(fd, OUT);

	}
	if (inputLoc != -1) {
		fd = open(tokens[inputLoc + 1], O_RDONLY | O_CLOEXEC, 0777);
		redirect(fd, IN);
	}
	execvp(args[0], args);
}

// @func pipeDreams
// @brief see HEAT by BROCKHAMPTON
// (it actually pipes two commands together)
// @param args1
// @param args2
// @return -1 for failure, 0 otherwise
static int pipeDreams(char *tokens[], char *args1[], char *args2[], int pipeLoc) {
	int pfd[2];
	pid_t cpid[2];
	int status[2];
	int fd;


	// get args for exec call
	getArgs(tokens, args1);
	getArgs(tokens + pipeLoc + 1, args2);

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
	if (cpid[0] == 0) { // Child 1 Process ("Writer")
		// close read end of pipe
		close(pfd[PIPE_RD]);
		// set stdout to pipe writer
		fd = dup2(pfd[PIPE_WR], STDOUT_FILENO);
		// execute command
		execute(tokens, args1);
	}
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
		fd = dup2(pfd[PIPE_RD], STDIN_FILENO);
		// execute command
		execute(tokens, args2);
	}
	
	close(pfd[0]);
	close(pfd[1]);
	// wait for children to finish
	waitpid(cpid[0], &status[0], 0);
	waitpid(cpid[1], &status[1], 0);

	return status[0] | status[1];
}

int cmd(char *tokens[], char *args1[], char *args2[]) {
	int status, cpid;
	int pipeLoc;

	// determine if we're piping or not
	pipeLoc = findPipe(tokens);

	if (pipeLoc != -1) {
		status = pipeDreams(tokens, args1, args2, pipeLoc);
	}
	else {
		// get args for exec call
		getArgs(tokens, args1);

		// create child process
		cpid = fork();

		if (cpid != 0) { // Parent process
			wait(&status);
		}
		else { // Child Process
			execute(tokens, args1);
		}

	}
	return status;
}
