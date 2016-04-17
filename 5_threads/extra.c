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

//static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

static void *threadF(void *arg) {
    int sum = 0;
    int *tab = (int*)arg;
    for(int i=0 ; i<10 ; ++i) {
        sum += tab[i];
    }

    return sum;
}
 
int main (int argc, char **argv) {
    srand(time(NULL));

    int tab[2][10];
    pthread_t t1,t2;
    void *s1, *s2;

    for(int i=0 ; i<2 ; ++i) {
        for(int j=0 ; j<10 ; ++j) {
            tab[i][j] = rand()%100;
            printf("%d,", tab[i][j]);
        }
        printf("\n");
    }

    pthread_create(&t1, NULL, threadF, &tab[0][0]);
    pthread_create(&t2, NULL, threadF, &tab[1][0]);

    pthread_join(t1, &s1);
    pthread_join(t2, &s2);

    //printf("statuses %d, %d", (int)s1, (int)s2);
    printf("sum = %d", (int)s1 + (int)s2);

    printf("\n");
    return(0);
}