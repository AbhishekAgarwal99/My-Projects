/* 
 * tsh - A tiny shell program with job control
 * 
 * <Abhishek Agarwal Apa5509@psu.edu>
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

/* Misc manifest constants */
#define MAXLINE    1024   /* max line size */
#define MAXARGS     128   /* max args on a command line */
#define MAXJOBS      16   /* max jobs at any point in time */
#define MAXJID    1<<16   /* max job ID */

/* Job states */
#define UNDEF 0 /* undefined */
#define FG 1    /* running in foreground */
#define BG 2    /* running in background */
#define ST 3    /* stopped */

/* 
 * Jobs states: FG (foreground), BG (background), ST (stopped)
 * Job state transitions and enabling actions:
 *     FG -> ST  : ctrl-z
 *     ST -> FG  : fg command
 *     ST -> BG  : bg command
 *     BG -> FG  : fg command
 * At most 1 job can be in the FG state.
 */

/* Global variables */
extern char **environ;      /* defined in libc */
char prompt[] = "tsh> ";    /* command line prompt (DO NOT CHANGE) */
int verbose = 0;            /* if true, print additional output */
int nextjid = 1;            /* next job ID to allocate */
char sbuf[MAXLINE];         /* for composing sprintf messages */

struct job_t {              /* The job struct */
  pid_t pid;              /* job PID */
  int jid;                /* job ID [1, 2, ...] */
  int state;              /* UNDEF, BG, FG, or ST */
  char cmdline[MAXLINE];  /* command line */
};
struct job_t jobs[MAXJOBS]; /* The job list */
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
int parseline(const char *cmdline, char **argv); 
void sigquit_handler(int sig);

void clearjob(struct job_t *job);
void initjobs(struct job_t *jobs);
int maxjid(struct job_t *jobs); 
int addjob(struct job_t *jobs, pid_t pid, int state, char *cmdline);
int deletejob(struct job_t *jobs, pid_t pid); 
pid_t fgpid(struct job_t *jobs);
struct job_t *getjobpid(struct job_t *jobs, pid_t pid);
struct job_t *getjobjid(struct job_t *jobs, int jid); 
int pid2jid(pid_t pid); 
void listjobs(struct job_t *jobs);

void usage(void);
void unix_error(char *msg);
void app_error(char *msg);
typedef void handler_t(int);
handler_t *Signal(int signum, handler_t *handler);

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
/* For my eval function I first started with the code given on page 755
   of the textbook. The first thing thateval checked is if the user
   inputted nothing, then it would just return in the tsh. Next, I checked
   if the command the user entered was a built in command because the code
   for the built in command is already impleented. So then the eval function
   uses a fork to check if the command does something in the shell. If the
   job is in the foreground, I then add it to the foreground, and wait for
   the job to finish in the foreground using waitfg(). If the job is in the background,
   I add the job to the background and print out the job id, pid and cmdline.
   The sigpromask is used to deal with race conditions, and most of it was
   learned from the textbook and the lecture slides 11-01. I first define
   the prev mask, and add the signals SIGINT, SIGCHLD, and SIGTSTP to the 
   sigset. I then block all signals before the fork, and then I unblock the
   signals after all the add jobs and before the execve function gets called. 
 *For the signaling part of eval, I used a lot of TA help.
 For the forking error part of the eval, I basically initilize pid to be -1, and if fork fails for some 
 reason, the pid would remain -1, therefore causing a forking error. - I got this from the TA's.
 */
void eval(char *cmdline) 
{
  sigset_t mask, prev_mask;
  char *argv[MAXARGS];
  char buf[MAXLINE];
  int bg;
  pid_t pid = -1;
  strcpy(buf, cmdline);
  bg = parseline(buf, argv); 
  if (argv[0] == NULL)
    return;

  if (!builtin_cmd(argv)) {
    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);
    sigaddset(&mask, SIGCHLD);
    sigaddset(&mask, SIGTSTP);

    sigprocmask(SIG_BLOCK, &prev_mask, NULL);

    if ((pid = fork()) == 0) {
      sigprocmask(SIG_SETMASK, &prev_mask, NULL);
      setpgid(0,0);
      if (execve(argv[0], argv, environ) < 0) {

        printf("%s: Command not found\n",argv[0]);
        exit(0);
      }
    }
    if(pid == -1)
    {
      unix_error("Forking error");
    }
    if (!bg) {
      addjob(jobs,pid,FG,cmdline);
      sigprocmask(SIG_SETMASK, &prev_mask, NULL);
      waitfg(pid);
    }
    else{
      addjob(jobs, pid, BG, cmdline);
      sigprocmask(SIG_SETMASK, &prev_mask, NULL);
      printf("[%d] (%d) %s",(pid2jid(pid)), pid, cmdline);
    }
  }
  return;

}
/* 
 * parseline - Parse the command line and build the argv array.
 * 
 * Characters enclosed in single quotes are treated as a single
 * argument.  Return true if the user has requested a BG job, false if
 * the user has requested a FG job.  
 */
int parseline(const char *cmdline, char **argv) 
{
  static char array[MAXLINE]; /* holds local copy of command line */
  char *buf = array;          /* ptr that traverses command line */
  char *delim;                /* points to first space delimiter */
  int argc;                   /* number of args */
  int bg;                     /* background job? */

  strcpy(buf, cmdline);
  buf[strlen(buf)-1] = ' ';  /* replace trailing '\n' with space */
  while (*buf && (*buf == ' ')) /* ignore leading spaces */
    buf++;

  /* Build the argv list */
  argc = 0;
  if (*buf == '\'') {
    buf++;
    delim = strchr(buf, '\'');
  }
  else {
    delim = strchr(buf, ' ');
  }

  while (argc < MAXARGS-1 && delim) {
    argv[argc++] = buf;
    *delim = '\0';
    buf = delim + 1;
    while (*buf && (*buf == ' ')) /* ignore spaces */
      buf++;

    if (*buf == '\'') {
      buf++;
      delim = strchr(buf, '\'');
    }
    else {
      delim = strchr(buf, ' ');
    }
  }
  if (delim) {
    fprintf(stderr, "Too many arguments.\n");
    argc = 0; //treat it as an empty line.
  }
  argv[argc] = NULL;

  if (argc == 0)  /* ignore blank line */
    return 1;

  /* should the job run in the background? */
  if ((bg = (*argv[argc-1] == '&')) != 0) {
    argv[--argc] = NULL;
  }
  return bg;
}

/* 
 * builtin_cmd - If the user has typed a built-in command then execute
 *    it immediately.  
 */
/*
 * The built in command function checks to see specific commands are typed 
 * into the shell. So the first command quit exits the shell by using exit(0).
 * It also returns 1 becuase it is a built in command. The next command I check
 * is the fg command which menas if fg is typed into the shell, then the bgfg cmd
 * will run. Same goes if the user types in the bg command. If the user types in 
 * jobs then the jobs will be displayed. Finally, if the user types in an & nothing
 * will happen. 
 */
int builtin_cmd(char **argv) 
{
  if(strcmp("quit", argv[0]) == 0){
    exit(0);
    return 1;
  }
  if(strcmp("fg", argv[0]) == 0){
    do_bgfg(argv);
    return 1;
  }
  if(strcmp("bg", argv[0]) == 0){
    do_bgfg(argv);
    return 1;
  }
  if(strcmp("jobs", argv[0]) == 0){
    listjobs(jobs);
    return 1;
  }
  if(strcmp("&", argv[0]) == 0)
  {
    return 1;
  }

  return 0;     /* not a builtin command */
}

/* 
 * do_bgfg - Execute the builtin bg and fg commands
 */

/*
 * For the do_bgfg method, I started by defining a pid and and integer temp id.
 *The first thing I checked was id the argument it null after bg or fg is typed then print that the command needs a valid jobid argument.
 *I also had to return so the shell would keep going. Next, I checked if the value infront of the bg/fg was a percent sign.
 *This would indicate that the given value is a job.
 *I also had to perform an addisional check to make sure that the value after the percent sign was a digit and not a invalid value by using isdigit.
 *So if I found the value entered was a job id, I then converted the string typed in after the % sign to an integer, and checked if if it was a valid job id.
 *If it was a valid job id, I set the pid variable to the pid that corresponded with that job.
 *On the otherside, if there wasn't a % sign after the bg/fg in the shell, I know that the value entered was a process id, so I did the same check to see if the value was a digit, and if it was I set the value of pid to the value the user entered.
 *Now that I have the pid, I then checked if the user typed in fg or background, If they typed bg, I sent a SIGCONT signal to the bg of that process id.
 I also then changed the state of the job to be in the BG.
 I did the same thing for if the job was in the fg except I used the waitfg command after I changed the state of job to make sure the job was finished.
 I used a lot of TA help to get this done right.
 */
void do_bgfg(char **argv) 
{
  int id;
  pid_t pid;

  if(argv[1] == NULL)
  {
    printf("%s command requires PID or %%jobid argument\n", argv[0]);
    return;
  }




  if(argv[1][0]== '%'){
    if (!isdigit(argv[1][1])){
      printf("%s: argument must be a PID or %%jobid\n", argv[0]);
      return;
    }

    id = atoi(&argv[1][1]);
    if(getjobjid(jobs,id) == NULL){
      printf("%s: No such job\n", argv[1]);
      return;
    }
    pid = (getjobjid(jobs, id)) -> pid;
  }
  else{
    if (!isdigit(argv[1][0])){
      printf("%s: argument must be a PID or %%jobid\n", argv[0]);
      return;
    }
    id = atoi(argv[1]);
    pid = id; 
  }


  if(getjobpid(jobs,pid) == NULL){
    printf("(%s): No such process\n", argv[1]);
    return;
  }

  if(strcmp("bg", argv[0])== 0){
    kill(-pid, SIGCONT);
    getjobpid(jobs, pid)->state =  BG;
    printf("[%d] (%d) %s",(pid2jid(pid)), pid, (getjobpid(jobs,pid)-> cmdline));

  }
  else  if(strcmp("fg", argv[0])== 0){
    kill(-pid, SIGCONT);
    getjobpid(jobs, pid)->state =  FG;
    waitfg(pid);

  }


  return;
}

/* 
 * waitfg - Block until process pid is no longer the foreground process
 */
/*The waitfg command I implemented used an infinite while loop, to make sure waitfg would always execute.
 * I checked if the while to loop was running if the pid given matched the pid of the job currently running in the foreground.
 * If they didn't match the loop would break and wait fg would return meaning that the job was done running in the foreground.
 * Also, I had an addisional check if the pid == 0 to just return. 
 */
void waitfg(pid_t pid)
{
  while(1)
  {
    if(pid != fgpid(jobs)){
      break;
    }
    if(pid == 0)
    {
      return;
    }
  }

  return;
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

/*
 * To start the sigchld handler I used the starter code on the practice exam.
 * I realized the while loop was checking any children which still had to be reaped so now my handler had to check for the status of the job currently.
 * I did that by checking the status's, so if the status was WIFEXITED I dleted the job.
 * If the status was that it was receiving a termination signal, I would print the signal and then delete the job.
 * If the status was the it was being stopped, I would then print what signal stopped the job, and then change the state of the job to stopped.
 * I used a lot of TA help to get this right. 
 */
void sigchld_handler(int sig) 
{
  int status;
  pid_t pid;
  while ((pid = waitpid(-1, &status, WUNTRACED|WNOHANG))>0) {
    if (WIFEXITED(status)){
      deletejob(jobs, pid);
    }
    if (WIFSIGNALED(status)) {
      printf("Job [%d] (%d) terminated by signal %d\n", pid2jid(pid), pid, WTERMSIG(status));
      deletejob(jobs,pid);
    }
    if(WIFSTOPPED(status)){
      printf("Job [%d] (%d) stopped by signal %d\n", pid2jid(pid), pid, WSTOPSIG(status));
      getjobpid(jobs, pid)->state =  ST;
    }
    return;
  }
}
/* 
 * sigint_handler - The kernel sends a SIGINT to the shell whenver the
 *    user types ctrl-c at the keyboard.  Catch it and send it along
 *    to the foreground job.  
 */
/*
 * For sigint handler, I set the pid equal to the current pid in the foreground, and then I sent a SIGINT signal to that pid to terminate the process. 
 */
void sigint_handler(int sig) 
{
  pid_t pid;
  pid = fgpid(jobs);
  kill(-pid,SIGINT);
  return;
}

/*
 * sigtstp_handler - The kernel sends a SIGTSTP to the shell whenever
 *     the user types ctrl-z at the keyboard. Catch it and suspend the
 *     foreground job by sending it a SIGTSTP.  
 */

/*
 * For the SIGTSPT signal, I set the pid equal to whatever job's pid which was running in the foreground, and then I sent a SIGTSTP signal to that job.
 */
void sigtstp_handler(int sig) 
{
  pid_t pid;
  pid = fgpid(jobs);

  kill(-pid, SIGTSTP);
  return;
}

/*********************
 * End signal handlers
 *********************/

/***********************************************
 * Helper routines that manipulate the job list
 **********************************************/

/* clearjob - Clear the entries in a job struct */
void clearjob(struct job_t *job) {
  job->pid = 0;
  job->jid = 0;
  job->state = UNDEF;
  job->cmdline[0] = '\0';
}

/* initjobs - Initialize the job list */
void initjobs(struct job_t *jobs) {
  int i;

  for (i = 0; i < MAXJOBS; i++)
    clearjob(&jobs[i]);
}

/* maxjid - Returns largest allocated job ID */
int maxjid(struct job_t *jobs) 
{
  int i, max=0;

  for (i = 0; i < MAXJOBS; i++)
    if (jobs[i].jid > max)
      max = jobs[i].jid;
  return max;
}

/* addjob - Add a job to the job list */
int addjob(struct job_t *jobs, pid_t pid, int state, char *cmdline) 
{
  int i;

  if (pid < 1)
    return 0;

  for (i = 0; i < MAXJOBS; i++) {
    if (jobs[i].pid == 0) {
      jobs[i].pid = pid;
      jobs[i].state = state;
      jobs[i].jid = nextjid++;
      if (nextjid > MAXJOBS)
        nextjid = 1;
      strcpy(jobs[i].cmdline, cmdline);
      if(verbose){
        printf("Added job [%d] %d %s\n", jobs[i].jid, jobs[i].pid, jobs[i].cmdline);
      }
      return 1;
    }
  }
  printf("Tried to create too many jobs\n");
  return 0;
}

/* deletejob - Delete a job whose PID=pid from the job list */
int deletejob(struct job_t *jobs, pid_t pid) 
{
  int i;

  if (pid < 1)
    return 0;

  for (i = 0; i < MAXJOBS; i++) {
    if (jobs[i].pid == pid) {
      clearjob(&jobs[i]);
      nextjid = maxjid(jobs)+1;
      return 1;
    }
  }
  return 0;
}

/* fgpid - Return PID of current foreground job, 0 if no such job */
pid_t fgpid(struct job_t *jobs) {
  int i;

  for (i = 0; i < MAXJOBS; i++)
    if (jobs[i].state == FG)
      return jobs[i].pid;
  return 0;
}

/* getjobpid  - Find a job (by PID) on the job list */
struct job_t *getjobpid(struct job_t *jobs, pid_t pid) {
  int i;

  if (pid < 1)
    return NULL;
  for (i = 0; i < MAXJOBS; i++)
    if (jobs[i].pid == pid)
      return &jobs[i];
  return NULL;
}

/* getjobjid  - Find a job (by JID) on the job list */
struct job_t *getjobjid(struct job_t *jobs, int jid) 
{
  int i;

  if (jid < 1)
    return NULL;
  for (i = 0; i < MAXJOBS; i++)
    if (jobs[i].jid == jid)
      return &jobs[i];
  return NULL;
}

/* pid2jid - Map process ID to job ID */
int pid2jid(pid_t pid) 
{
  int i;

  if (pid < 1)
    return 0;
  for (i = 0; i < MAXJOBS; i++)
    if (jobs[i].pid == pid) {
      return jobs[i].jid;
    }
  return 0;
}

/* listjobs - Print the job list */
void listjobs(struct job_t *jobs) 
{
  int i;

  for (i = 0; i < MAXJOBS; i++) {
    if (jobs[i].pid != 0) {
      printf("[%d] (%d) ", jobs[i].jid, jobs[i].pid);
      switch (jobs[i].state) {
        case BG: 
          printf("Running ");
          break;
        case FG: 
          printf("Foreground ");
          break;
        case ST: 
          printf("Stopped ");
          break;
        default:
          printf("listjobs: Internal error: job[%d].state=%d ", 
              i, jobs[i].state);
      }
      printf("%s", jobs[i].cmdline);
    }
  }
}
/******************************
 * end job list helper routines
 ******************************/


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
 * unix_error - unix-style error routine
 */
void unix_error(char *msg)
{
  fprintf(stdout, "%s: %s\n", msg, strerror(errno));
  exit(1);
}

/*
 * app_error - application-style error routine
 */
void app_error(char *msg)
{
  fprintf(stdout, "%s\n", msg);
  exit(1);
}

/*
 * Signal - wrapper for the sigaction function
 */
handler_t *Signal(int signum, handler_t *handler) 
{
  struct sigaction action, old_action;

  action.sa_handler = handler;  
  sigemptyset(&action.sa_mask); /* block sigs of type being handled */
  action.sa_flags = SA_RESTART; /* restart syscalls if possible */

  if (sigaction(signum, &action, &old_action) < 0)
    unix_error("Signal error");
  return (old_action.sa_handler);
}

/*
 * sigquit_handler - The driver program can gracefully terminate the
 *    child shell by sending it a SIGQUIT signal.
 */
void sigquit_handler(int sig) 
{
  printf("Terminating after receipt of SIGQUIT signal\n");
  exit(1);
}



