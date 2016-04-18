#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
/* ------------------------------------------------------------- */

#include "fifo.h"
/* ------------------------------------------------------------- */

int x=0;

void mark() {
  printf("here %d\n", x++);
}

void clear(char *s) {
  i=0;
  while(s[i]!=0) {
    s[i++]=0;
  }
}

void writeResponse(char *s) {
  int loc;
    for(i=0 ; i<sizeof(trans)/sizeof(trans[0]) ; i+=2) {
      if(strcmp(trans[i],s)==0) {
        loc=lookup(s);
        if(loc == -1) continue;
        clear(s);
        strcpy(s,trans[loc+1]);
        return;
      }
    }
    clear(s);
    strcpy(s,"hello...");
}

int main(void)
{
  struct message msg;
  char* fifosrvbasename = "srvfifoqueue";
  char fifosrvname[FIFO_NAME_BUF_SIZE];
  char fifocntname[FIFO_NAME_BUF_SIZE];
  int fdsrv,
    fdcnt,
    bread,
    bwrite;
  
  printf("Server started...\n");
  setbuf(stdout, NULL);

  /* Creating server fifo queue */
  make_srv_fifo_queue_name(fifosrvname, fifosrvbasename, FIFO_NAME_BUF_SIZE);
  printf("Creating server fifo queue \'%s\'...", fifosrvname);
  if((mkfifo(fifosrvname, PERM_FILE) == -1) && (errno != EEXIST)) {
    errorHandler("mkfifo");
  }

  /* Opening fifo */
  printf("OK\nOpening server fifo queue \'%s\' for reading...", fifosrvname);
  if((fdsrv = open(fifosrvname, O_RDONLY | O_NONBLOCK)) == -1) {
    errorHandler("open");
  }
  printf("OK\n");

  while(1)
    {
      /* Reading from queue */
      printf("Waiting for data");
      while((bread = read(fdsrv, &msg, sizeof(msg)))<=0) {
        if(bread == -1 && errno != EAGAIN) {
          errorHandler("read");
        }
        printf(".");
        fflush(stdout);
        usleep(1000000);
      }
      
      printf("OK\n");

      printf("Message from client [%d]: %s\n", msg.pid, msg.data);
      /*stdin_readall(msg.data, MESSAGE_BUF_SIZE);
*/
      /* Creating client fifo name  */
      make_cnt_fifo_queue_name(fifocntname, msg.pid, FIFO_NAME_BUF_SIZE); 

      /* Opening client fifo for writing  */
      printf("Opening client fifo \'%s\' for writing...", fifocntname);
      if((fdcnt = open(fifocntname, O_WRONLY)) == -1) {
        errorHandler("open");
      }

      /* Sending responce */
      writeResponse(msg.data);
      printf("OK\nWritting responce to client [%ld][%s]...", (long)msg.pid, msg.data);
      if((bwrite = write(fdcnt, &msg, sizeof(msg))) == -1) {
        errorHandler("write");
      }
      printf("OK\n");
      close(fdcnt);
    }
  
  
  /* Cleaning up */
  close(fdsrv);
  unlink(fifosrvname);
  
  return 0;
}
