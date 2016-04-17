#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>

#define BUFFER_SIZE 512

int errorHandler(char*);

int main(void) {
    
    char fileName[] = "dictionary.txt";
    int fo1, fo2, file, nread, filesize, pos, bytesToRead;
    char buffer[BUFFER_SIZE];
    int pfd[2];
    size_t pip;

    pos = 0;

    if((file=open(fileName, O_RDONLY)) == -1) {
        errorHandler("open file");
    }
    filesize = lseek(file, 0, SEEK_END);
    lseek(file, 0, SEEK_SET);

    if((pip=pipe(pfd))==-1) {
        errorHandler("pipe");
    }

    if((fo1=fork())==-1) {
        errorHandler("fork 1");
    } else if(fo1==0) {
        printf("child 1\n");
        close(pfd[0]);
        close(pfd[1]);
        if(execl("/usr/bin/wc","wc","-l","dictionary.txt",NULL) == -1) {
            errorHandler("wc");
        }
        //wc -l dictionary.txt | cut -d " " -f1
    } else {
        if((fo2=fork())==-1) {
            errorHandler("fork 2");
        } else if(fo2==0) {
            printf("child 2\n");
            //cat dictionary.txt | grep pipe | wc -l
        } else {
            printf("parent\n");
            /*bytesToRead = BUFFER_SIZE;
            while((nread = read(file, buffer, bytesToRead)) != 0) {
                pos += nread;
                if(filesize - pos < bytesToRead) {
                    bytesToRead = filesize - pos;
                }
                //write(1, buffer, nread);
            }*/
            ;
        }
    }

    close(file);

    return 0;
}

int errorHandler(char* title) {
    printf("error %s: %s\n", title, strerror(errno));
    return errno;
}
