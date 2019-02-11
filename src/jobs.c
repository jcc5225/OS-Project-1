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
  j -> pgid = -1;
  j -> command[0] = '\0';
}

static bool isFull() {
  return bgJobs.idx == NUM_JOBS;
}
static bool isEmpty() {
  return bgJobs.idx == 0;
}

static int putJob(job_t *j) {
  if (isFull())
    return -1;
  bgJobs.jobs[bgJobs.idx] = j;
  bgJobs.idx++;
  return 0;
}

static job_t *getJob() {
  if (isEmpty())
    return NULL;
  job_t *j = bgJobs.jobs[bgJobs.idx - 1];
  bgJobs.jobs[bgJobs.idx - 1] = NULL;
  bgJobs.idx--;
  return j;
}

static void killJob(job_t *job) {
  kill(job -> pgid, SIGKILL);

}

static void statusAsStr(job_t *job, char *strBuf) {
	int status = job->status;
	if (WIFSTOPPED(status)) {
		sprintf(strBuf, "Stopped");
		return;
	}
	else if (WIFCONTINUED(status)) {
		// TODO: check for completion here
		sprintf(strBuf, "Running");
		return;
	}
	else if (WIFSIGNALED(status)) {
		sprintf(strBuf, "Stopped");
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

job_t *createJob(pid_t pid1, int status, char *cmd) {
  job_t *j = (job_t *) malloc(sizeof(job_t));
  j -> pgid = pid1;
  j -> command = calloc(strlen(cmd) + 1, sizeof(char));
  sprintf(j->command, "%s", cmd);
	j -> status = status;
  return j;
}

void setMainJob(pid_t pid, char *args[]) {
	fgJob.pgid = pid;
	joinArgs(fgJob.command, args);
}

void clearMainJob() {
	fgJob.pgid = 0;
	fgJob.status = 0;
	for (int i = 0; i < strlen(fgJob.command); i++)
		fgJob.command[i] = '\0';
}

static int pushToBg() {
	job_t *j = createJob(fgJob.pgid, fgJob.status, fgJob.command);
	if (putJob(j) == -1) {
		killJob(&fgJob);
		return -1;
	}
	clearMainJob();
	return 0;
}

void updatePID(int status) {
	fgJob.status = status;
	if (WIFEXITED(status)) {
		clearMainJob();
	}
	else if (WIFSIGNALED(status)) {
		if (pushToBg() == -1)
				printf("too many jobs running (%d), killing foreground job\n", NUM_JOBS);
	}
	else if (WIFSTOPPED(status)) {
    if (pushToBg() == -1)
				printf("too many jobs running (%d), killing foreground job\n", NUM_JOBS);
	}
	else if (WIFCONTINUED(status)) {
		if (pushToBg() == -1)
				printf("too many jobs running (%d), killing foreground job\n", NUM_JOBS);
	}
}

void printJobs() {
	char status[30] = {0};
	for (int i = 0; i < bgJobs.idx; i++) {
        statusAsStr(bgJobs.jobs[i], status);
        printf("[%d] %c %s\t%s\n", i, (i == (bgJobs.idx - 1)) ? '+' : '-', status, bgJobs.jobs[i]->command);
    }
}

pid_t wakeUp() {
	// copy old job to foreground job
	job_t *newJob = getJob();
	fgJob.pgid = newJob->pgid;
	sprintf(fgJob.command, "%s", newJob->command);
	free(newJob->command);
	free(newJob);

	return fgJob.pgid;
}

void resume(int job_no) {

}


