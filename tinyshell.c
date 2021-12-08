#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include "jobs.h"
#include "globals.h"
#include "helpers.h"
#include "tinyshell.h"


static int valid_id(char* id);

/* 
 * eval - Evaluate the command line that the user has just typed in
 * 
 * If the user has requested a built-in command (quit, jobs, bg or fg)
 * then execute it immediately. Otherwise, fork a child process and
 * run the job in the context of the child. If the job is running in
 * the foreground, wait for it to terminate and then return.  Note:
 * each child process must have a unique process group ID so that our
 * background children don't receive SIGINT (SIGTSTP) from the kernel
 * when we type ctrl-c (ctrl-z) at the keyboard.  
*/

void eval(char *cmdline) {
    pid_t pid;
    char *argv[MAXARGS];
    int bg = parseline(cmdline, argv);
    if (!builtin_cmd(argv)) {
        sigset_t maskAll, prevMask;
        sigfillset(&maskAll);
        if ((pid = fork()) == 0) {
            setpgid(pid, pid);
            if (execve(argv[0], argv, environ) < 0) {
                printf("%s: Command not found\n", argv[0]);
                exit(0);
                //return;
            }
        }

        sigprocmask(SIG_BLOCK, &maskAll, &prevMask);

        if (bg) {
            printf("[%d] (%d) %s", pid2jid(pid), pid, cmdline);
            addjob(jobs, pid, BG, cmdline);
        } else {
            addjob(jobs, pid, FG, cmdline);
        }

        sigprocmask(SIG_SETMASK, &prevMask, NULL);

        if (!bg) waitfg(pid);
    }
    return;
}


/* 
 * builtin_cmd - If the user has typed a built-in command then execute
 *    it immediately.  
 *    done
 */
int builtin_cmd(char **argv) {
    if (!strcmp(argv[0], "quit")) {
        exit(0);
    }
    if (!strcmp(argv[0], "jobs")) {
        listjobs(jobs);
        return 1;
    }
    if (!(strcmp(argv[0], "fg") && strcmp(argv[0], "bg"))) {
        do_bgfg(argv);
        return 1;
    }
    return 0;     /* not a builtin command */
}

/* 
 * do_bgfg - Execute the builtin bg and fg commands
 */
void do_bgfg(char **argv) {
    int id;
    struct job_t *job;
    sigset_t maskAll, prevMask;
    sigfillset(&maskAll);
    char *strId = argv[1];
    
    if (!strId) {
        printf("%s command requires PID or %%jobid argument\n", argv[0]);
        return;
    }
    int valid  = ((strId[0] == '%')? valid_id(strId+1) : valid_id(strId));
    if(!valid){
         printf("%s argument must be a PID or %%jobid\n", argv[0]);
        return;
    }
    if (strId[0] == '%') {
        id = atoi(strId + 1);
        if (!getjobjid(jobs, id)) {
            printf("(%%%d): No such job\n", id);
            return;
        }
    } else {
        id = atoi(strId);
        if (!getjobpid(jobs, id)) {
            printf("(%d): No such process\n", id);
            return;
        }
    }
    if (id == 0) {
        printf("%s: argument must be a PID or %%jobid\n", argv[0]);
        return;
    }
    int bg = ((strcmp(argv[0], "bg") == 0) ? 1 : 0);
    sigprocmask(SIG_BLOCK, &maskAll, &prevMask);
    if (strId[0] == '%') {
        job = getjobjid(jobs, id);
    } else {
        job = getjobpid(jobs, id);
    }
    sigprocmask(SIG_SETMASK, &prevMask, NULL);

    if (bg) {
        printf("[%d] (%d) %s", job->jid, job->pid, job->cmdline);
        kill(-job->pid, SIGCONT);
        sigprocmask(SIG_BLOCK, &maskAll, &prevMask);
        job->state = BG;
        sigprocmask(SIG_SETMASK, &prevMask, NULL);
    } else {
        kill(-job->pid, SIGCONT);
        sigprocmask(SIG_BLOCK, &maskAll, &prevMask);
        job->state = FG;
        sigprocmask(SIG_SETMASK, &prevMask, NULL);
        waitfg(job->pid);
    }
    return;
}


/* 
 * waitfg - Block until process pid is no longer the foreground process
 */
void waitfg(pid_t pid) {
    sigset_t childMask ,prevMask;
    sigemptyset(&childMask);
    sigaddset(&childMask,SIGCHLD);
    sigprocmask(SIG_SETMASK,&childMask,&prevMask);
    struct job_t *job = getjobpid(jobs,pid);
    while (job->state == FG) sigsuspend(&prevMask);
    sigprocmask(SIG_SETMASK,&prevMask,NULL);
}

int valid_id(char* id){
    int length = strlen(id);
    for(int i = 0 ; i < length ; i++) if(id[i] < '0' || id[i] > '9') return 0;
    return 1;
}