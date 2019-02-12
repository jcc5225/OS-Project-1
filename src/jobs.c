#include "jobs.h"
#include "parse.h"
// File: jobs.c
// Author: Jarrad Cisco
// UT eid: jcc5225
// Description:
// Contains functions for managing jobs

job_t fgJob;
process_buffer_t bgJobs;

static int pop() {
	if (bgJobs.queue.top == 0)
		return -1;

	bgJobs.queue.top--;
	return bgJobs.queue.buf[bgJobs.queue.top];
}

static int push(int val) {
	if (bgJobs.queue.top == NUM_JOBS)
		return -1;

	bgJobs.queue.buf[bgJobs.queue.top] = val;
	bgJobs.queue.top++;
	return 0;
}

static int peek() {
	if (bgJobs.queue.top == 0)
		return -1;
	return bgJobs.queue.buf[bgJobs.queue.top - 1];
}

static int putJob(job_t *j) {
	int i = 0;
	while(i < NUM_JOBS && bgJobs.jobs[i] != NULL) {
		i++;
	}
	if (i == NUM_JOBS)
		return -1;
	bgJobs.jobs[i] = j;
	// if job is stopped, add it to the queue
	if (!(j->bg))
		push(i);
	return i;
}

static job_t *getJob(bool remove) {
	int pos = pop();
	if (pos == -1)
		return NULL;
	job_t *j = bgJobs.jobs[pos];
	if (remove)
		bgJobs.jobs[pos] = NULL;
  	return j;
}

static void killJob(job_t *job) {
  kill(job -> pgid, SIGKILL);

}

static void statusAsStr(job_t *job, char *strBuf) {
	int status = job->status;
	if (job->bg) {
		sprintf(strBuf, "Running");
		return;
	}
	if (WIFCONTINUED(status)) {
		sprintf(strBuf, "Running");
		return;
	}
	else if (WIFSTOPPED(status)) {
		sprintf(strBuf, "Stopped");
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
	fgJob.bg = false;
	for (int i = 0; i < strlen(fgJob.command); i++)
		fgJob.command[i] = '\0';
}

int pushToBg() {
	job_t *j = createJob(fgJob.pgid, fgJob.status, fgJob.command);
	int pos = putJob(j);
	if (pos == -1) {
		killJob(&fgJob);
		return -1;
	}
	j->jobNo = pos;
	j->bg = false;
	clearMainJob();
	return 0;
}

void updatePID(int status, bool bg) {
	fgJob.status = status;
	fgJob.bg = bg;
	if (bg) {
		if (pushToBg() == -1)
			printf("too many jobs running (%d), killing foreground job\n", NUM_JOBS);
	}
	else if (WIFEXITED(status)) {
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

void jobToStr(job_t *job, char *dest) {
	char stat[30];
	statusAsStr(job, stat);
	sprintf(dest, "[%d] %c %s\t%s",
			job->jobNo, (job->jobNo == peek()) ? '+' : '-', stat, job->command);
}

void printJobs() {
	char jobStr[INPUT_SIZE] = {0};
	for (int i = 0; i < NUM_JOBS; i++) {
		if (bgJobs.jobs[i] != NULL) {
			jobToStr(bgJobs.jobs[i], jobStr);
			printf("%s\n", jobStr);
		}
    }
}

pid_t wakeUp() {
	// copy old job to foreground job
	job_t *newJob = getJob(true);
	fgJob.pgid = newJob->pgid;
	sprintf(fgJob.command, "%s", newJob->command);
	free(newJob->command);
	free(newJob);

	return fgJob.pgid;
}

pid_t resume() {
	// get job object
	job_t *newJob = getJob(false);
	newJob->bg = true;
	return newJob->pgid;
}

void initJobs() {
	// Initialize foreground job
	fgJob.command = malloc(INPUT_SIZE*sizeof(char));
	fgJob.bg = false;
	clearMainJob();
	// Initialize background jobs
	bgJobs.queue.top = 0;
}

void updateJobs(job_t *doneJobs[]) {
	int status;
	int doneIdx = 0;
	// empty the destination list
	for (int i = 0; i < NUM_JOBS; i++) {
		if (doneJobs[i] != NULL) {
			job_t *job = doneJobs[i];
			doneJobs[i] = NULL;
			free(job->command);
			free(job);
		}
	}
	// put done jobs into list
	for (int i = 0; i < NUM_JOBS; i++) {
		job_t *job = bgJobs.jobs[i];
		if (job != NULL) {
			if (waitpid(job->pgid, &status, WNOHANG) > 0) {
				doneJobs[doneIdx] = job;
				bgJobs.jobs[i] = NULL;
				doneIdx++;
				job-> status = status;
				job->bg = false;
			}
		}
	}
}


