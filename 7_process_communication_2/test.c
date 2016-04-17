#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

int errorHandler(char*);

int main(void) {

    int f;

    if((f=fork())==-1) {
        errorHandler("fork");
    } else if(f==0) {
        execl("/bin/ls","ls","-l",NULL);
        errorHandler("exec child");
    } else {
        execl("/usr/bin/who","who",NULL);
        errorHandler("exec parent");
    }

    return 0;
}

int errorHandler(char* title) {
    printf("error %s: %s\n", title, strerror(errno));
    return errno;
}