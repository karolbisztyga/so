#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "copyFile.h"
#include "writeall.h"

#define BUFFER_SIZE 128

int errorHandler(char* title) {
    printf("error %s: %s\n", title, strerror(errno));
    return errno;
}
 
int main (int argc, char **argv) {
    int copySuccessful, writeallTest;
    const char* buff;

    copySuccessful = copy("f","copied_f");
    if(copySuccessful < 0) {
        return errorHandler("copy");
    } else if(copySuccessful == 0) {
        printf("copy unsuccessful");
    } else {
        printf("copy successful");
    }

    writeallTest = creat("writeallTest",
            S_IRUSR | S_IRGRP | S_IROTH | S_IWUSR | S_IWGRP | S_IWOTH);
    if(writeallTest == -1) {
        return errorHandler("writeall");
    }
    buff = "siemaaa";
    writeall(writeallTest, buff, sizeof(buff));
    close(writeallTest);

    printf("\n");
    return(0);
}