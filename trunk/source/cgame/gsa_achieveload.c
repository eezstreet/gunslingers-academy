#include "gsa_achieve.h"

#ifdef ACHIEVEMENTS
void GSA_SaveAC(void);

void GSA_LoadAchievementFile(const char *filename, const char *path)
{
	//COMPLOX STOOF
	fileHandle_t f;
	char buffer[1024];
	int i;
	int j = 1;
	int k = 0;
	char achieve[64];

	int len = trap_FS_FOpenFile(va("%s/%s.adx", path, filename), &f, FS_READ);
	Com_Printf("Loading %s/%s.adx...", path, filename);

	if(!f || !len)
	{
		if(!len)
			trap_FS_FCloseFile(f);
		Com_Printf("failed. File nonexistant or blank");
		return;
	}
	//TODO: Encryption?
	//Formatted thusly:
	// *(count)*(count) etc
	trap_FS_Read(buffer, len, f);
	trap_FS_FCloseFile(f);
	buffer[len] = '\0';
	
	i = 0;
	//for(i = 0; i < len; i++)
	while(buffer[i] != '\0' && i < len)
	{
		if(buffer[i] == '*')
		{
			while(buffer[i] != '\0' && buffer[i] != '*')
			{
				i++;
				j++;
			}
			Q_strncpyz(achieve, buffer+i+2-j, j+2); //I AM WIZURD
			cgs.achievements[k].count = atoi(achieve);
			cgs.achievements[k].baseData = &GSA_achieveList[k];
			if(cgs.achievements[k].count >= cgs.achievements[k].baseData->acquireCount)
			{
				cgs.achievements[k].achieved = qtrue;
				thisAchieve = &cgs.achievements[k];
				cgs.achievements[k].baseData->onAchieve(qfalse);
			}
			else
				cgs.achievements[k].achieved = qfalse;
			k++;
			j = 1;
			achieve[0] = '\0';
		}
		i++;
	}
	GSA_SaveAC();
}

void GSA_InitializeClientAchievementData(void)
{
	int i;
	for(i = 0; i < A_MAX; i++)
	{
		cgs.achievements[i].baseData = &GSA_achieveList[i];
		cgs.achievements[i].count = 0;
		cgs.achievements[i].achieved = qfalse;
	}
}

void GSA_SaveAchievements(const char *fileName, const char *path)
{
	fileHandle_t f;
	int i = 0;
	int k = 0;
	int len = trap_FS_FOpenFile(va("%s/%s.adx", path, fileName), &f, FS_WRITE);
	char buffer[1024];

	buffer[0] = '\0';
	for(i = 0; i < A_MAX; i++)
	{
		strcat(buffer, va("*%i", cgs.achievements[k].count));
		k++;
	}
	trap_FS_Write(buffer, strlen(buffer) + 1, f);
	trap_FS_FCloseFile(f);
	cg.acSaveTime = cg.time;
}

void GSA_IncrementAchievement(iAchieve_t *achievement, int amount)
{
	thisAchieve = achievement;
	achievement->count += amount;
	if(achievement->count >= achievement->baseData->acquireCount && !achievement->achieved)
	{
		achievement->achieved = qtrue;
		achievement->baseData->onAchieve(qtrue);
		GSA_SaveAchievements("acx", "sav_data");
	}
}

void GSA_InitiateAchievements(void)
{
	GSA_InitializeClientAchievementData();
	GSA_LoadAchievementFile("acx", "sav_data");
}

void GSA_SaveAC(void)
{
	GSA_SaveAchievements("acx", "sav_data");
}
#endif