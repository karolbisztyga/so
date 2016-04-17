#include "funcs.h"
#include <sys/resource.h>
/* -------------------------------------------------------------------------------------- */

int errorHandler(char* title) {
    printf("error %s: %s\n", title, strerror(errno));
    return errno;
}

int main(void)
{
  int res;
  char cmd[MAXCMD];
  struct cmdlist cmds;
  struct rlimit rl;
 
  /* setting up the initial values of the structure  */
  /* this should be done every time when new object is created */
  setupnewcommand(&cmds);


  rl.rlim_cur = 50;
  rl.rlim_max = 50;

  setrlimit(RLIMIT_FSIZE, &rl);
 
  /* Setting up file size limit for this process and for the child processes */
 
  /* main loop of the shell */
  /* this loop proceses command wirtten by user in the following steps: */
  /* 1. Displaying command prompt
     2. Reading command from the std input
     3. Parsing this command
     4. Executing parsed commands
     5. Dealocating used memory
   */
  while(1){
    printprompt();
    res = readcmd(cmd, MAXCMD);
    if(res == RESERROR) continue;
    /*if(res == RESBREAK) exit(0);*/
    res = parsecmd(cmd, MAXCMD, &cmds);
    /*printparsedcmds(&cmds);*/
    executecmds(&cmds);
    dealocate(&cmds);
  }
 
  return 0;
}
/* -------------------------------------------------------------------------------------- */
 
/* This function sets up the initial values of the fileds in the cmdlist structure  */
void setupnewcommand(struct cmdlist* __cmd)
{
  if(__cmd == NULL)
    return;
 
  __cmd->next = NULL;           /* in order to know where is the end of the list */
  __cmd->argv = NULL;           /* see realloc(3) */
  __cmd->conjuction = CONJUD;
  __cmd->argc = 0;
}
/* -------------------------------------------------------------------------------------- */
 
/* This function adds the NULL pointer to the end of the argument list  */
/* For more details see exec(3) */
int setupparsedcommand(struct cmdlist* __cmd)
{
  /* Checking id the argument is not NULL */
  if(__cmd == NULL){
    printf("Null pointer at setupparsedcommand.");
    return RESERROR;
  }
 
  __cmd->argc++;
  __cmd->argv = (char**)realloc(__cmd->argv, __cmd->argc*sizeof(char*));  /* Adding new pointer  */
  if(__cmd->argv == NULL)                                                 /* Checking is allocation was OK  */
    return RESERROR;
  __cmd->argv[__cmd->argc-1] = NULL;                                      /* Setting NULL poniter  */
 
  return RESSUCCESS;
}
/* -------------------------------------------------------------------------------------- */
 
/* 2. Reading command from the std input */
int readcmd(char* __buf, int __bufsize)
{
  /* Reading command from the std input  */
  if(fgets(__buf, __bufsize, stdin) == NULL){
    printf("Error while reading -- try again!");
  }
 
  /* If the std input buffer is not empty */
  /* In this case we did not read all the data from the buffer, 
     so the incomplete command should not be executed
     and that is why we ignore it */
  if(strchr(__buf, '\n') == NULL){
    while(1){
      char c = getchar();
      if(c == '\n')
  break;
      else if((c == EOF) && (ferror(stdin)))
  return RESERROR;
    }
    printf("Line is too long -- the command will be ignored.\nMaximal size is %d.", __bufsize-2);
    return RESERROR;
  }
  return RESSUCCESS;
}
/* -------------------------------------------------------------------------------------- */
 
/* 3. Parsing this command */
int parsecmd(char* __buf, int __bufsize, struct cmdlist* __head)
{
  char* cmd = __buf;                  /* String that must be parsed  */
  char* word;                         /* String between white characters  */
  struct cmdlist* curr = __head;
  int nextConjution;
  struct cmdlist* next;
 
  /* Reading next word - read strtok(3)  */
  nextConjution = CONJUD;
  while((word = strtok(cmd, " \t\n")) != NULL){
    curr->argc++;
    curr->argv = (char**)realloc(curr->argv, sizeof(char*)*curr->argc);   /* memory reallocation - needed for new argument  */
    if(curr->argv == NULL){
      printf("Error while allocating memory!");
      return RESERROR;
    }
    if(strcmp(word, "||") == 0 || strcmp(word, "&&") == 0) {
      curr->conjuction = nextConjution;
      if(strcmp(word, "||") == 0)nextConjution = CONJOR;
      else nextConjution = CONJAND;

      next = malloc(sizeof(struct cmdlist));

      setupparsedcommand(curr);
      setupnewcommand(next);
      next->conjuction = nextConjution;
      curr->next = next;
      curr = curr->next;
    } else {
      curr->argv[curr->argc-1] = word;/* Storing new argument in the argument vector in our structure */
      cmd = NULL;
    }
  }
 
  /* Setting up parsed command -- the NULL pointer at the end of the parameters list must added  */
  if(setupparsedcommand(curr) == RESERROR){                               
    printf("Error while setting up parsed command.");
    return RESERROR;
  }
  return RESSUCCESS;
}
/* -------------------------------------------------------------------------------------- */
 
/* 4. Executing parsed commands */
int executecmds(struct cmdlist* __head)
{
  int f, e, procres, status, executeNext;
  struct cmdlist* curr = __head;
 
  executeNext = 1;
  while(curr != NULL){
    if(executeNext == 0) {
      executeNext = 1;
      curr = curr->next;
      continue;
    }
    f = fork();
    e = errno;

    if(curr->argv[0] != NULL && strcmp(curr->argv[0], "exit") == 0) {
      exit(0);
    }
 
    if(f == 0){
      execvp(curr->argv[0], curr->argv);
      e = errno;
      printf("Error while executing: %s", strerror(e));
      exit(0);
    } else if(f == -1){
      printf("Fork error: %s", strerror(e));
      return RESERROR;
    } else {
      if(waitpid(f, &status, 0) == -1) {
        errorHandler("wait");
      } else {
        if(status == 0) procres = 1;
        else procres = 0;
        printf("program executed with status %d\n", procres);
        if(procres == 1 && curr->next!=NULL && curr->next->conjuction == CONJOR) {
          executeNext = 0;
        }
        if(procres == 0 && curr->next!=NULL && curr->next->conjuction == CONJAND) {
          executeNext = 0;
        }
      }
    }
    curr = curr->next;
  }
  return RESSUCCESS;
}

/* -------------------------------------------------------------------------------------- */