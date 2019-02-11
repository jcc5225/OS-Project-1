#include <sys/wait.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
// File: jobs.h
// Author: Jarrad Cisco
// UT eid: jcc5225
// Description:
// Contains declarations for job control.

#define NUM_JOBS  20

typedef struct job_struct {
  pid_t pgid;
  int status;
  char *command;
} job_t;

typedef struct process_buffer {
  job_t *jobs[NUM_JOBS];
  int idx;
} process_buffer_t;

// @func printJobs
// @brief prints a summary of currently running jobs
// @param void
// @return void
void printJobs(void);

void setMainJob(pid_t pid1, char *args[]);

void clearMainJob(void);

pid_t wakeUp(void);

void updatePID(int status);