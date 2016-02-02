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
	kill(arg, 10);
  }
  return 0;
}
