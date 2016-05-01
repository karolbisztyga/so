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

#define INTERVAL 1000000
#define KEY 9
#define RIGHT 1
#define LEFT 0
#define SIZE 5
#define TEST 0

int errorHandler(char*);
int monkeyGoesOn(int,int,int);
void *monkeyGoesOff(void*);
int getInterval();

/*
	z childa malpy wchodza z lewej, z parenta z prawej
*/


int main() {

	srand(time(NULL));

	int shmid, semid;
	int f,i,monkeys,res,e;
	pthread_t *ths;
	monkeys = 10;
	/* inicjuje pamiec wspoldzielona */
	/* pamiec wspoldzielona:
		[0] - ilosc malp idacych z lewej
		[1] - ilosc malp idacych z prawej
 	*/
	if((shmid=shmget(KEY,sizeof(int)*2,IPC_CREAT|0777))==-1) {
		errorHandler("shmget");
	}
	/* ustawia wartosci poczatkowe pamieci wspoldzielonej */
	int *mut = shmat(shmid,NULL,0);
	for(i=0;i<2;++i) {
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
		int tharr[3];
		tharr[0] = semid;
		tharr[1] = shmid;
		ths = malloc(sizeof(pthread_t)*monkeys);
		i=0;
		while(i<monkeys) {
			res = monkeyGoesOn(LEFT,semid,shmid);
			if(res == 0) {
				printf("[CHILD] monkey got on the rope %d\n", i);
				/* tworzy watek, ktory po jakims czasie zdejmuje malpe z liny */
				tharr[2] = LEFT;
				if((e=pthread_create(&ths[i], NULL, monkeyGoesOff, tharr))>0) {
					printf("error pthread_join: %s", strerror(e));
				}
				++i;
			} else if(res == 1) {
				usleep(getInterval());
				if(TEST==1)printf("[CHILD] monkey tried to get on the rope but it is full\n");
			} else if(res == 2) {
				usleep(getInterval());
				if(TEST==1)printf("[CHILD] monkey tried to get on the rope but there are monkeys goin from the other side\n");
			}
		}
		for(i=0;i<monkeys;++i) {
			if((e=pthread_join(ths[i], NULL))!=0) {
				printf("error pthread_join: %s", strerror(e));
			}
		}
		free(ths);
	} else {
		/*parent*/
		int tharr[3];
		tharr[0] = semid;
		tharr[1] = shmid;
		ths = malloc(sizeof(pthread_t)*monkeys);
		i=0;
		while(i<monkeys) {
			res = monkeyGoesOn(RIGHT,semid,shmid);
			if(res == 0) {
				printf("[PARENT] monkey got on the rope %d\n", i);
				/* tworzy watek, ktory po jakims czasie zdejmuje malpe z liny */
				tharr[2] = RIGHT;
				if((e=pthread_create(&ths[i], NULL, monkeyGoesOff, tharr))>0) {
					printf("error pthread_join: %s", strerror(e));
				}
				++i;
			} else if(res == 1) {
				usleep(getInterval());
				if(TEST==1)printf("[PARENT] monkey tried to get on the rope but it is full\n");
			} else if(res == 2) {
				usleep(getInterval());
				if(TEST==1)printf("[PARENT] monkey tried to get on the rope but there are monkeys goin from the other side\n");
			}
		}
		for(i=0;i<monkeys;++i) {
			if((e=pthread_join(ths[i], NULL))!=0) {
				printf("error pthread_join: %s", strerror(e));
			}
		}
		free(ths);
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

/*
	sprawdza czy jest taka opcja, zeby malpa z danej strony weszla na line
	jesli tak to dodaje ja na line
	return:
		0 - malpa dodana na line
		1 - lina pelna
		2 - na linie sa malpy idace w przeciwna strone
*/
int monkeyGoesOn(int side, int semid, int shmid) {
	int result = 0;
	struct sembuf sb;
	sb.sem_num = 0;
	sb.sem_flg = 0;

	sb.sem_op = -1;
	if(semop(semid, &sb, 1)==-1) {
		errorHandler("semop wait child");
	}
	/* sekcja krytyczna */
	int *mut = shmat(shmid,NULL,0);
	if(side == LEFT) {
		if(mut[RIGHT] > 0) result = 2;
		else if(mut[LEFT] >= SIZE) result = 1;
		else ++mut[LEFT];
	} else {
		if(mut[LEFT] > 0) result = 2;
		else if(mut[RIGHT] >= SIZE) result = 1;
		else ++mut[RIGHT];
	}
	/* sekcja krytyczna */
	sb.sem_op = 1;
	if(semop(semid, &sb, 1)==-1) {
		errorHandler("semop wait child");
	}
	return result;
}

/*
	args:
		semid
		shmid
		side
*/
void *monkeyGoesOff(void *args) {
	int *argarr = (int*)args;
	int semid = argarr[0];
	int shmid = argarr[1];
	int side = argarr[2];
	struct sembuf sb;
	sb.sem_num = 0;
	sb.sem_flg = 0;
	
	usleep(getInterval());

	sb.sem_op = -1;
	if(semop(semid, &sb, 1)==-1) {
		errorHandler("semop wait child");
	}
	/* sekcja krytyczna */
	int *mut = shmat(shmid,NULL,0);
	--mut[side];
	/* sekcja krytyczna */
	sb.sem_op = 1;
	if(semop(semid, &sb, 1)==-1) {
		errorHandler("semop wait child");
	}
	printf("monkey got off the rope \n");
	return NULL;
}

int getInterval() {
	return (rand()%INTERVAL)+INTERVAL;
}

int errorHandler(char* title) {
    printf("error %s: %s\n", title, strerror(errno));
    return errno;
}