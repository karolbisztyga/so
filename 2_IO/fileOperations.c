#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "fileOperations.h"

#define BUFFER_SIZE 8

int writeAll(char *fileName, char *buffer, int size) {
	int file, currentBytesWritten, dataWritten, nextBufferSize;
	currentBytesWritten = 0;
	dataWritten = 0;
	nextBufferSize = BUFFER_SIZE;

	file = open(	fileName,
					O_WRONLY | O_CREAT | O_TRUNC, 
					S_IRUSR | S_IRGRP | S_IROTH | S_IWUSR | S_IWGRP | S_IWOTH );
	if(file == -1) {
		return -1;
	}
	nextBufferSize = (dataWritten+nextBufferSize < size) ? nextBufferSize : size-dataWritten ;
	while(currentBytesWritten != -1 && dataWritten < size) {
		nextBufferSize = (dataWritten+nextBufferSize < size) ? nextBufferSize : size-dataWritten ;
		currentBytesWritten = write(file, &buffer[dataWritten], nextBufferSize);
		dataWritten += currentBytesWritten;
		/*printf("here %d, %d, %d \n", nextBufferSize, currentBytesWritten, dataWritten);*/
	}
	if(currentBytesWritten == -1) {
		return -1;
	}
	return dataWritten;
}

int readAll(char *fileName, char *buffer) {
	int file, currentBytesRead, dataRead;
	currentBytesRead = 0;
	dataRead = 0;

	file = open(fileName,O_RDONLY);
	if(file == -1) {
		return -1;
	}
	while((currentBytesRead = read(file, &buffer[dataRead], BUFFER_SIZE)) != 0) {
		if(currentBytesRead == -1) {
			return -1;
		}
		dataRead += currentBytesRead;
	}
	truncateBuffer(buffer, dataRead);
	return dataRead;
}

int cat(char *fileName, int size) {
	int file, currentBytesRead, dataRead, nextBufferSize;
	char buffer[BUFFER_SIZE];
	currentBytesRead = 0;
	dataRead = 0;
	nextBufferSize = BUFFER_SIZE;

	file = open(fileName,O_RDONLY);
	if(file == -1) {
		return -1;
	}
	nextBufferSize = (dataRead+nextBufferSize < size) ? nextBufferSize : size-dataRead ;
	while((currentBytesRead = read(file, buffer, nextBufferSize)) != 0) {
		if(size != -1 && dataRead > size) {
			break;
		}
		if(currentBytesRead == -1) {
			return -1;
		}
		truncateBuffer(buffer, currentBytesRead);
		printf("%s", buffer);
		dataRead += currentBytesRead;
		nextBufferSize = (dataRead+nextBufferSize < size) ? nextBufferSize : size-dataRead ;
	}
	return dataRead;
}

int tail(char *fileName, int size) {
	int file, currentBytesRead, dataRead, nextBufferSize;
	char buffer[BUFFER_SIZE];
	currentBytesRead = 0;
	dataRead = 0;
	nextBufferSize = BUFFER_SIZE;

	if((file = open(fileName,O_RDONLY)) == -1) {
		return -1;
	}
	if((s = lseek(file, -size, SEEK_END)) == -1) {
		return -1;
	}
	nextBufferSize = (dataRead+nextBufferSize < size) ? nextBufferSize : size-dataRead ;
	while((currentBytesRead = read(file, buffer, nextBufferSize)) != 0) {
		if(size != -1 && dataRead > size) {
			break;
		}
		if(currentBytesRead == -1) {
			return -1;
		}
		truncateBuffer(buffer, currentBytesRead);
		printf("%s", buffer);
		dataRead += currentBytesRead;
		nextBufferSize = (dataRead+nextBufferSize < size) ? nextBufferSize : size-dataRead ;
	}
	return dataRead;
}

void truncateBuffer(char *buffer, int size) {
    int i;
    i = size;
    while(buffer[i] != '\0') {
        buffer[i] = 0;
    }
}