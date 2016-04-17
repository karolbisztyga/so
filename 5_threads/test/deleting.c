#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>

#define INTERVAL 10000

int errorHandler(char* title) {
    printf("error %s: %s\n", title, strerror(errno));
    return errno;
}

static void *threadA(void *arg) {

    while(1) {
        printf(".");
        fflush(stdout);
        usleep(INTERVAL);
    }
    return NULL;
}

int main (int argc, char **argv) {

    int i;
    pthread_t tidA;

    if(pthread_create(&tidA,NULL,threadA,NULL) > 0) {
        errorHandler("thread");
    }

    for(i=0;i<10;++i) {
        printf("-");
        fflush(stdout);
        usleep(INTERVAL);
    }
    pthread_cancel(tidA);
    pthread_join(tidA, NULL);

    printf("OVER\n");
    return(0);
}