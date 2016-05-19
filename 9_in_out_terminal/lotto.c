#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <fcntl.h>

int errorHandler(char*);

int main() {
	srand(time(NULL));

	int f;
	if((f=fork())==-1) {
		errorHandler("fork");
	} else if(f==0) {
		int i,d,r,j;
		if(setsid()==-1) {
			errorHandler("setsid");
			exit(1);
		}
		umask(0);
		close(STDIN_FILENO);
		close(STDERR_FILENO);
/*
		system("echo '' > wyniki_lotto");
		system("echo $RANDOM >> wyniki_lotto");*/
		for(j=0;j<10;++j) {
			if((d=open("wyniki_lotto",O_TRUNC | O_WRONLY))==-1) {
				errorHandler("open");
			}/* trunc nie dziala?? */
			dup2(d, STDOUT_FILENO);
			printf("---%d",d);
			for(i=0;i<6;++i) {
				r = rand()%49+1;
				printf("%d\n", r);
			}
			close(d);
			usleep(10000000);
		}
	} else {
		exit(0);
	}

	printf("\n");
	return 0;
}

int errorHandler(char* title) {
    printf("error %s: %s\n", title, strerror(errno));
    return errno;
}