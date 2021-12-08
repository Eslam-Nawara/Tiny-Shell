#ifndef JOBS_H
#define JOBS_H

#include <sys/types.h>
#include "globals.h"

struct job_t {              /* The job struct */
    pid_t pid;              /* job PID */
    int jid;                /* job ID [1, 2, ...] */
    int state;              /* UNDEF, BG, FG, or ST */
    char cmdline[MAXLINE];  /* command line */
};

extern struct job_t jobs[MAXJOBS]; /* The job list */

void clearjob(struct job_t *job);

void initjobs(struct job_t *jobs);

int maxjid(struct job_t *jobs);

int addjob(struct job_t *jobs, pid_t pid, int state, char *cmdline);

int deletejob(struct job_t *jobs, pid_t pid);

pid_t fgpid(struct job_t *jobs);

struct job_t *getjobpid(struct job_t *jobs, pid_t pid);

struct job_t *getjobjid(struct job_t *jobs, int jid);

int pid2jid(pid_t pid);

void listjobs(struct job_t *jobs);

#endif