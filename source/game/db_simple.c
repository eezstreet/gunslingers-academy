#include "db_simple.h"


extern int		trap_FS_FOpenFile( const char *qpath, fileHandle_t *f, fsMode_t mode );
extern void	trap_FS_Read( void *buffer, int len, fileHandle_t f );
extern void	trap_FS_FCloseFile( fileHandle_t f );
internalDatabase_t *DB_OpenFile(const char * const fileName)
{
	int i = 0;
	qboolean noMore = qfalse;
	internalDatabase_t *database = (internalDatabase_t *)malloc(sizeof(internalDatabase_t));
	database->len = trap_FS_FOpenFile((const char *)fileName, &database->handle, FS_READ);

	//Fixed: ~eezstreet
	database->buffer = (char *)malloc(MAX_FIELDSIZE * MAX_DBCOLS * MAX_DBROWS);
	database->fileName = (char *)malloc(MAX_QPATH);
	memset(database->buffer, 0, sizeof(database->buffer));
	memset(database->fileName, 0, sizeof(database->fileName));

	//Best to initialize these like Xyc says, that way we don't try to increment uninitialized ints...
	database->rows = 0;
	database->cols = 0;

	if(!database->handle){
		free(database);
		Com_Error(ERR_FATAL, "db_simple: %s NULL handle", fileName);
		return NULL;
	}

	if(database->len >= MAX_DBSIZE){
		trap_FS_FCloseFile(database->handle);
		free(database);
		Com_Error(ERR_FATAL, "db_simple: %s >= MAX_DBSIZE", fileName);
		return NULL;
	}

	Q_strncpyz(database->fileName, fileName, MAX_QPATH);

	trap_FS_Read(database->buffer, database->len, database->handle);
	database->buffer[database->len] = '\0';
	trap_FS_FCloseFile(database->handle);

	//Time to start string parsing.
	while(database->buffer[i] != '\0')
	{
		if(database->buffer[i] == '\t' && !noMore)
		{
			database->cols++;
		}
		else if(database->buffer[i] == '\n')
		{
			if(!noMore){
				database->cols++;
				noMore = qtrue;
			}

			database->rowLocs[database->rows] = i+1;
			database->rows++;
		}
		i++;
	}

	return database;
}

void DB_CloseDatabase(internalDatabase_t *database)
{
	free(database->fileName);
	free(database->buffer);
	free(database);
}

int DB_RetrieveValue(internalDatabase_t *database, int column, int row, char *buffer, unsigned int bufferLen)
{
	int i = 0;
	unsigned int j = 0;
	if(!database)
	{
		Com_Error(ERR_FATAL, "DB_RetrieveValue: %s NULL database", database->fileName);
		DB_CloseDatabase(database); //May be unsafe.
		return 0;
	}
	if(column >= database->cols)
	{
		Com_Error(ERR_DROP, "DB_RetrieveValue: %s: nonexistant column (%i), max %i", database->fileName, column, database->cols);
		DB_CloseDatabase(database);
		return 0;
	}
	if(row >= database->rows)
	{
		Com_Error(ERR_DROP, "DB_RetrieveValue: %s: nonexistant row (%i), max %i", database->fileName, row, database->rows);
		DB_CloseDatabase(database);
		return 0;
	}

	i = database->rowLocs[row];
	while(j < column)
	{
		if(database->buffer[i] == '\t')
			j++;
		else if(database->buffer[i] == '\n')
			return 0;

		i++;
	}
	j = 0;
	//Cool. Found where we need to start. Now just copy until we reach the \t
	while(database->buffer[i] != '\t' && j < (bufferLen - 1))
	{
		buffer[j] = database->buffer[i];
		j++;
		i++;
	}
	buffer[j] = '\0';
	return 1;
}
