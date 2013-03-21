// bg_killstreaks.c -- killstreaks and such

#include "q_shared.h"
#include "bg_public.h"
#include "bg_local.h"

ksData_t killstreakData[MAX_KILLSTREAK_AWARDS];
unsigned int numKS;

#define KS_TXT_FILE_SIZE	65535

#ifdef _WIN32
#define READVALUE(a,b,c,d,e)	if(DB_RetrieveValue(database, ##a##, ##b##, buffer, ##c##)) ##d##[##b##].##e## = atoi(buffer);
#define READSTRING(a,b,c,d,e)	if(DB_RetrieveValue(database, ##a##, ##b##, buffer, ##c##)) strncpy(##d##[##b##].##e##, buffer, ##c##);
#define READCSTRING(a,b,c,d,e)	if(DB_RetrieveValue(database, ##a##, ##b##, buffer, ##c##)) strncpy((char *)##d##[##b##].##e##, buffer, ##c##);
#define READSHORT(a,b,c,d,e)	if(DB_RetrieveValue(database, ##a##, ##b##, buffer, ##c##)) ##d##[##b##].##e## = (short)atoi(buffer);
#define READUSHORT(a,b,c,d,e)	if(DB_RetrieveValue(database, ##a##, ##b##, buffer, ##c##)) ##d##[##b##].##e## = (unsigned short)atoi(buffer);
#define READFLOAT(a,b,c,d,e)	if(DB_RetrieveValue(database, ##a##, ##b##, buffer, ##c##)) ##d##[##b##].##e## = atof(buffer);
#define READUINT(a,b,c,d,e)		if(DB_RetrieveValue(database, ##a##, ##b##, buffer, ##c##)) ##d##[##b##].##e## = (unsigned int)atoi(buffer);
#define READQBOOL(a,b,c,d,e)	if(DB_RetrieveValue(database, ##a##, ##b##, buffer, ##c##)) ##d##[##b##].##e## = (qboolean)atoi(buffer);
#else //not really sure what i'm doing, but the version with all the ##s didn't seem to make sense -redsaurus
#define READVALUE(a,b,c,d,e)	if(DB_RetrieveValue(database, a, b, buffer, c)) d[b].e = atoi(buffer);
#define READSTRING(a,b,c,d,e)	if(DB_RetrieveValue(database, a, b, buffer, c)) strncpy(d[b].e, buffer, c);
#define READCSTRING(a,b,c,d,e)	if(DB_RetrieveValue(database, a, b, buffer, c)) strncpy((char *)d[b].e, buffer, c);
#define READSHORT(a,b,c,d,e)	if(DB_RetrieveValue(database, a, b, buffer, c)) d[b].e = (short)atoi(buffer);
#define READUSHORT(a,b,c,d,e)	if(DB_RetrieveValue(database, a, b, buffer, c)) d[b].e = (unsigned short)atoi(buffer);
#define READFLOAT(a,b,c,d,e)	if(DB_RetrieveValue(database, a, b, buffer, c)) d[b].e = atof(buffer);
#define READUINT(a,b,c,d,e)		if(DB_RetrieveValue(database, a, b, buffer, c)) d[b].e = (unsigned int)atoi(buffer);
#define READQBOOL(a,b,c,d,e)	if(DB_RetrieveValue(database, a, b, buffer, c)) d[b].e = (qboolean)atoi(buffer);
#endif

extern internalDatabase_t *DB_OpenFile(const char * const fileName);
extern int DB_RetrieveValue(internalDatabase_t *database, int column, int row, char *buffer, int bufferLen);
extern void DB_CloseDatabase(internalDatabase_t *database);
void GSA_LoadKillstreakTXT(const char *directory, const char *BSPName, ksData_t *killstreakArray)
{
	int i;
	char buffer[KS_TXT_FILE_SIZE];
	fileHandle_t f;
#ifdef QAGAME
	int len = trap_FS_FOpenFile(va("%s/%s.txt", directory, BSPName), &f, FS_READ);
	internalDatabase_t *database;
#else
	//Extra code is needed in CGAME to strip the .bsp extension
	internalDatabase_t *database;
	int len;
	char *path = (char *)malloc(MAX_QPATH);
	char fixedPath[MAX_QPATH];

	COM_StripExtension(BSPName, path);

	strncpy(fixedPath, va("%s/%s.txt", directory, path+5), MAX_QPATH); //The +5 is to remove "maps/" from beginning of string

	len = trap_FS_FOpenFile(fixedPath, &f, FS_READ);
	free(path);
#endif

	numKS = 0;
	if(!f || !len)
	{
		if(!len)
			trap_FS_FCloseFile(f);
		database = DB_OpenFile("ext_data/ks/default.txt");
		Com_Printf("^3WARNING: %s/%s not found, reverting to default.\n", directory, BSPName);
	}
	else
	{
#ifdef QAGAME
		database = DB_OpenFile(va("%s/%s.txt", directory, BSPName));
#else
		database = DB_OpenFile(fixedPath);
#endif
		trap_FS_FCloseFile(f);
	}

	memset(buffer, '\0', sizeof(buffer));

	for(i = 0; i < database->rows-1; i++)
	{
		char rewardstring[256];
		READUINT(KSLINE_KILLS, i, KS_TXT_FILE_SIZE, killstreakArray, kill)
		if(DB_RetrieveValue(database, KSLINE_REWARD, i, buffer, KS_TXT_FILE_SIZE))
		{
			strcpy(rewardstring, buffer);

			if(!Q_stricmp(rewardstring, "KS_EWEB")){
				killstreakArray[i].reward = HI_EWEB;
				killstreakArray[i].rewardVariation = 0;
			} else if(!Q_stricmp(rewardstring, "KS_SENTRY")){
				killstreakArray[i].reward = HI_SENTRY_GUN;
				killstreakArray[i].rewardVariation = 0;
			} else if(!Q_stricmp(rewardstring, "KS_SENTRY_SILENCED")){
				killstreakArray[i].reward = HI_SENTRY_GUN;
				killstreakArray[i].rewardVariation = 1;
			} else if(!Q_stricmp(rewardstring, "KS_SENTRY_STEALTH")){
				killstreakArray[i].reward = HI_SENTRY_GUN;
				killstreakArray[i].rewardVariation = 2;
			} else if(!Q_stricmp(rewardstring, "KS_RECON")){
				killstreakArray[i].reward = HI_BINOCULARS;
				killstreakArray[i].rewardVariation = 0;
			} else if(!Q_stricmp(rewardstring, "KS_RADAR")){
				killstreakArray[i].reward = HI_BINOCULARS;
				killstreakArray[i].rewardVariation = 1;
			} else if(!Q_stricmp(rewardstring, "KS_COMM_TAP")){
				killstreakArray[i].reward = HI_BINOCULARS;
				killstreakArray[i].rewardVariation = 2;
			} else if(!Q_stricmp(rewardstring, "KS_PERSONAL_COUNTER_UAV")) {
				killstreakArray[i].reward = HI_BINOCULARS;
				killstreakArray[i].rewardVariation = 3;
			} else if(!Q_stricmp(rewardstring, "KS_RADAR_JAMMER")) {
				killstreakArray[i].reward = HI_BINOCULARS;
				killstreakArray[i].rewardVariation = 4;
			} else if(!Q_stricmp(rewardstring, "KS_IR_GOGGLES")) {
				killstreakArray[i].reward = HI_BINOCULARS;
				killstreakArray[i].rewardVariation = 5;
			} else if(!Q_stricmp(rewardstring, "KS_MISSILE")) {
				killstreakArray[i].reward = HI_AIRSTRIKE;
				killstreakArray[i].rewardVariation = 0;
			}
		}
//#define READSTRING(a,b,c,d,e)	if(DB_RetrieveValue(database, ##a##, ##b##, buffer, ##c##)) strncpy(##d##[##b##].##e##, buffer, ##c##);
		if(DB_RetrieveValue(database, KSLINE_DISPLAYTEXT, i, buffer, KS_TXT_FILE_SIZE))
			strcpy(killstreakArray[i].displayText, buffer);
		//READSTRING(KSLINE_DISPLAYTEXT, i, KS_TXT_FILE_SIZE, killstreakArray, displayText)
		if(DB_RetrieveValue(database, KSLINE_SOUND, i, buffer, KS_TXT_FILE_SIZE))
			strcpy(killstreakArray[i].soundPath, buffer);
		//READSTRING(KSLINE_SOUND, i, KS_TXT_FILE_SIZE, killstreakArray, soundPath)
		numKS++;
	}
	DB_CloseDatabase(database);
}

qboolean rewardExistsForKill(int numkills, ksData_t *killstreakArray)
{
	int i = 0;
	for(i = 0; i < numKS; i++)
	{
		if(killstreakArray[i].kill == numkills)
			return qtrue;
	}
	return qfalse;
}

int rewardVariationForReward(unsigned int reward)
{
	int i = 0;
	for( ; i < numKS; i++)
	{
		if(killstreakData[i].reward == reward)
			return (int)killstreakData[i].rewardVariation;
	}
	return -1;
}

int rewardIndexForGiTag(unsigned int giTag)
{
	int i = 0;
	for( ; i < numKS; i++)
	{
		if(killstreakData[i].reward == giTag)
			return i;
	}
	return -1;
}