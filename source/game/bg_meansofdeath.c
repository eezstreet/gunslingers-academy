#include "q_shared.h"
#include "bg_public.h"
#include "bg_local.h"

#define MOD_TXT_FILE_SIZE	65535

modData_t modData[MAX_MEANSOFDEATH];

unsigned int numMODs;
#if MAC_PORT //not really sure what i'm doing, but the version with all the ##s didn't seem to make sense -redsaurus
#define READVALUE(a,b,c,d,e)	if(DB_RetrieveValue(database, a, b, buffer, c)) d[b].e = atoi(buffer);
#define READSTRING(a,b,c,d,e)	if(DB_RetrieveValue(database, a, b, buffer, c)) strncpy(d[b].e, buffer, c);
#define READCSTRING(a,b,c,d,e)	if(DB_RetrieveValue(database, a, b, buffer, c)) strncpy((char *)d[b].e, buffer, c);
#define READSHORT(a,b,c,d,e)	if(DB_RetrieveValue(database, a, b, buffer, c)) d[b].e = (short)atoi(buffer);
#define READUSHORT(a,b,c,d,e)	if(DB_RetrieveValue(database, a, b, buffer, c)) d[b].e = (unsigned short)atoi(buffer);
#define READFLOAT(a,b,c,d,e)	if(DB_RetrieveValue(database, a, b, buffer, c)) d[b].e = atof(buffer);
#define READUINT(a,b,c,d,e)		if(DB_RetrieveValue(database, a, b, buffer, c)) d[b].e = (unsigned int)atoi(buffer);
#define READQBOOL(a,b,c,d,e)	if(DB_RetrieveValue(database, a, b, buffer, c)) d[b].e = (qboolean)atoi(buffer);
#define READSBYTE(a,b,c,d,e)	if(DB_RetrieveValue(database, a, b, buffer, c)) d[b].e = (signed char)atoi(buffer);
#define READBYTE(a,b,c,d,e)		if(DB_RetrieveValue(database, a, b, buffer, c)) d[b].e = (unsigned char)atoi(buffer);

#else
#define READVALUE(a,b,c,d,e)	if(DB_RetrieveValue(database, ##a##, ##b##, buffer, ##c##)) ##d##[##b##].##e## = atoi(buffer);
#define READSTRING(a,b,c,d,e)	if(DB_RetrieveValue(database, ##a##, ##b##, buffer, ##c##)) strncpy(##d##[##b##].##e##, buffer, ##c##);
#define READCSTRING(a,b,c,d,e)	if(DB_RetrieveValue(database, ##a##, ##b##, buffer, ##c##)) strncpy((char *)##d##[##b##].##e##, buffer, ##c##);
#define READSHORT(a,b,c,d,e)	if(DB_RetrieveValue(database, ##a##, ##b##, buffer, ##c##)) ##d##[##b##].##e## = (short)atoi(buffer);
#define READUSHORT(a,b,c,d,e)	if(DB_RetrieveValue(database, ##a##, ##b##, buffer, ##c##)) ##d##[##b##].##e## = (unsigned short)atoi(buffer);
#define READFLOAT(a,b,c,d,e)	if(DB_RetrieveValue(database, ##a##, ##b##, buffer, ##c##)) ##d##[##b##].##e## = atof(buffer);
#define READUINT(a,b,c,d,e)		if(DB_RetrieveValue(database, ##a##, ##b##, buffer, ##c##)) ##d##[##b##].##e## = (unsigned int)atoi(buffer);
#define READQBOOL(a,b,c,d,e)	if(DB_RetrieveValue(database, ##a##, ##b##, buffer, ##c##)) ##d##[##b##].##e## = (qboolean)atoi(buffer);
#define READSBYTE(a,b,c,d,e)	if(DB_RetrieveValue(database, ##a##, ##b##, buffer, ##c##)) ##d##[##b##].##e## = (signed char)atoi(buffer);
#define READBYTE(a,b,c,d,e)		if(DB_RetrieveValue(database, ##a##, ##b##, buffer, ##c##)) ##d##[##b##].##e## = (unsigned char)atoi(buffer);
#endif

extern internalDatabase_t *DB_OpenFile(const char * const fileName);
extern int DB_RetrieveValue(internalDatabase_t *database, int column, int row, char *buffer, int bufferLen);
extern void DB_CloseDatabase(internalDatabase_t *database);

void GSA_LoadMODTXT(const char *directory, const char *fileName, modData_t *modDataArray)
{
	int i;
	char buffer[MOD_TXT_FILE_SIZE];
	fileHandle_t f;
	int len = trap_FS_FOpenFile(va("%s/%s", directory, fileName), &f, FS_READ);
	internalDatabase_t *database;

	numMODs = 0;

	if(!f || !len)
	{
		if(!len)
			trap_FS_FCloseFile(f);
		Com_Error(ERR_FATAL, "GSA_LoadMODTXT: NULL handle");
		return;
	}
	else
	{
		database = DB_OpenFile(va("%s/%s", directory, fileName));
		trap_FS_FCloseFile(f);
	}
	memset(buffer, '\0', sizeof(buffer));

	for(i = 0; i < database->rows-1; i++)
	{
		READUINT(MODD_INDEX, i, MOD_TXT_FILE_SIZE, modDataArray, index)
		READCSTRING(MODD_INTERNAL, i, MOD_TXT_FILE_SIZE, modDataArray, internalName)
		READCSTRING(MODD_DEATH, i, MOD_TXT_FILE_SIZE, modDataArray, deathMessage)
		READCSTRING(MODD_SUICIDE, i, MOD_TXT_FILE_SIZE, modDataArray, suicideString)
		READCSTRING(MODD_SUICIDE_FEMALE, i, MOD_TXT_FILE_SIZE, modDataArray, femaleSuicideString)
		READCSTRING(MODD_SUICIDE_NEUTER, i, MOD_TXT_FILE_SIZE, modDataArray, neuterSuicideString)
		READSBYTE(MODD_HEADSHOT, i, MOD_TXT_FILE_SIZE, modDataArray, headShot)
		READBYTE(MODD_HS_LINE, i, MOD_TXT_FILE_SIZE, modDataArray, headShotLine)
		READQBOOL (MODD_BLOODSPRAY, i, MOD_TXT_FILE_SIZE, modDataArray, bloodSpray)
		READUINT(MODD_BLOODSPLASHTIME, i, MOD_TXT_FILE_SIZE, modDataArray, bloodSplashTime)
		READUINT(MODD_BLOODSPLASHRADIUS, i, MOD_TXT_FILE_SIZE, modDataArray, bloodSplashRadius)
		READQBOOL(MODD_USECHOKESOUND, i, MOD_TXT_FILE_SIZE, modDataArray, useChokeSound)
	}
	DB_CloseDatabase(database);
}