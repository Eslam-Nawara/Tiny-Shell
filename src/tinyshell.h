#ifndef TINYSHELL_H
#define TINYSHELL_H

#include <sys/types.h>

/* Here are the functions that you will implement */
void eval(char *cmdline);

int builtin_cmd(char **argv);

void do_bgfg(char **argv);

void waitfg(pid_t pid);

#endif
