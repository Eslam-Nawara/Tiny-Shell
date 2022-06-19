#ifndef HELPERS_H
#define HELPERS_H

void usage(void);

void unix_error(char *msg);

void app_error(char *msg);

int parseline(const char *cmdline, char **argv);


#endif
