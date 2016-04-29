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

#define KEY 4
#define INTERVAL 100000

union semun {
	int              val;    /* Value for SETVAL */
	struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
	unsigned short  *array;  /* Array for GETALL, SETALL */
	struct seminfo  *__buf;  /* Buffer for IPC_INFO
								(Linux-specific) */
};

int errorHandler(char*);
void *getMonkeyOut(void*);

int main() {

	srand(time(NULL));

	int f,i,semid;
	/*
		ilsoc malp idacych z
			0 - prawej
			1 - lewej
	*/
	int monkeys[2];
	monkeys[0] = -1;
	monkeys[1] = 10;

	pthread_t *ths;
	int semstat;
	int tharr[3];
	void *v;
	int rope[5];
	for(i=0 ; i<sizeof(rope)/sizeof(int) ; ++i) {
		rope[i] = 0;
	}
/*
	if((shmid = shmget(KEY, sizeof(rope), IPC_CREAT|IPC_EXCL|0777)) == -1) {
		errorHandler("shmget");
	}
	printf("id pamieci wspoldzielonej: %d\n", shmid);
	*/
	/*
		sa 2 semafory:
			0 - malpy z prawej(parent)
			1 - malpy z lewej(child)
	*/
	if((semid = semget(KEY+1, 2, IPC_CREAT|IPC_EXCL|0777))==-1) {
		errorHandler("semget");
	}
	printf("id zbioru semaforow: %d\n", semid);

	if((f=fork())==-1) {
		errorHandler("fork");
	} else if(f==0) {
		/*child*/
		tharr[0] = semid;
		tharr[1] = 1;

		ths = malloc(sizeof(pthread_t)*monkeys[1]);

		for(i=0;i<monkeys[1];++i) {
			/* sprawdza czy zadna malpa nie idzie z prawej od parenta */
			while((semstat=semctl(semid,0,GETVAL))>0) {
				usleep(1);
			}
			if(semstat == -1) {
				errorHandler("semstat");
			}
			/* sprawdza czy na linie nie ma wiecej niz 5 malp */
			while((semstat=semctl(semid,1,GETVAL))>=5) {
				usleep(1);
			}
			if(semstat == -1) {
				errorHandler("semstat");
			}
			/* dodaje jedna malpe idaca z lewej */
			if(semctl(semid,1,SETVAL,++semstat)==-1) {
				errorHandler("semctl SET child");
			}
			/* 	uruchamia dodatkowy watek, ktory po uplywie okreslonego czasu
				zdejmuje jedna malpe idaca z lewej */
			tharr[2] = (rand()%INTERVAL)+INTERVAL*3;
			if(pthread_create(&ths[i], NULL, getMonkeyOut, tharr)>0) {
				errorHandler("pthread_create");
			}
			/*  */
			printf("ilosc malp idaca z lewej strony: %d\n", semstat);
	        fflush(stdout);
	        usleep((rand()%INTERVAL)+INTERVAL);
		}
		for(i=0;i<monkeys[1];++i) {
			pthread_join(ths[i], &v);
		}
		/*		TO SAMO DLA PARENTA TERAZ!!!	*/
	} else {
		/*parent*/
		tharr[0] = semid;
		tharr[1] = 1;

		ths = malloc(sizeof(pthread_t)*monkeys[1]);

		for(i=0;i<monkeys[0];++i) {
			/* sprawdza czy zadna malpa nie idzie z lewej od childa */
			while((semstat=semctl(semid,1,GETVAL))>0) {
				usleep(1);
			}
			if(semstat == -1) {
				errorHandler("semstat");
			}
			/* sprawdza czy na linie nie ma wiecej niz 5 malp */
			while((semstat=semctl(semid,0,GETVAL))>=5) {
				usleep(1);
			}
			if(semstat == -1) {
				errorHandler("semstat");
			}
			/* dodaje jedna malpe idaca z prawej */
			if(semctl(semid,0,SETVAL,++semstat)==-1) {
				errorHandler("semctl SET parent");
			}
			/* 	uruchamia dodatkowy watek, ktory po uplywie okreslonego czasu
				zdejmuje jedna malpe idaca z lewej */
			tharr[2] = (rand()%INTERVAL)+INTERVAL*3;
			if(pthread_create(&ths[i], NULL, getMonkeyOut, tharr)>0) {
				errorHandler("pthread_create");
			}
			/* */
			printf("ilosc malp idaca z prawej strony: %d", semstat);
	        fflush(stdout);
	        usleep((rand()%INTERVAL)+INTERVAL);
		}
		waitpid(f,0,0);
		/*
		if(shmctl(shmid, IPC_RMID, NULL)==-1) {
			errorHandler("shm IPC_RMID");
		}
		*/
		if(semctl(semid, IPC_RMID, 0)==-1) {
			errorHandler("sem IPC_RMID");
		}
		printf("over\n");
	}

	printf("\n");
	return 0;
}

/*
	args(int*):
		0 - id zbioru semaforow
		1 - z lewje czy z prawej(1/0)
		2 - ile ma czekac funkcja usleep
*/
void *getMonkeyOut(void *args) {
	int stat;
	int *argarr = (int*)args;
	usleep(argarr[2]);
	if((stat=semctl(argarr[0],argarr[1],GETVAL))==-1) {
		errorHandler("semctl getMonkeyOut GET");
	}
	if(semctl(argarr[0],argarr[1],SETVAL,stat-1)==-1) {
		errorHandler("semctl getMonkeyOut SET");
	}
	return NULL;
}

int errorHandler(char* title) {
    printf("error %s: %s\n", title, strerror(errno));
    return errno;
}