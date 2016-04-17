#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include "copyFile.h"

#define BUFFER_SIZE 128
/*
 * @return
 *		1  - success
 *		0  - failure
 *		-1 - error(errno set)
 */
int copy(char* from, char* to) {
	int toRead, dataRead, toWrite, dataWrote, readClose, writeClose;
	char buffer[BUFFER_SIZE];

	toRead = open(from,O_RDONLY);
	toWrite = open(	to, 
					O_WRONLY | O_CREAT | O_TRUNC, 
					S_IRUSR | S_IRGRP | S_IROTH | S_IWUSR | S_IWGRP | S_IWOTH );
	if(toRead == -1 || toWrite == -1) {
		return -1;
	}
	printf("toRead: %d\ntoWrite: %d\n", toRead, toWrite);
	while((dataRead = read(toRead, buffer, BUFFER_SIZE)) > 0) {
		dataWrote = write(toWrite, buffer, dataRead);
		if(dataWrote == -1 || dataRead == -1) {
			return -1;
		}
	}
	readClose = close(toRead);
	writeClose = close(toWrite);
	if(readClose == -1 || writeClose == -1) {
		return -1;
	}
	return 1;
}