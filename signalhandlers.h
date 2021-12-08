#ifndef SIGNALHANDLERS_H
#define SIGNALHANDLERS_H

void sigquit_handler(int sig);

void sigchld_handler(int sig);

void sigtstp_handler(int sig);

void sigint_handler(int sig);

handler_t *Signal(int signum, handler_t *handler);

#endif