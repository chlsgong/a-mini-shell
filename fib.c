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
  int print = 0; // acts as a bool

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

  doFib(arg, print);

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
doFib(int n, int doPrint) // 0 is false
{
  printf("test\n");
  pid_t child1, child2, retpid;
  int status1 = 0, status2 = 0;
  // if(n == 0)
  //   return 0;
  // else if(n == 1)
  //   return 1;
  // else {
  //   return doFib(n-1, doPrint) + doFib(n-2, doPrint);
  // }

  // Charles and Manasa drove here
  if(n == 0) {
     printf("Exit at 0\n");
    exit(0);
  }
  if(n < 3) {
     printf("Exit at 1/2\n");
    exit(1);

  }


  else {
    child1 = fork();
    if(child1 == 0) // child process
    {
      printf("child 1 fork\n"); 
      doFib(n-1, doPrint);
    }
    else {
      child2 = fork();
      if(child2 == 0) // child process
        doFib(n-2, doPrint);
    }
    if((retpid = waitpid(child1, &status1, 0)) > 0)
      if(WIFEXITED(status1))
        if((retpid = waitpid(child2, &status2, 0)) > 0)
          if(WIFEXITED(status2))
            printf("%d\n", status1 + status2);
  }
}


