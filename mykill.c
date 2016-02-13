/* Charles Gong, Manasa Tipparam
 * 1/30/16
 * Takes a process ID and S=sends a SIGUSR1 to that process.
 */

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <string.h>


int main(int argc, char **argv)
{
  // Charles and Manasa drove
  int arg;

  if(argc != 2){
    fprintf(stderr, "Usage: mykill <pid>\n");
    exit(-1);
  }

  arg = atoi(argv[1]);	

  if(arg == 0 && strcmp(argv[1], "0") != 0){
    exit(-1);
  }
  else {
	kill(arg, SIGUSR1);
  }
  return 0;
  // stopped driving
}
