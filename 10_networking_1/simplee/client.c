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
#include <netinet/in.h>
#include <sys/socket.h>
 
#define PORT 26643
 
#define MAXDATASIZE 100
 
int main(int argc, char *argv[])
{
	int sockfd, numbytes;  
	char buf[MAXDATASIZE];
	struct hostent *he;
	struct sockaddr_in their_addr;
    char *hostname;
 
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
 
	if ((numbytes=recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
	    perror("recv");
	    exit(1);
	}
 
	buf[numbytes] = '\0';
 
	printf("Received: %s",buf);
 
	close(sockfd);
 
	return 0;
}

