# Tiny-Shell
  - This program is a simple Unix shell program that supports job control.
  - This is the 5th lab of [2015 Fall: 15-213 Introduction to Computer Systems](https://www.cs.cmu.edu/afs/cs/academic/class/15213-f15/www/schedule.html).
  - The program works as the following:
    - The shell takes command which can be a built-in command (quit, jobs, bg or fg) or a path of an executable file.
    - Parse the command and evaluate it.
    - Run the command if it is built-in or fork a child process to run it otherwise.
    - Reap all finished processes after terminating.
 ### To start write the following commands
  - `git clone https://github.com/Eslam-Nawara/Tiny-Shell.git`
  - `make`
  - `./tsh`
# Source
- `tsh.c`: contains the main function
- `helpers.c`: contains helper functions
- `jobs.c`: contains functions used to control the jobs list.
- `signalhandlers.c`: contains the customised signal handlers
- `tinyshell.c`: contains my implementation of the shell
- `tshref`: reference shell to check the shell behaviour
