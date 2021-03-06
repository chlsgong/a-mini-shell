/* Charles Gong, Manasa Tipparam
 * 1/30/16
 * Repeatedly prints until a kill signal is sent from another process.
 * Handles the ctrl-c event by not exiting the process.
 * Handles the SIGUSR1 signal by exiting the process.
 */

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
void sig_handler(int sig) 
{
	// Manasa started driving
    ssize_t bytes; 
    const int STDOUT = 1; 
    if(sig == 10) {
		bytes = write(STDOUT, "exiting\n", 8);
		if(bytes != SIGUSR1) {
       		exit(-999);
       	}
		exit(1);	
	} else {
		bytes = write(STDOUT, "Nice Try.\n", 10); 
    	if(bytes != 10) {
       		exit(-999);
       	}
	}
    // Manasa stopped driving
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
	// Charles started driving
	struct timespec req, remain, remain2;
	pid_t pid = getpid();

	printf("%d\n", pid);
	req.tv_sec = 1;
	Signal(SIGINT, sig_handler);
	Signal(SIGUSR1, sig_handler);

	while(1) {
		printf("Still here\n");
		if(nanosleep(&req, &remain) < 0) {
			nanosleep(&remain, &remain2);
		}
	}
  	return 0;
  	// Charles stopped driving
}
