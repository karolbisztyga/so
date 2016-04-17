#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

int errorHandler(char* title) {
    printf("error %s: %s\n", title, strerror(errno));
    return errno;
}

int main(void)
{
    int f,f2;
    int pfd[2];

    if(pipe(pfd)==-1) {
        errorHandler("pipe");
    }

    if((f=fork())==-1) {
        errorHandler("fork");
    } else if(f == 0) {
        dup2(pfd[1],STDOUT_FILENO);
        close(pfd[0]);
        close(pfd[1]);
        execl("/usr/bin/who","who",NULL);
    } else {
        if((f2=fork())==-1) {
            errorHandler("fork 2");
        } else if(f2 == 0) {
            dup2(pfd[0],STDIN_FILENO);
            close(pfd[0]);
            close(pfd[1]);
            execl("/usr/bin/wc","wc",NULL);
        } else {
            close(pfd[1]);
            close(pfd[1]);
            waitpid(f,NULL,0);
            waitpid(f2,NULL,0);
        }
    }

    return 0;
}

