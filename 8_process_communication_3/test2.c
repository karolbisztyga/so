#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h>
#include <pthread.h>

#define KEY 9
#define INTERVAL 100000

int errorHandler(char*);
void *th(void*);

int main() {

	int *pta = malloc(sizeof(int));
	pthread_t *ths= malloc(sizeof(pthread_t));

	pta[0] = 123;
	printf("%d\n",pta[0]);

	if(pthread_create(&ths[0], NULL, th, pta)>0) {
		errorHandler("pthread_create child");
	}
	pthread_join(ths[0], NULL);

	printf("%d",pta[0]);

	printf("\n");
	return 0;
}

void *th(void *args) {
	int *pt = (int*)args;
	*pt = 345;
	return NULL;
}

int errorHandler(char* title) {
    printf("error %s: %s\n", title, strerror(errno));
    return errno;
}