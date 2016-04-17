#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>

#define INTERVAL 100000

int errorHandler(char* title) {
    printf("error %s: %s\n", title, strerror(errno));
    return errno;
}

static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
static long double x = 0;
static int block = 1;

static void *threadF(void *arg) {
    int loopCount;
    loopCount = 0;
    /*while(rand()%10 <= 8) {*/
    for(int i=0 ; i<1000 ; ++i) {
        if(block == 1)pthread_mutex_lock(&mtx);
        ++x;
        ++loopCount;
        /*printf("-");
        fflush(stdout);*/
        if(block == 1)pthread_mutex_unlock(&mtx);
        /*usleep(INTERVAL);*/
    }
    return (void*)loopCount;
}
 
int main (int argc, char **argv) {

    pthread_t tid;
    int threadRes, i;
    void *status;

    srand(time(NULL));

    threadRes = pthread_create(&tid,NULL,threadF,NULL);
    if(threadRes > 0) {
        errorHandler("thread");
    } else {
        printf("thread started successfuly\n");
    }
    for(i=0;i<1000;++i) {
        /*printf(".");
        fflush(stdout);
        usleep(INTERVAL);*/
        if(block == 1)pthread_mutex_lock(&mtx);
        ++x;
        if(block == 1)pthread_mutex_unlock(&mtx);
    }
    pthread_join(tid, &status);
    printf("thread ended with status %d and x = %d\n", (int)status, (int)x);

    printf("\n");
    return(0);
}