/*
** server.c -- a stream socket server demo
*/
 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
 
#define MYPORT 26643
 
#define BACKLOG 10
 
void sigchld_handler(int s)
{
	while(waitpid(-1, NULL, WNOHANG) > 0);
}
int errorHandler(char*);
 
int main(void)
{
	int sockfd, new_fd;
	struct sockaddr_in my_addr;
	struct sockaddr_in their_addr;
	socklen_t sin_size;
	struct sigaction sa;
	int yes=1;
 
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		exit(1);
	}
 
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
		perror("setsockopt");
		exit(1);
	}
 
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(MYPORT);
	my_addr.sin_addr.s_addr = INADDR_ANY;
	memset(my_addr.sin_zero, '\0', sizeof my_addr.sin_zero);
 
	if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof my_addr) == -1) {
		perror("bind");
		exit(1);
	}
 
	if (listen(sockfd, BACKLOG) == -1) {
		perror("listen");
		exit(1);
	}
 
	sa.sa_handler = sigchld_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1) {
		perror("sigaction");
		exit(1);
	}
 	while(1==1) {
 		int f;
		sin_size = sizeof their_addr;
		if ((new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size)) == -1) {
			perror("accept");
		}
		if((f=fork())==-1) {
			errorHandler("fork");
		} else if(f==0) {
			printf("server: got connection from %s\n",inet_ntoa(their_addr.sin_addr));
			printf("[%d] sending response...\n", getpid());
			if (send(new_fd, "Hello, world!\n", 14, 0) == -1)
				perror("send");
			sleep(2);
			printf("[%d] response sended...\n", getpid());
			return 0;
		}
	}	
	sleep(5);
	close(new_fd);
 
	return 0;
}

int errorHandler(char* title) {
    printf("error %s: %s\n", title, strerror(errno));
    return errno;
}