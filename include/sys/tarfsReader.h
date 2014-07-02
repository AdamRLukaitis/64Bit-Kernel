#ifndef TARFS
#define TARFS


#define BLOCKSIZE 512

#include <sys/tarfs.h>
#include <sys/writeUtil.h>

int getSize(char *p);

char* ReadFile(char *fileName);

#endif