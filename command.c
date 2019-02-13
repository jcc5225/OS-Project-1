#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "command.h"
#include "parse.h"
#include "yash.h"
#include "jobs.h"
// File: command.c
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
	int status = -1;
	switch (dir) {
	case IN:
		if (fd == -1)
			return fd;
		status = dup2(fd, STDIN_FILENO);
		break;
	case OUT:
		// open the file for writing
		if (fd == -1)
			return fd;
		status = dup2(fd, STDOUT_FILENO);
		break;
	case ERR:
		if (fd == -1)
			return fd;
		status = dup2(fd, STDERR_FILENO);
		break;
	}
	return status;
}

static void execute(char *tokens[], char *args[]) {
	int fd;
	int outputLoc = findOutputRedirect(tokens);
	int inputLoc = findInputRedirect(tokens);
	int errLoc = findErr(tokens);

	if (outputLoc != -1) {
		// do output redirection
		fd = open(tokens[outputLoc + 1], O_RDWR | O_APPEND | O_CREAT | O_CLOEXEC, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH);
		redirect(fd, OUT);
	}
	if (inputLoc != -1) {
		// do input redirection
		fd = open(tokens[inputLoc + 1], O_RDONLY | O_CLOEXEC, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH);
		if (fd == -1)
			exit(-1);
		redirect(fd, IN);
	}
	if (errLoc != -1) {
		fd = open(tokens[errLoc + 1], O_RDWR | O_APPEND | O_CREAT | O_CLOEXEC, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH);
		redirect(fd, ERR);
	}

	// look for print jobs command
	if (strcmp(args[0], "jobs") == 0 && argLen(args) == 1) {
		printJobs();
		exit(0);
	}
	// execute command
	else {
		exit(execvp(args[0], args));
	}
}

// @func pipeDreams
// @brief see HEAT by BROCKHAMPTON
// (it actually pipes two commands together)
// @param args1
// @param args2
// @return -1 for failure, 0 otherwise
static int pipeDreams(char *tokens[], char *args1[], char *args2[], int pipeLoc, bool bg) {
	int pfd[2];
	pid_t cpid[2];
	int status;

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
		dup2(pfd[PIPE_WR], STDOUT_FILENO);
		// execute command
		execute(tokens, args1);
	}

	// create new process group
	setpgid(cpid[0], 0);

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
		execute(tokens, args2);
	}
	// add second child to process group
	setpgid(cpid[1], cpid[0]);

	// Close Pipe
	close(pfd[0]);
	close(pfd[1]);
	
	// update fg job
	setMainJob(-1*cpid[0], tokens);
	if (!bg) {
		// wait for children to finish
		waitpid(cpid[1], &status, WUNTRACED);
		waitpid(cpid[0], &status, WUNTRACED);
	}
	else {
		waitpid(cpid[1], &status, WNOHANG);
		waitpid(cpid[0], &status, WNOHANG);
	}
	return status;
}

int cmd(char *tokens[], char *args1[], char *args2[], bool bg) {
	int status, cpid;
	int pipeLoc;

	// pipe if '|' token found
	pipeLoc = findPipe(tokens);
	if (pipeLoc != -1) {
		status = pipeDreams(tokens, args1, args2, pipeLoc, bg);
	}
	// look for fg command
	else if (strcmp(tokens[0], "fg") == 0) {
		int status;
		pid_t pid = wakeUp();
		kill(pid, SIGCONT);
		// wait for all children in process group
		while(waitpid(pid, &status, WUNTRACED) > 0);
		return status;
	}
	else if (strcmp(tokens[0], "bg") == 0) {
		pid_t pid = resume();
		kill (pid, SIGCONT);
		return -1;
	}
	else {
		// get args for exec call
		getArgs(tokens, args1);

		// create child process
		cpid = fork();

		if (cpid == 0) { // Child Process
			execute(tokens, args1);
		}
		// create process group for child
		setpgid(cpid, 0);
		// set foreground process job
		setMainJob(cpid, tokens);
		if (!bg)
			waitpid(cpid, &status, WUNTRACED);
		else {
			waitpid(cpid, &status, WNOHANG);
		}
	}

	return status;
}
