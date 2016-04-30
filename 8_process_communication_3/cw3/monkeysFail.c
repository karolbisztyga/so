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

#define INTERVAL 100000
#define KEY 9
#define RIGHT 1
#define LEFT -1
#define EMPTY 0
#define SIZE 5

int errorHandler(char*);
int monkeyGoesOn(int*,int);
void monkeyGoesOff();
int getInterval();

/*
	z childa malpy wchodza z lewej, z parenta z prawej
*/


int main() {

	srand(time(NULL));

	int shmid, semid;
	int f,i,monkeys,n;
	pthread_t *ths;
	struct sembuf sb;
	sb.sem_num = 0;
	sb.sem_flg = 0;
	monkeys = 10;
	/* inicjuje pamiec wspoldzielona */
	if((shmid=shmget(KEY,sizeof(int)*SIZE,IPC_CREAT|0777))==-1) {
		errorHandler("shmget");
	}
	/* ustawia wartosci poczatkowe pamieci wspoldzielonej */
	int *mut = shmat(shmid,NULL,0);
	for(i=0;i<SIZE;++i) {
		mut[i] = 0;
	}
	/* inicjuje semafor sluzacy do operowania na pamieci wspoldzielonej */
	if((semid = semget(KEY+1, 1, IPC_CREAT|0777))==-1) {
		errorHandler("semget");
	}
	if(semctl(semid,0,SETVAL,1)==-1) {
		errorHandler("semctl SET");
	}

	if((f=fork())==-1) {
		errorHandler("fork");
	} else if(f==0) {
		/*child*/
		ths = malloc(sizeof(pthread_t)*monkeys);
		i=0;
		while(i<monkeys) {
			printf("[c] %d\n", i);
			sb.sem_op = -1;
			if(semop(semid, &sb, 1)==-1) {
				errorHandler("semop wait child");
			}
			/* sekcja krytyczna */
			int *rope = shmat(shmid,NULL,0);
			n=monkeyGoesOn(rope, LEFT);
			if(n==-1) {
				usleep(getInterval());
				monkeyGoesOff(rope);
				printf("[uc %d]\n", n);
				continue;
			} else if(n==-2) {
				usleep(getInterval());
				printf("[uc %d]\n", n);
				continue;
			}
			++i;
			sb.sem_op = 1;
			if(semop(semid, &sb, 1)==-1) {
				errorHandler("semop wait child");
			}
			printf("+monkey got on the rope on the left %d\n", n);
			/* sekcja krytyczna */
		}
		for(i=0;i<monkeys;++i) {
			pthread_join(ths[i], NULL);
		}
	} else {
		/*parent*/
		ths = malloc(sizeof(pthread_t)*monkeys);
		i=0;
		while(i<-1) {
			
		}
		waitpid(f,0,0);

		/* koniec */

		if(shmdt(mut)==-1) {
			errorHandler("shmdt");
		}
		if(semctl(semid, 0, IPC_RMID, NULL)==-1) {
			errorHandler("shm IPC_RMID");
		}
		if(shmctl(shmid, IPC_RMID, NULL)==-1) {
			errorHandler("shm IPC_RMID");
		}
		printf("over\n");
	}

	printf("\n");
	return 0;
}

void printRope(int *r) {
	int i;
	printf("---[");
	for(i=0;i<SIZE;++i) {
		printf("%d ", r[i]);
	}
	printf("]\n");
}

/*
	sprawdza czy jest taka opcja, zeby malpa z danej strony weszla na line
	jesli tak, zwraca pozycje malpy na linie, jesli nie to:
		-1 lina jest full
		-2 na linie sa malpy idace w przeciwna strone
*/
int monkeyGoesOn(int *rope, int side) {
	int i=0;
	while(rope[i]!=EMPTY) {
		if(rope[i] == -side) {
			return -2;
		}
		if(i>SIZE) {
			return -1;
		}
		++i;
	}
	rope[i] = side;
	return i;
}

int getInterval() {
	return (rand()%INTERVAL)+INTERVAL;
}

void monkeyGoesOff(int *rope) {
	int size=0,i;
	while(rope[size] != EMPTY && size<SIZE) {
		++size;
	}
	if(size == 0) {
		return;
	}
	for(i=1;i<SIZE;++i) {
		if(i<size)
			rope[i-1] = rope[i];
		else
			rope[i]=EMPTY;
	}
}

/*
	funkcja dla watku
	zdejmuje pierwsza z brzegu malpe z liny po okreslonym czasie
	args
		int interval,
		int semid,
		int shmid,
*/
void *monkeyGoesOffold(void *args) {
	int *argarr = (int*)args;
	int semid = argarr[0];
	int shmid = argarr[1];
	int size=0,i;
	struct sembuf sb;
	sb.sem_num = 0;
	sb.sem_flg = 0;

	usleep((rand()%INTERVAL)+INTERVAL);
	sb.sem_op = -1;
	if(semop(semid, &sb, 1)==-1) {
		errorHandler("semop wait monkeyGoesOff");
	}
	int *rope = shmat(shmid,NULL,0);
	/* sekcja krytyczna */
	while(rope[size] != EMPTY && size<SIZE) {
		++size;
	}
	if(size == 0) {
		sb.sem_op = 1;
		if(semop(semid, &sb, 1)==-1) {
			errorHandler("semop free monkeyGoesOff");
		}
		return NULL;
	}
	for(i=1;i<SIZE;++i) {
		if(i<size)
			rope[i-1] = rope[i];
		else
			rope[i]=EMPTY;
	}
	/* sekcja krytyczna */
	sb.sem_op = 1;
	if(semop(semid, &sb, 1)==-1) {
		errorHandler("semop free monkeyGoesOff");
	}
	printf("-monkey went off the rope\n");
	printRope(rope);
	return NULL;
}

int errorHandler(char* title) {
    printf("error %s: %s\n", title, strerror(errno));
    return errno;
}