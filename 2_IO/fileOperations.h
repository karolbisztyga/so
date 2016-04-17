#ifndef FILE_OPERATIONS
#define FILE_OPERATIONS

int writeAll(char*, char*, int);

int readAll(char*, char*);

int cat(char*, int);

int tail(char*, int);

void truncateBuffer(char*, int);

#endif