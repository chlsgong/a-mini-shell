/* Charles Gong, Manasa Tipparam
 * 1/28/16
 * Recursively computes a Fibonacci number based on the argument n.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

const int MAX = 13;

static void doFib(int n, int doPrint);


/*
 * unix_error - unix-style error routine.
 */
inline static void 
unix_error(char *msg)
{
    fprintf(stdout, "%s: %s\n", msg, strerror(errno));
    exit(1);
}


int main(int argc, char **argv)
{
  int arg;
  int print; // acts as a bool

  if(argc != 2){
    fprintf(stderr, "Usage: fib <num>\n");
    exit(-1);
  }

  if(argc >= 3){
    print = 1;
  }

  arg = atoi(argv[1]);
  if(arg < 0 || arg > MAX){
    fprintf(stderr, "number must be between 0 and %d\n", MAX);
    exit(-1);
  }

  doFib(arg, 1);

  return 0;
}

/* 
 * Recursively compute the specified number. If print is
 * true, print it. Otherwise, provide it to my parent process.
 *
 * NOTE: The solution must be recursive and it must fork
 * a new child for each call. Each process should call
 * doFib() exactly once.
 */
static void 
doFib(int n, int doPrint)
{
  pid_t child1, child2, retpid;
  int status1, status2, result = 0;

  // Charles drove here
  if(n == 0) {
    if(doPrint == 1)
      printf("0\n");
    exit(0);
  }
  else if(n == 1) {
    if(doPrint == 1)
      printf("1\n");
    exit(1);
  }
  else {
    child1 = fork();
    if(child1 == 0) // child process
      doFib(n-1, 0);
    else {
	    child2 = fork();
      if(child2 == 0) // child process
        doFib(n-2, 0);
    }
    // Charles stopped driving, Manasa starts driving
    if((retpid = waitpid(child1, &status1, 0)) > 0)
      if(WIFEXITED(status1))
        if((retpid = waitpid(child2, &status2, 0)) > 0)
          if(WIFEXITED(status2)) {
            result += (WEXITSTATUS(status1) + WEXITSTATUS(status2));
            if(doPrint == 1)
              printf("%d\n", result);
            exit(result);
          }
  }
  // Manasa stopped driving
}
