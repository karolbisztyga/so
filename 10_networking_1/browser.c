
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define PORT 80

int main(void)
{
    int fd;
    char *s;
    ssize_t nread;
    // deklaracja odpowiednich zmiennych
    char *url = "http://www.google.com";
    s = malloc(sizeof(char)*100);


    struct hostent *he;
    struct sockaddr_in their_addr;

          if ((he=gethostbyname(url)) == NULL) {
              perror("gethostbyname");
              exit(1);
          }
          their_addr.sin_family = AF_INET;
          their_addr.sin_port = htons(PORT);
          their_addr.sin_addr = *((struct in_addr *)he->h_addr);
          if (connect(fd, (struct sockaddr *)&their_addr, sizeof their_addr) == -1) {
            perror("connect");
            exit(1);
          }
          snprintf(s, sizeof(s), "GET /%s HTTP/1.0\r\n\r\n", url);
          printf("connecting to [%s]...", s);
          // wyslij zadanie do serwera
          //odczytaj odpowiedz z serwera i przekaz wynik na standardowe wyjscie
          //zamknij polaczenie

 }

