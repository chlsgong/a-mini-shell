#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include "util.h"

/*
 * Handler for the SIGINT signal
 * prints nice try and then continues looping
 */
void sigint_handler(int sig) 
{
    ssize_t bytes; 
    const int STDOUT = 1; 
    bytes = write(STDOUT, "Nice Try.\n", 10); 
    if(bytes != 10) 
       exit(-999);
}

/*
 * First, print out the process ID of this process.
 *
 * Then, set up the signal handler so that ^C causes
 * the program to print "Nice try.\n" and continue looping.
 *
 * Finally, loop forever, printing "Still here\n" once every
 * second.
 */
int main(int argc, char **argv)
{	
	struct timespec req, remain, remain2;
	pid_t pid = getpid();

	printf("%d\n", pid);
	req.tv_sec = 1;
	signal(SIGINT, sigint_handler);

	while(1) {
		printf("Still here\n");
		if(nanosleep(&req, &remain) < 0) {
			nanosleep(&remain, &remain2);
		}
	}

  	return 0;
}
