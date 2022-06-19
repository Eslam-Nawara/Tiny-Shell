#include "signalhandlers.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include "jobs.h"


/*
 * Signal - wrapper for the sigaction function
 */
handler_t *Signal(int signum, handler_t *handler) {
    struct sigaction action, old_action;

    action.sa_handler = handler;
    sigemptyset(&action.sa_mask); /* block sigs of type being handled */
    action.sa_flags = SA_RESTART; /* restart syscalls if possible */

    if (sigaction(signum, &action, &old_action) < 0)
        unix_error("Signal error");
    return (old_action.sa_handler);
}


/*****************
 * Signal handlers
 *****************/

/*
 * sigquit_handler - The driver program can gracefully terminate the
 *    child shell by sending it a SIGQUIT signal.
 */
void sigquit_handler(int sig) {
    printf("Terminating after receipt of SIGQUIT signal\n");
    exit(1);
}


/* 
 * sigchld_handler - The kernel sends a SIGCHLD to the shell whenever
 *     a child job terminates (becomes a zombie), or stops because it
 *     received a SIGSTOP or SIGTSTP signal. The handler reaps all
 *     available zombie children, but doesn't wait for any other
 *     currently running children to terminate.  
 */
void sigchld_handler(int sig) {
    int oldErrno = errno;
    int states, pid;
    sigset_t maskAll, prevMask;
    sigfillset(&maskAll);

    sigprocmask(SIG_SETMASK, &maskAll, &prevMask);

    while ((pid = waitpid(-1, &states, WNOHANG | WUNTRACED)) > 0) {
        if (WIFEXITED(states)) deletejob(jobs, pid);
        else if (WIFSIGNALED(states)) {
            printf("Job [%d] (%d) terminated by signal %d\n", pid2jid(pid), pid, WTERMSIG(states));
            deletejob(jobs, pid);
        } else if (WIFSTOPPED(states)) {
            printf("Job [%d] (%d) stopped by signal %d\n", pid2jid(pid), pid, WSTOPSIG(states));
            struct job_t *job = getjobpid(jobs,pid);
            job -> state = ST;
        }
    }
    sigprocmask(SIG_SETMASK, &prevMask, NULL);

    errno = oldErrno;
    return;
}

/* 
 * sigint_handler - The kernel sends a SIGINT to the shell whenver the
 *    user types ctrl-c at the keyboard.  Catch it and send it along
 *    to the foreground job.  
 */
void sigint_handler(int sig) {
    int oldErrno = errno;
    sigset_t maskAll, prevMask;
    sigfillset(&maskAll);

    sigprocmask(SIG_SETMASK, &maskAll, &prevMask);
    kill(-fgpid(jobs), SIGINT);
    sigprocmask(SIG_SETMASK, &prevMask, NULL);

    errno = oldErrno;
    return;
}

/*
 * sigtstp_handler - The kernel sends a SIGTSTP to the shell whenever
 *     the user types ctrl-z at the keyboard. Catch it and suspend the
 *     foreground job by sending it a SIGTSTP.  
 */
void sigtstp_handler(int sig) {
    int oldErrno = errno;
    sigset_t maskAll, prevMask;
    sigfillset(&maskAll);

    sigprocmask(SIG_SETMASK, &maskAll, &prevMask);
    kill(-fgpid(jobs), SIGTSTP);
    sigprocmask(SIG_SETMASK, &prevMask, NULL);

    errno = oldErrno;
    return; 
}

/*********************
 * End signal handlers
 *********************/

