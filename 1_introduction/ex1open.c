#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h> /*errno*/
/*#include <cerrno> //errno*/
/*#include <cstring> /strerror*/
#include <stdlib.h> /*exit*/
#include "t.h"
 
#define BUFSIZE 1024
 
int main (int argc, char **argv) {
    int f1, c, r;
    char b[BUFSIZE], *n1;
 
    c = 20;
    n1 = argv[1];
 
    f1 = open (n1, O_RDONLY);
    if(f1 < 0) {
        printf("error open: %s\n",strerror(errno));
        exit(f1);
    }
    r = read (f1, b, c);
    if(r==-1) {
        printf("error read: %s\n",strerror(errno));
        exit(r);
    }
    printf("%s: Przeczytano %d znakow z pliku %s: \"%s\"\n",
	   argv[0], c, n1, b);
    close(f1);
 
    return(0);
}