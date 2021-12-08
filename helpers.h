#ifndef HELPERS_H
#define HELPERS_H

/* Here are helper routines that we've provided for you */

void usage(void);

void unix_error(char *msg);

void app_error(char *msg);

typedef void handler_t(int);

int parseline(const char *cmdline, char **argv);


#endif