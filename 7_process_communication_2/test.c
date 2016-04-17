#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>

int errorHandler(char*);

int main(void) {

    int fo, fd, bytes;
    char *fifoName = "fifo_test";

    if((fo=fork())==-1) {
        errorHandler("fork");
    } else if(fo==0) {
        if(mkfifo(fifoName,S_IRUSR|S_IWUSR)==-1 && errno!=EEXIST) {
            errorHandler("fifo create");
        }
        if((fd=open(fifoName, O_WRONLY))==-1) {
            errorHandler("open fifo");
        }
        if(dup2(fd, STDOUT_FILENO)==-1) {
            errorHandler("dup2 child");
        }
        execl("/bin/ls","ls","-la",NULL);
        errorHandler("ls");
    } else {
        if(mkfifo(fifoName,S_IRUSR|S_IWUSR)==-1 && errno!=EEXIST) {
            errorHandler("fifo create");
        }
        if((fd=open(fifoName, O_RDONLY))==-1) {
            errorHandler("open fifo");
        }
        if(dup2(fd, STDIN_FILENO)==-1) {
            errorHandler("dup2 parent");
        }
        execl("/usr/bin/wc","wc","-l",NULL);
        errorHandler("wc");
    }

    return 0;
}

int errorHandler(char* title) {
    printf("error %s: %s\n", title, strerror(errno));
    return errno;
}