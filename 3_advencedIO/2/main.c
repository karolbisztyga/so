#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "fileOperations.h"

#define BUFFER_SIZE 512

int errorHandler(char* title) {
    printf("error %s: %s\n", title, strerror(errno));
    return errno;
}
 
int main (int argc, char **argv) {

    int dataRead;
    char buffer[BUFFER_SIZE];

    if((dataRead = printContent(argv[1], buffer)) == -1) {
        printf("dataRead %d\n", dataRead);
        errorHandler("print content");
        return 1;
    }
    /*printf("success %d [%s]", dataRead, buffer);*/

    printf("\n");
    return(0);
}