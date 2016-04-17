#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

#define BUFFSIZE 512

int errorHandler(char*);
int charPt2int(char*);
char* int2CharPt(int);

int main(void)
{
    int f,f2,nread,i;
    int pfd[2], pfd2[2];
    char buffer[BUFFSIZE], *str;

    if(pipe(pfd)==-1) {
        errorHandler("pipe");
    }

    if((f=fork())==-1) {
        errorHandler("fork 1");
    } else if(f == 0) {
        /*child 1*/
        if(pipe(pfd2)==-1) {
            errorHandler("pipe 2");
        }
        if((f2=fork())==-1) {
            errorHandler("fork 2");
        } else if(f2 == 0) {
            /*child 2*/
            close(pfd[0]);
            close(pfd[1]);
            close(pfd2[0]);
            for(i=0;i<10;++i) {
                str = int2CharPt(i);
                if(write(pfd2[1], str, BUFFSIZE)==-1) {
                    errorHandler("write 2");
                } else {
                    /*printf("-child 2 writing: %s\n", str);*/
                }
            }
        } else {
            /*child 1*/
            close(pfd[0]);
            close(pfd2[1]);
            while((nread=read(pfd2[0], buffer, BUFFSIZE))>0) {
                buffer[nread] = 0;
                /*printf("child 1 read data %d [%s]\n", nread, buffer);*/
                str = int2CharPt(charPt2int(buffer)*5);
                /*printf("---attempting to send [%s]\n", str);*/
                if(write(pfd[1], str, BUFFSIZE)==-1) {
                    errorHandler("child 1 write");
                }
            }
            if(nread==-1) {
                errorHandler("read child 1");
            }
            /*
            str = "patrollin patrolling hehe";
            printf("-child 1 writing %s\n", str);
            if(write(pfd[1], str, BUFFSIZE)==-1) {
                errorHandler("write");
            }
            */
        }
    } else {
        /*parent*/
        close(pfd[1]);
        while((nread=read(pfd[0], buffer, BUFFSIZE))>0) {
            buffer[nread] = 0;
            /*printf("parent read data: %d [%s]\n", nread, buffer);*/
            printf("%s,", buffer);
        }
        if(nread==-1) {
            errorHandler("read");
        }
        printf("\n");
    }

    return 0;
}

char* int2CharPt(int x) {
    int length,i;
    char *result;
    length = 1;
    i = x;
    while(i>9) {
        ++length;
        i /= 10;
    }
    result = malloc(length);
    for(i=0;i<length;++i) {
        result[length-i-1] = (x%10)+48;
        x /= 10;
    }
    return result;
}

int charPt2int(char* s) {
    int res, i;
    res = 0;
    i = 0;
    while(s[i] != 0) {
        res *= 10;
        res += s[i++]-48;
    }
    return res;
}

int errorHandler(char* title) {
    printf("error %s: %s\n", title, strerror(errno));
    return errno;
}