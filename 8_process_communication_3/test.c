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

int main() {

	srand(time(NULL));

	int f,shmid,i,size,semid;
	size=100;
	/* inicjuje pamiec wspoldzielona */
	if((shmid=shmget(KEY,sizeof(int)*size,IPC_CREAT|0777))==-1) {
		errorHandler("shmget");
	}
	/* ustawia wartosci poczatkowe pamieci wspoldzielonej */
	int *mut = shmat(shmid,NULL,0);
	for(i=0;i<size;++i) {
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
		int j;
		struct sembuf sb;
		sb.sem_num = 0;
		sb.sem_flg = 0;
		for(i=0;i<1;++i) {
			for(j=0;j<size;++j) {
				sb.sem_op = -1;
				if(semop(semid, &sb, 1)==-1) {
					errorHandler("semop wait child");
				}
				/* sekcja krytyczna */
				usleep(100);
				int *mut = shmat(shmid,NULL,0);
				mut[j]+=2;
				/* sekcja krytyczna */
				sb.sem_op = 1;
				if(semop(semid, &sb, 1)==-1) {
					errorHandler("semop free child");
				}
			}
		}
	} else {
		/*parent*/
		struct sembuf sb;
		sb.sem_num = 0;
		sb.sem_flg = 0;
		int j;
		for(i=0;i<1;++i) {
			for(j=0;j<size;++j) {
				sb.sem_op = -1;
				if(semop(semid, &sb, 1)==-1) {
					errorHandler("semop wait parent");
				}
				/* sekcja krytyczna */
				usleep(100);
				int *mut = shmat(shmid,NULL,0);
				mut[j]+=2;
				/* sekcja krytyczna */
				sb.sem_op = 1;
				if(semop(semid, &sb, 1)==-1) {
					errorHandler("semop free parent");
				}
			}
		}
		waitpid(f,0,0);

		for(i=0;i<size;++i) {
			if(mut[i]!=4)printf("[%d] = %d", i, mut[i]);
		}

		if(shmdt(mut)==-1) {
			errorHandler("shmdt");
		}
		if(shmctl(shmid, IPC_RMID, NULL)==-1) {
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

int errorHandler(char* title) {
    printf("error %s: %s\n", title, strerror(errno));
    return errno;
}