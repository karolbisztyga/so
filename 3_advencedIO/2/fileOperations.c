#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <aio.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include "fileOperations.h"

#define BUFFER_SIZE 8

int printContent(char *fileName, char *buffer) {
	struct aiocb aiocb;

	char option[1], aioReadData, aioError;
	int file, fileSize, dataRead, nextBufferSize;

	printf("do you want to print file [%s]?(y/n)\n", fileName);
	scanf("%s", option);
	if(option[0] != 'y') {
		return 0;
	}
	if((file = open(fileName, O_RDONLY)) == -1) {
		return -1;
	}
	nextBufferSize = BUFFER_SIZE;
	dataRead = 0;
	fileSize = lseek(file, 0, SEEK_END);
	lseek(file, 0, SEEK_SET);
	nextBufferSize = (dataRead+nextBufferSize < fileSize) ? nextBufferSize : fileSize-dataRead ;
	while(dataRead < fileSize) {
		lseek(file, nextBufferSize, SEEK_CUR);
		memset(&aiocb, 0, sizeof(struct aiocb));
		aiocb.aio_fildes = file;
		aiocb.aio_buf = buffer;
		aiocb.aio_nbytes = nextBufferSize;
		aiocb.aio_lio_opcode = LIO_WRITE;

		if((aioReadData = aio_read(&aiocb)) == -1) {
			return -1;
		}

		while((aioError = aio_error(&aiocb)) == EINPROGRESS) {
			/*printf("processing...\n");*/
		}

		if((aioError = aio_error(&aiocb)) < 0) {
			return -1;
		}
		truncateBuffer(buffer, nextBufferSize);
		printf("%s", buffer);
		dataRead += nextBufferSize;
		nextBufferSize = (dataRead+nextBufferSize < fileSize) ? nextBufferSize : fileSize-dataRead ;
		/*printf("%lx bytes read, %d/%d\n\n", sizeof(buffer), dataRead, fileSize);*/
	}
	printf("\nsize: %d\n", fileSize);
	close(file);
	return 0;
}

void truncateBuffer(char *buffer, int size) {
    int i;
    i = size;
    while(buffer[i] != '\0') {
        buffer[i] = 0;
    }
}