#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

void err(char* s) {
    printf("error: %s\n", s);
    exit(0);
}

int main(int argc, char *argv[]){

    int pfd[2], fpc;
    size_t nread, nwrite, f, s, pip;
    char *buf;
    char * str;

    if((pip = pipe(pfd))==-1)err("pipe");
    s=512;
    f = fork();
    if(f==0) {
        printf("child writing...\n");
        close(pfd[0]);
        str = "Ur beautiful pipe example";
        if((nwrite = write(pfd[1], str, s))==-1)err("write");
    } else if(f>0) {
        printf("parent reading...\n");
        close(pfd[1]);
        buf = malloc(sizeof(char)*s);
        if((nread=read(pfd[0],buf, s))==-1)err("read");
        (nread!=0)?printf("%s (%ld bytes)\n",buf,(long)nread):printf("No data\n");
        free(buf);

        if((fpc = fpathconf(pip,_PC_PIPE_BUF))==-1)err("fpathconf");
        printf("bytes for pipe: [%d]\n",fpc);
    } else {
        printf("fork err\n");return(1);
    }
    
    return 0;
}

