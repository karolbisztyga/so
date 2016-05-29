#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFFLEN 32
#define PORT 1111

int charToInt(char*);

int main(int argc, char *argv[]) {

	int f,sockfd,nbytes,fd;
	struct sockaddr_in my_addr;
	struct sockaddr_in their_addr;
	char *buf = malloc(sizeof(char)*BUFFLEN);

	fd = 0;

	if (argc != 5) {
		fprintf(stderr,"usage: [my address] [my port] [target address] [target port]\n");
		exit(1);
	}

	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
		perror("socket");
		exit(1);
	}

	if((f=fork())==-1) {
		perror("fork");
	} else if(f==0) {
		socklen_t addr_len;

		my_addr.sin_family = AF_INET;		 
		my_addr.sin_port = htons(charToInt(argv[4]));
		my_addr.sin_addr.s_addr = htonl(charToInt(argv[3]));
		memset(my_addr.sin_zero, '\0', sizeof my_addr.sin_zero);
	 
		if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof my_addr) == -1) {
			perror("bind");
			exit(1);
		}

		addr_len = sizeof their_addr;
		for(;;) {
			if ((nbytes = recvfrom(sockfd, buf, BUFFLEN-1 , 0,
				(struct sockaddr *)&their_addr, &addr_len)) == -1) {
				perror("recvfrom");
				exit(1);
			}
			buf[nbytes] = '\0';
			printf("*[%s]\n",buf);
		}
		
	} else {

		int n;
		struct hostent *he;

		if(dup2(fd, STDIN_FILENO)==-1) {
			perror("dup2");
		}

		if ((he=gethostbyname(argv[1])) == NULL) {
			perror("gethostbyname");
			exit(1);
		}
	 
		their_addr.sin_family = AF_INET;
		their_addr.sin_port = htons(charToInt(argv[2]));
		their_addr.sin_addr = *((struct in_addr *)he->h_addr);
		memset(their_addr.sin_zero, '\0', sizeof their_addr.sin_zero);

		for(;;){
			if((n=read(fd,buf,sizeof(buf)))==-1) {
				perror("read");
			}
			buf[n-1]=0;
			if ((nbytes = sendto(sockfd, buf, sizeof(buf), 0,
					 (struct sockaddr *)&their_addr, sizeof(their_addr))) == -1) {
				perror("sendto");
				exit(1);
			}
			if(strcmp(buf,"bye")==0) {
				break;
			}
		}
		kill(f,SIGKILL);
	}
	free(buf);
	close(sockfd);
	return 0;
}

int charToInt(char *c) {
	int res=0,i=0,size=0;
	while(c[size++]!=0){}
	while(c[i]!=0) {
		res *= 10;
		res += c[i]-48;
		++i;
	}
	return res;
}