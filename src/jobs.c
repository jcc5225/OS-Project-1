#include "jobs.h"
#include "parse.h"
// File: jobs.c
// Author: Jarrad Cisco
// UT eid: jcc5225
// Description:
// Contains functions for managing jobs

job_t fgJob;
process_buffer_t bgJobs;

static void clearJob(job_t *j) {
  j -> mainPid = -1;
  j -> subPid = -1;
  j -> command[0] = '\0';
}

static bool isFull() {
  return (bgJobs.inBuf + 1) % NUM_JOBS == bgJobs.outBuf;
}
static bool isEmpty() {
  return bgJobs.inBuf == bgJobs.outBuf;
}

static int putJob(job_t *j) {
  if (isFull())
    return -1;
  bgJobs.jobs[bgJobs.inBuf] = j;
  bgJobs.inBuf = (bgJobs.inBuf + 1) % NUM_JOBS;
  return 0;
}

static job_t *getJob() {
  if (isEmpty())
    return NULL;
  job_t *j = bgJobs.jobs[bgJobs.outBuf];
  bgJobs.jobs[bgJobs.outBuf] = NULL;
  bgJobs.outBuf = (bgJobs.outBuf + 1) % NUM_JOBS;
  return j;
}

static void killJob(job_t *job) {
  kill(job -> mainPid, SIGKILL);
  if (job -> subPid > 0)
    kill(job -> subPid, SIGKILL);
}

static void statusAsStr(job_t *job, char *strBuf) {
    int status;
   	waitpid(job->mainPid, &status, WNOHANG);
	if (WIFSTOPPED(status)) {
		sprintf(strBuf, "Stopped");
		return;
	}
	else if (WIFCONTINUED(status)) {
		sprintf(strBuf, "Running");
		return;
	}
	else if (WIFSIGNALED(status)) {
		sprintf(strBuf, "Killed");
		return;
	}
	else if (WIFEXITED(status)) {
		sprintf(strBuf, "Done");
		return;
	}
	else {
		sprintf(strBuf, "Uncertain State");
	}
}

job_t *createJob(pid_t pid1, pid_t pid2, char *cmd) {
  job_t *j = (job_t *) malloc(sizeof(job_t));
  j -> mainPid = pid1;
  j -> subPid = pid2;
  j -> command = calloc(strlen(cmd) + 1, sizeof(char));
  strcpy(j->command, cmd);
  return j;
}

void setMainJob(pid_t pid1, pid_t pid2, char *args[]) {
	fgJob.mainPid = pid1;
	fgJob.subPid = pid2;
	joinArgs(fgJob.command, args);
}

void clearMainJob() {
	fgJob.mainPid = -1;
	fgJob.subPid = -1;
	fgJob.command[0] = '\0';
}

void updatePID(int status) {
	if (WIFEXITED(status)) {
		clearJob(&fgJob);
	}
	else if (WIFSIGNALED(status)) {
		printf("child %d killed by signal %d\n", fgJob.mainPid, WTERMSIG(status));
	}
	else if (WIFSTOPPED(status)) {
    job_t *j = createJob(fgJob.mainPid, fgJob.subPid, fgJob.command);
    if (putJob(j) == -1) {
      printf("too many jobs running (%d), killing foreground job\n", NUM_JOBS);
      killJob(&fgJob);
    }
	}
	else if (WIFCONTINUED(status)) {
		printf("Continuing %d\n", fgJob.mainPid);
	}
}

int pushToBg() {
	job_t *j = createJob(fgJob.mainPid, fgJob.subPid, fgJob.command);
    if (putJob(j) == -1) {
		killJob(&fgJob);
		return -1;
	}
	clearMainJob();
	return 0;
}

void printJobs() {
    char jobChar;
    char status[30] = {0};
    // TODO: fix this
	for (int i = (bgJobs.outBuf) % NUM_JOBS; i < bgJobs.inBuf; i++) {
        statusAsStr(bgJobs.jobs[i], status);
        printf("[%d] %c %s\t%s\n", i, '-', status, bgJobs.jobs[i]->command);
    }
}


