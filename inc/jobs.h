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

typedef struct stack_struct {
    int buf[NUM_JOBS];
    int top;
} int_stack;

typedef struct job_struct {
  pid_t pgid;
  int status;
  int jobNo;
  char *command;
  bool bg;
} job_t;

typedef struct process_buffer {
  job_t *jobs[NUM_JOBS];
  int_stack queue;
} process_buffer_t;

// @func printJobs
// @brief prints a summary of currently running jobs
// @param void
// @return void
void printJobs(void);

// @func initJobs
// @brief initializes job tracking
// @param void
// @return void
void initJobs(void);

void setMainJob(pid_t pid1, char *args[]);

void clearMainJob(void);

pid_t wakeUp(void);

pid_t resume(void);

void updatePID(int status, bool bg);

void updateJobs(job_t **);

void jobToStr(job_t *, char *);
