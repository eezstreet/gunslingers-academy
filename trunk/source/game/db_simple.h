
#ifndef DBSIMPLEH
#define DBSIMPLEH

#include "q_shared.h"

#define MAX_DBCOLS		128
#define MAX_DBROWS		2048
#define MAX_DBSIZE		65536
#define MAX_FIELDSIZE	8

typedef struct {
	unsigned short cols;
	unsigned short rows;
	int rowLocs[MAX_DBROWS];
	char *buffer;
	char *fileName;
	fileHandle_t handle;
	int len;
} internalDatabase_t;

#endif