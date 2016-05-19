#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

int errorHandler(char*);
 
int main(void) {
 
  pid_t pid;
  pid = fork();
  if (pid < 0) exit(EXIT_FAILURE);
  if (pid > 0) {
    printf("Rodzic:  pid=%d ppid=%d sid=%d\n",getpid(),getppid(),getsid(0));
  }
  if (pid == 0) {
    if(setsid()==-1) {
      errorHandler("setsid");
    }
    printf("Dziecko: pid=%d ppid=%d sid=%d\n",getpid(),getppid(),getsid(0));
    while(1==1)usleep(1000000000);
  }
  
  /*
    ps -g xxx
    while(1==1)usleep(1000000000);
  */
  exit(EXIT_SUCCESS);
}

int errorHandler(char* title) {
    printf("error %s: %s\n", title, strerror(errno));
    return errno;
}