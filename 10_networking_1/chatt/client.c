/*
** client.c -- a stream socket client demo
*/
 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <sys/socket.h>
 
#define PORT 26643
 
#define MAXDATASIZE 100
int errorHandler(char*);
 
int main(int argc, char *argv[])
{
	int sockfd;
	struct hostent *he;
	struct sockaddr_in their_addr;
    char *hostname;
    int f,i;
	char buffer[32];
 
    if(argc == 2) {
        hostname = argv[1];
    } else if(argc == 1) {
        hostname = malloc(sizeof(char)*128);
        hostname[127] = 0;
        gethostname(hostname, 127);
    } else if (argc != 2) {
	    fprintf(stderr,"usage: client hostname\n");
	    exit(1);
	}
 
	if ((he=gethostbyname(hostname)) == NULL) {
	    perror("gethostbyname");
	    exit(1);
	}
 
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
	    perror("socket");
	    exit(1);
	}
 
	their_addr.sin_family = AF_INET;
	their_addr.sin_port = htons(PORT);
	their_addr.sin_addr = *((struct in_addr *)he->h_addr);
	memset(their_addr.sin_zero, '\0', sizeof their_addr.sin_zero);
 
	if (connect(sockfd, (struct sockaddr *)&their_addr, sizeof their_addr) == -1) {
	    perror("connect");
	    exit(1);
	}
	if((f=fork())==-1) {
		errorHandler("fork");
	} else if(f==0) {
		int r;
		while((r=recv(sockfd,buffer,sizeof(buffer),0)) > 0) {
			buffer[r]=0;
			printf("\nserver: %s", buffer);
		}
		if(r==-1) {
			errorHandler("recv");
		}
		exit(0);
	} else {
		int fd=0, r;
		if(dup2(fd,STDIN_FILENO)==-1)errorHandler("dup2");
		while((r=read(fd, buffer, sizeof(buffer))) > 0) {
			if(r==1) {
				/*printf("no data, exiting...\n");*/
				kill(f, SIGKILL);
				break;
			}
			/*printf("sending: %s\n", buffer);*/
			if(send(sockfd,buffer,sizeof(buffer),0)==-1) {
				errorHandler("send");
			}
			/*printf("data sent\n");*/
			for(i=0;i<sizeof(buffer);++i) {
				buffer[i] = 0;
			}
		}
		if(r==-1) {
			errorHandler("read");
		}
		waitpid(f,NULL,0);
	}
 
	close(sockfd);
 
	return 0;
}

int errorHandler(char* title) {
    printf("error %s: %s\n", title, strerror(errno));
    return errno;
}