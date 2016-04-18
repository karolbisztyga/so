#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/stat.h>

#include "fifo.h"
/* ------------------------------------------------------------- */

int main(void)
{
  struct message msg;
  char* fifosrvbasename = "srvfifoqueue";
  char fifosrvname[FIFO_NAME_BUF_SIZE];
  char fifocntname[FIFO_NAME_BUF_SIZE];
  int fdsrv,
    fdcnt,
    bread,
    bwrite, test;
    test=0;
  
  printf("Client [%ld] started...\n", (long)getpid());
  setbuf(stdout, NULL);
  msg.pid = getpid();

  /* Creating client fifo name */
  make_cnt_fifo_queue_name(fifocntname, msg.pid, FIFO_NAME_BUF_SIZE);
  if(test==1)printf("Creating client fifo queue \'%s\'...", fifocntname);
  if((mkfifo(fifocntname, PERM_FILE) == -1) && (errno != EEXIST)) {
    errorHandler("mkfifo");
  }

  /* Opening server fifo for writing */
  make_srv_fifo_queue_name(fifosrvname, fifosrvbasename, FIFO_NAME_BUF_SIZE);
  if(test==1)printf("OK\nOpening server fifo queue \'%s\' for writing...", fifosrvname);
  if((fdsrv = open(fifosrvname, O_WRONLY)) == -1) {
    errorHandler("open");
  }
  if(test==1)printf("OK\n");

  while(1)
    {
      /* Getting message */
      printf("Send message: ");
      if(stdin_readall(msg.data, MESSAGE_BUF_SIZE) == 0) {
        break;
      }
      if(strcmp(msg.data,"bye")==0) {
        printf("bye\n");
        close(fdcnt);
        close(fdsrv);
        unlink(fifocntname);
        return 0;
      }
      /* Sending message to server */
      if(test==1)printf("Writing message to server...");
      if((bwrite = write(fdsrv, &msg, sizeof(msg))) == -1) {
        errorHandler("write");
      }

      /* Opening client fifo for reading */
      if(test==1)printf("OK\nOpening client fifo queue \'%s\' for reading...", fifocntname);
      if((fdcnt = open(fifocntname, O_RDONLY | O_NONBLOCK)) == -1) {
        errorHandler("open");
      }
      if(test==1)printf("OK\n");

      /* Reading responce */
      if(test==1)printf("Waiting for data");
      while((bread = read(fdcnt, &msg, sizeof(msg)))==0) {
        printf(".");
        fflush(stdout);
        usleep(1000000);
      }
      if(bread == -1) {
    	  close(fdcnt);
        errorHandler("read");
    	}
      close(fdcnt);
      if(test==1)printf("OK\n");
      printf("Message from server: %s\n", msg.data);
    }

  /* Cleaning up */
  close(fdsrv);
  unlink(fifocntname);

  return 0;
}
