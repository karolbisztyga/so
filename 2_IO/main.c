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
    int read, write, ncat, ntail;
    char buffer[BUFFER_SIZE];

    printf("reading data...\n");

    read = readAll("testFile", buffer);
    if(read == -1) {
        errorHandler("readAll");
        return 1;
    }

    printf("%d data read in buffer: \n-----\n%s\n-----\n", read, buffer);

    printf("writting data...\n");

    write = writeAll("testFileCopy", buffer, read);
    if(write == -1) {
        errorHandler("writeAll");
        return 1;
    }
    printf("%d data written\ncat:\n", write);

    ncat = cat("testFileCopy", 10);
    if(ncat == -1) {
        errorHandler("cat");
        return 1;
    }
    printf("\ncat: %d\n", ncat);

    ntail = tail("testFileCopy", 5);
    if(ntail == -1) {
        errorHandler("tail");
        return 1;
    }
    printf("\ntail: %d\n", ntail);

    printf("\n");
    return(0);
}