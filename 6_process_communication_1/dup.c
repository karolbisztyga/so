#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>

int errorHandler(char* title) {
    printf("error %s: %s\n", title, strerror(errno));
    return errno;
}

int main(void)
{
    int     pfd[2];
    pid_t   pid;
    char    string[] = "Test";
    char    buf[10];

    pipe(pfd);

    if((pid=fork())==-1) {
        errorHandler("fork");
    } else if(pid == 0) {
        /*
            *predefined file descriptors
                * 0 standard input
                * 1 standard output
                * 2 standard error
        */
        close(pfd[1]);              
        dup2(pfd[0],0);
        read(STDIN_FILENO, buf, sizeof(buf));
        printf("Wypisuje: %s\n", buf);
    } else {
        close(pfd[0]);
        write(pfd[1], string, (strlen(string)+1));        
    }

    return 0;
}

