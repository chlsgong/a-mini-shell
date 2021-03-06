/* 
 * msh - A mini shell program with job control
 * 
 * <Put your name and login ID here>
 * Manasa Tipparam
 * mt32855
 *
 * Charles Gong
 * hcg359
 * 
 * 2/7/16
 * A mini shell that executes a few commands or executes another program. Continually loops until a
 * kill signal is sent to the shell. Handles bad commands and invalid programs. Allows programs to be
 * run as background or foreground process. Adds the processes to a jobs list that can display the states
 * of such processes. Can also handle the stopping or termination of processes with SIGINT or SIGTSTP.
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
#include "util.h"
#include "jobs.h"


/* Global variables */
int verbose = 0;            /* if true, print additional output */

extern char **environ;      /* defined in libc */
static char prompt[] = "msh> ";    /* command line prompt (DO NOT CHANGE) */
static struct job_t jobs[MAXJOBS]; /* The job list */
/* End global variables */


/* Function prototypes */

/* Here are the functions that you will implement */
void eval(char *cmdline);
int builtin_cmd(char **argv);
void do_bgfg(char **argv);
void waitfg(pid_t pid);

void sigchld_handler(int sig);
void sigtstp_handler(int sig);
void sigint_handler(int sig);

/* Here are helper routines that we've provided for you */
void usage(void);
void sigquit_handler(int sig);



/*
 * main - The shell's main routine 
 */
int main(int argc, char **argv) 
{
    char c;
    char cmdline[MAXLINE];
    int emit_prompt = 1; /* emit prompt (default) */

    /* Redirect stderr to stdout (so that driver will get all output
     * on the pipe connected to stdout) */
    dup2(1, 2);

    /* Parse the command line */
    while ((c = getopt(argc, argv, "hvp")) != EOF) {
        switch (c) {
        case 'h':             /* print help message */
            usage();
	    break;
        case 'v':             /* emit additional diagnostic info */
            verbose = 1;
	    break;
        case 'p':             /* don't print a prompt */
            emit_prompt = 0;  /* handy for automatic testing */
	    break;
	default:
            usage();
	}
    }

    /* Install the signal handlers */

    /* These are the ones you will need to implement */
    Signal(SIGINT,  sigint_handler);   /* ctrl-c */
    Signal(SIGTSTP, sigtstp_handler);  /* ctrl-z */
    Signal(SIGCHLD, sigchld_handler);  /* Terminated or stopped child */

    /* This one provides a clean way to kill the shell */
    Signal(SIGQUIT, sigquit_handler); 

    /* Initialize the job list */
    initjobs(jobs);

    /* Execute the shell's read/eval loop */
    while (1) {

	/* Read command line */
	if (emit_prompt) {
	    printf("%s", prompt);
	    fflush(stdout);
	}
	if ((fgets(cmdline, MAXLINE, stdin) == NULL) && ferror(stdin))
	    app_error("fgets error");
	if (feof(stdin)) { /* End of file (ctrl-d) */
	    fflush(stdout);
	    exit(0);
	}

	/* Evaluate the command line */
	eval(cmdline);
	fflush(stdout);
	fflush(stdout);
    } 

    exit(0); /* control never reaches here */
}
  
/* 
 * eval - Evaluate the command line that the user has just typed in
 * 
 * If the user has requested a built-in command (quit, jobs, bg or fg)
 * then execute it immediately. Otherwise, fork a child process and
 * run the job in the context of the child. If the job is running in
 * the foreground, wait for it to terminate and then return.  Note:
 * each child process must have a unique process group ID so that our
 * background children don't receive SIGINT (SIGTSTP) from the kernel
 * when we type ctrl-c (ctrl-z) at the keyboard.  
*/
void eval(char *cmdline) 
{ 
    // Manasa started driving
	pid_t child;
    sigset_t mask;
    char* argv[MAXARGS];
    int bg, jid, builtin;

    sigemptyset(&mask);
    sigaddset(&mask, SIGCHLD);
    sigaddset(&mask, SIGTSTP);
    sigaddset(&mask, SIGINT);
    sigprocmask(SIG_BLOCK, &mask, NULL);

    if(cmdline[0] == '\n')
        return;

    bg = parseline(cmdline, argv); // Manasa stopped driving, Charles started driving
    builtin = builtin_cmd(argv);
    if(builtin)
        sigprocmask(SIG_UNBLOCK, &mask, NULL);
    else { // not a built in command
        child = fork();
        if(child == 0) { // child process
            setpgid(0, 0);
            sigprocmask(SIG_UNBLOCK, &mask, NULL);
            if(execve(argv[0], argv, environ) < 0) {
                fprintf(stderr, "%s: Command not found\n", argv[0]);
                exit(-1);
            }
        }
        // parent process
        if(bg == 0) { // if a foreground job, wait for child process to finish
            addjob(jobs, child, FG, cmdline);
            sigprocmask(SIG_UNBLOCK, &mask, NULL);
            waitfg(child);
        }
        else { // if a background job
            addjob(jobs, child, BG, cmdline);
            jid = pid2jid(jobs, child); // get job from pid then prints it
            printf("[%d] (%d) %s", jid, child, cmdline);
            sigprocmask(SIG_UNBLOCK, &mask, NULL);
        }
    }
    return; // Charles stopped driving
}


/* 
 * builtin_cmd - If the user has typed a built-in command then execute
 *    it immediately.  
 * Return 1 if a builtin command was executed; return 0
 * if the argument passed in is *not* a builtin command.
 */
int builtin_cmd(char **argv) 
{
    // Manasa started driving
	char* quit = "quit";
    char* jobsCmd = "jobs";
    char* bg = "bg";
    char* fg = "fg";

    if(strcmp(argv[0], jobsCmd) == 0) { // lists jobs
        listjobs(jobs);
        return 1;
    }
    if(strcmp(argv[0], quit) == 0) { // quit command
        exit(0);
    }
    if(strcmp(argv[0], bg) == 0) { // restart as backgroudn process
        do_bgfg(argv);
        return 1;
    }
    if(strcmp(argv[0], fg) == 0) { // restart as foreground process
        do_bgfg(argv);
        return 1;
    }
    return 0;     /* not a builtin command */
    // Manasa stopped driving
}

/* 
 * do_bgfg - Execute the builtin bg and fg commands
 */
void do_bgfg(char **argv) 
{   
    // Charles and Manasa drove
    pid_t pid;
    struct job_t* job;
    char* arg2;
    char jidChar[3];
    int count = 0, byte, jid;

    if(!argv[1]) { // get jid or pid
        fprintf(stderr, "%s command requires PID or %%jobid argument\n", argv[0]);
        return;
    }
    arg2 = argv[1];

    if(arg2[0] == '%') { // if jid is passed in
        while(arg2[++count]) { // check if arg is valid
            byte = arg2[count];
            if(!isdigit(byte)) {
                fprintf(stderr, "%s: argument must be a PID or %%jobid\n", argv[0]);
                return;
            }
        }
        strcpy(jidChar, arg2+1);
        jid = atoi(jidChar);
        if((job = getjobjid(jobs, jid)) == NULL) {
            fprintf(stderr, "%%%d: No such job\n", jid);
            return;
        }
        pid = job->pid;
    }
    else { // else if pid is passed in
        while(arg2[count++]) { // check if arg is valid
            byte = arg2[count-1];
            if(!isdigit(byte)) {
                fprintf(stderr, "%s: argument must be a PID or %%jobid\n", argv[0]);
                return;
            }
        }
        pid = atoi(arg2);
        if(!(jid = pid2jid(jobs, pid))) {
            fprintf(stderr, "(%d): No such process\n", pid);
            return;
        }
        job = getjobpid(jobs, pid);
    }

    if(strcmp(argv[0], "bg") == 0) { // run as bg
        if(job->state != BG) {
            job->state = BG;
            kill(-pid, SIGCONT);
            printf("[%d] (%d) %s", jid, pid, job->cmdline);
        }
    }
    else { // run as fg
        if(job->state != FG) {
            job->state = FG;
            kill(-pid, SIGCONT);
            waitfg(pid);
        }
    }
    return;
    // stopped driving
}

/* 
 * waitfg - Block until process pid is no longer the foreground process
 */
void waitfg(pid_t pid)
{   
    // Charles drove
    sigset_t waitMask;
    sigemptyset(&waitMask);
    sigaddset(&waitMask, SIGCHLD);
    sigprocmask(SIG_BLOCK, &waitMask, NULL);

    while(fgpid(jobs) == pid) {
        sigdelset(&waitMask, SIGCHLD);
        sigsuspend(&waitMask);
        sigaddset(&waitMask, SIGCHLD);
    }
    sigprocmask(SIG_UNBLOCK, &waitMask, NULL);
    return;
    // Charles stopped driving
}

/*****************
 * Signal handlers
 *****************/

/* 
 * sigchld_handler - The kernel sends a SIGCHLD to the shell whenever
 *     a child job terminates (becomes a zombie), or stops because it
 *     received a SIGSTOP or SIGTSTP signal. The handler reaps all
 *     available zombie children, but doesn't wait for any other
 *     currently running children to terminate.  
 */
void sigchld_handler(int sig) 
{   
    // Manasa started driving
    pid_t pid;
    struct job_t* job;
    int status, jid;
    char msg[60];
    errno = 0;

    // WNOHANG|WUNTRACED returns 0 if no children has stopped or terminated
    // else return pid
    while((pid = waitpid(-1, &status, WNOHANG|WUNTRACED)) > 0) {
        if(status >= 0) {
            if(WIFSTOPPED(status)) {
                jid = pid2jid(jobs, pid);
                sprintf(msg, "Job [%d] (%d) stopped by signal 20", jid, pid);
                puts(msg);
                job = getjobpid(jobs, pid);
                job->state = ST;
            }
            else if(WIFSIGNALED(status)) {  
                jid = pid2jid(jobs, pid);
                sprintf(msg, "Job [%d] (%d) terminated by signal 2", jid, pid);
                puts(msg);
                deletejob(jobs, pid);
            }
            else if(WIFEXITED(status)) {
                deletejob(jobs, pid);
            }
        }
    } // Manasa stopped driving, Charles started driving
    if(errno == EINTR) // if handler interrupted restart handler
        sigchld_handler(sig);
    else if(errno != ECHILD && pid != 0)
        unix_error("waitpid error");
    return; // Charles stopped driving
}

/* 
 * sigint_handler - The kernel sends a SIGINT to the shell whenver the
 *    user types ctrl-c at the keyboard.  Catch it and send it along
 *    to the foreground job.  
 */
void sigint_handler(int sig) 
{
    // Charles drove
    pid_t pid;
    if((pid = fgpid(jobs)) > 0)
        kill(-pid, SIGKILL); // terminated job
    return;
    // Charles stopped driving
}

/*
 * sigtstp_handler - The kernel sends a SIGTSTP to the shell whenever
 *     the user types ctrl-z at the keyboard. Catch it and suspend the
 *     foreground job by sending it a SIGTSTP.  
 */
void sigtstp_handler(int sig) 
{
    // Charles drove
    pid_t pid;
    if((pid = fgpid(jobs)) > 0)
        kill(-pid, SIGTSTP); // stops job
    return;
    // Charles stopped driving 
}

/*********************
 * End signal handlers
 *********************/



/***********************
 * Other helper routines
 ***********************/

/*
 * usage - print a help message
 */
void usage(void) 
{
    printf("Usage: shell [-hvp]\n");
    printf("   -h   print this message\n");
    printf("   -v   print additional diagnostic information\n");
    printf("   -p   do not emit a command prompt\n");
    exit(1);
}

/*
 * sigquit_handler - The driver program can gracefully terminate the
 *    child shell by sending it a SIGQUIT signal.
 */
void sigquit_handler(int sig) 
{
    ssize_t bytes;
    const int STDOUT = 1;
    bytes = write(STDOUT, "Terminating after receipt of SIGQUIT signal\n", 45);
    if(bytes != 45)
       exit(-999);
    exit(1);
}
