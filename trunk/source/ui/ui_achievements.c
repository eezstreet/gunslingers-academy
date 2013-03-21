#include "ui_local.h"
#include "ui_shared.h"

#ifdef ACHIEVEMENTS
#define  CAP(x,y)	if(x > y) x = y
extern uiInfo_t uiInfo;
//extern achieve_t GSA_achieveList[A_MAX];
//achieve_t GSA_achieveList[A_MAX];

qboolean abortc;

achieve_t GSA_achieveList[] =
{
		{
		"Sidearm",
		"Kill 10 people using the M&P 40",
		10,
		"sound/interface/secret_area",
		"",
		FEEDER_SIDEARM,
		0,
		NULL
	},
	{
		"Sidearm II",
		"Kill 25 people using the M&P 40",
		25,
		"sound/interface/secret_area",
		"",
		FEEDER_SIDEARM,
		0,
		NULL
	},
	{
		"Sidearm III",
		"Kill 50 people using the M&P 40",
		50,
		"sound/interface/secret_area",
		"",
		FEEDER_SIDEARM,
		0,
		NULL
	},
	{
		"Suicidal",
		"Die by falling.",
		1,
		"sound/interface/secret_area",
		"",
		FEEDER_BASE,
		0,
		NULL
	},
	{
		"Six-Shooter",
		"Kill 10 people using the Magnum",
		10,
		"sound/interface/secret_area",
		"",
		FEEDER_SIDEARM,
		0,
		NULL
	},
	{
		"Six-Shooter II",
		"Kill 25 people using the Magnum",
		25,
		"sound/interface/secret_area",
		"",
		FEEDER_SIDEARM,
		0,
		NULL
	},
	{
		"Six-Shooter III",
		"Kill 50 people using the Magnum",
		50,
		"sound/interface/secret_area",
		"",
		FEEDER_SIDEARM,
		0,
		NULL
	},
	{
		"Soldier",
		"Kill 10 people using the M16",
		10,
		"sound/interface/secret_area",
		"",
		FEEDER_PRIMARY,
		0,
		NULL
	},
	{
		"Soldier II",
		"Kill 25 people using the M16",
		25,
		"sound/interface/secret_area",
		"",
		FEEDER_PRIMARY,
		0,
		NULL
	},
	{
		"Soldier III",
		"Kill 50 people using the M16",
		50,
		"sound/interface/secret_area",
		"",
		FEEDER_PRIMARY,
		0,
		NULL
	},
	{
		"Reconnaissance",
		"Call in a Recon Team",
		1,
		"sound/interface/secret_area",
		"",
		FEEDER_EQUIPMENT,
		0,
		NULL
	},
	{
		"Sentry",
		"Get 5 kills using a Sentry Gun",
		5,
		"sound/interface/secret_area",
		"",
		FEEDER_EQUIPMENT,
		0,
		NULL
	},
	{
		"TFFA Winner",
		"Win a game of Team Deathmatch",
		1,
		"sound/interface/secret_area",
		"",
		FEEDER_OPERATIONS,
		0,
		NULL
	},
	{
		"TFFA Master",
		"Win 5 games of Team Deathmatch",
		5,
		"sound/interface/secret_area",
		"",
		FEEDER_OPERATIONS,
		0,
		NULL
	},
	{
		"TFFA Master II",
		"Win 10 games of Team Deathmatch",
		10,
		"sound/interface/secret_area",
		"",
		FEEDER_OPERATIONS,
		0,
		NULL
	},
	{
		"TFFA Master III",
		"Win 25 games of Team Deathmatch",
		25,
		"sound/interface/secret_area",
		"",
		FEEDER_OPERATIONS,
		0,
		NULL
	},
	{
		"TFFA Master IV",
		"Win 50 games of Team Deathmatch",
		50,
		"sound/interface/secret_area",
		"",
		FEEDER_OPERATIONS,
		0,
		NULL
	},
	{
		"Impossible Odds",
		"Feat of Valor: Played online against the Gunslinger's Academy developers.",
		1,
		"sound/interface/secret_area",
		"",
		FEEDER_FEATS,
		0,
		NULL
	},
	{
		"Point Guard I",
		"Get 5 assists.",
		5,
		"sound/interface/secret_area",
		"",
		FEEDER_BASE,
		0,
		NULL
	},
	{
		"Point Guard II",
		"Get 10 assists.",
		10,
		"sound/interface/secret_area",
		"",
		FEEDER_BASE,
		0,
		NULL
	},
	{
		"Point Guard III",
		"Get 20 assists.",
		20,
		"sound/interface/secret_area",
		"",
		FEEDER_BASE,
		0,
		NULL
	},
	{
		"White Belt",
		"Kill 1 person using your fists.",
		1,
		"sound/interface/secret_area",
		"",
		FEEDER_BASE,
		0,
		NULL
	},
	{
		"Blue Belt",
		"Kill 5 people using your fists.",
		5,
		"sound/interface/secret_area",
		"",
		FEEDER_BASE,
		0,
		NULL
	},
	{
		"Black Belt",
		"Kill 10 people using your fists.",
		10,
		"sound/interface/secret_area",
		"",
		FEEDER_BASE,
		0,
		NULL
	},
	{
		"Sleight of Hand",
		"Manually reload 50 times.",
		50,
		"sound/interface/secret_area",
		"",
		FEEDER_FEATS,
		0,
		NULL
	},
	{
		"On-Duty",
		"Kill 10 people using the TASER",
		10,
		"sound/interface/secret_area",
		"",
		FEEDER_SIDEARM,
		0,
		NULL
	},
	{
		"On-Duty II",
		"Kill 25 people using the TASER",
		25,
		"sound/interface/secret_area",
		"",
		FEEDER_SIDEARM,
		0,
		NULL
	},
	{
		"On-Duty III",
		"Kill 50 people using the TASER",
		50,
		"sound/interface/secret_area",
		"",
		FEEDER_SIDEARM,
		0,
		NULL
	},
	{
		"Master Blaster",
		"Kill 10 people using the Glock 18c",
		10,
		"sound/interface/secret_area",
		"",
		FEEDER_SIDEARM,
		0,
		NULL
	},
	{
		"Master Blaster II",
		"Kill 25 people using the Glock 18c",
		25,
		"sound/interface/secret_area",
		"",
		FEEDER_SIDEARM,
		0,
		NULL
	},
	{
		"Master Blaster III",
		"Kill 50 people using the Glock 18c",
		50,
		"sound/interface/secret_area",
		"",
		FEEDER_SIDEARM,
		0,
		NULL
	},
	{
		"AK-1337",
		"Kill 10 people using the AK-104",
		10,
		"sound/interface/secret_area",
		"",
		FEEDER_PRIMARY,
		0,
		NULL
	},
	{
		"AK-1337 II",
		"Kill 25 people using the AK-104",
		25,
		"sound/interface/secret_area",
		"",
		FEEDER_PRIMARY,
		0,
		NULL
	},
	{
		"AK-1337 III",
		"Kill 50 people using the AK-104",
		50,
		"sound/interface/secret_area",
		"",
		FEEDER_PRIMARY,
		0,
		NULL
	},
	{
		"Frag Master",
		"Kill 10 people using frag grenades.",
		10,
		"sound/interface/secret_area",
		"",
		FEEDER_EQUIPMENT,
		0,
		NULL
	},
	{
		"Frag Master II",
		"Kill 25 people using frag grenades.",
		25,
		"sound/interface/secret_area",
		"",
		FEEDER_EQUIPMENT,
		0,
		NULL
	},
	{
		"Frag Master III",
		"Kill 50 people using frag grenades.",
		50,
		"sound/interface/secret_area",
		"",
		FEEDER_EQUIPMENT,
		0,
		NULL
	},
	{
		"Frag Master IV",
		"Kill 75 people using frag grenades.",
		75,
		"sound/interface/secret_area",
		"",
		FEEDER_EQUIPMENT,
		0,
		NULL
	},
	{
		"Frag Master V",
		"Kill 100 people using frag grenades.",
		100,
		"sound/interface/secret_area",
		"",
		FEEDER_EQUIPMENT,
		0,
		NULL
	},
	{
		"Magnum Headhunter",
		"Get 5 headshots using the Magnum",
		5,
		"sound/interface/secret_area",
		"",
		FEEDER_SIDEARM,
		0,
		NULL
	},
	{
		"Magnum Headhunter II",
		"Get 10 headshots using the Magnum.",
		10,
		"sound/interface/secret_area",
		"",
		FEEDER_SIDEARM,
		0,
		NULL
	},
	{
		"Magnum Headhunter III",
		"Get 25 headshots using the Magnum.",
		25,
		"sound/interface/secret_area",
		"",
		FEEDER_SIDEARM,
		0,
		NULL
	},
	{
		"M14 Headhunter",
		"Get 5 headshots using the M14",
		5,
		"sound/interface/secret_area",
		"",
		FEEDER_PRIMARY,
		0,
		NULL
	},
	{
		"M14 Headhunter II",
		"Get 10 headshots using the M14.",
		10,
		"sound/interface/secret_area",
		"",
		FEEDER_PRIMARY,
		0,
		NULL
	},
	{
		"M14 Headhunter III",
		"Get 25 headshots using the M14.",
		25,
		"sound/interface/secret_area",
		"",
		FEEDER_PRIMARY,
		0,
		NULL
	},
	{
		"M98C Headhunter",
		"Get 5 headshots using the M98C",
		5,
		"sound/interface/secret_area",
		"",
		FEEDER_PRIMARY,
		0,
		NULL
	},
	{
		"M98C Headhunter II",
		"Get 10 headshots using the M98C.",
		10,
		"sound/interface/secret_area",
		"",
		FEEDER_PRIMARY,
		0,
		NULL
	},
	{
		"M98C Headhunter III",
		"Get 25 headshots using the M98C.",
		25,
		"sound/interface/secret_area",
		"",
		FEEDER_PRIMARY,
		0,
		NULL
	},
	{
		"Bullet For My Valentine",
		"Feat of Valor: Kill someone on Valentine's Day",
		1,
		"sound/interface/secret_area",
		"",
		FEEDER_FEATS,
		0,
		NULL
	},
	{
		"Soothsayer",
		"Feat of Valor: Kill someone on the Ides of March",
		1,
		"sound/interface/secret_area",
		"",
		FEEDER_FEATS,
		0,
		NULL
	},
	{
		"In Memoriam",
		"Feat of Valor: Kill someone on Memorial Day",
		1,
		"sound/interface/secret_area",
		"",
		FEEDER_FEATS,
		0,
		NULL
	},
	{
		"Fight for Independence",
		"Feat of Valor: Kill someone on July 4th",
		1,
		"sound/interface/secret_area",
		"",
		FEEDER_FEATS,
		0,
		NULL
	},
	{
		"Creepy Crawl",
		"Feat of Valor: Kill someone on Halloween",
		1,
		"sound/interface/secret_area",
		"",
		FEEDER_FEATS,
		0,
		NULL
	},
	{
		"Veteran vrs. Noob",
		"Feat of Valor: Kill someone on Veteran's Day",
		1,
		"sound/interface/secret_area",
		"",
		FEEDER_FEATS,
		0,
		NULL
	},
	{
		"...a Date in Infamy",
		"Feat of Valor: Kill someone on Pearl Harbor Remembrance Day",
		1,
		"sound/interface/secret_area",
		"",
		FEEDER_FEATS,
		0,
		NULL
	},
	{
		"Anthrax",
		"Kill 10 people using gas",
		10,
		"sound/interface/secret_area",
		"",
		FEEDER_EQUIPMENT,
		0,
		NULL
	},
	{
		"Anthrax II",
		"Kill 25 people using gas",
		25,
		"sound/interface/secret_area",
		"",
		FEEDER_EQUIPMENT,
		0,
		NULL
	},
	{
		"Anthrax III",
		"Kill 50 people using gas",
		50,
		"sound/interface/secret_area",
		"",
		FEEDER_EQUIPMENT,
		0,
		NULL
	},
	{
		"Full House",
		"Feat of Valor: Attended an Open House",
		1,
		"sound/interface/secret_area",
		"",
		FEEDER_FEATS,
		0,
		NULL
	},
	{
		"Any Means Necessary",
		"Kicked down a locked door.",
		1,
		"sound/interface/secret_area",
		"",
		FEEDER_BASE,
		0,
		NULL
	},
};

void GSA_UI_SaveAchievements(const char *fileName, const char *path)
{
	fileHandle_t f;
	int i = 0;
	int k = 0;
	char buffer[1024];

	buffer[0] = '\0';

	trap_FS_FOpenFile(va("%s/%s.adx", path, fileName), &f, FS_WRITE);
	for(i = 0; i < A_MAX; i++)
	{
		strcat(buffer, va("*%i", myAchievements[i].count));
		k++;
	}
	trap_FS_Write(buffer, strlen(buffer) + 1, f);
	trap_FS_FCloseFile(f);
}

void GSA_UI_LoadAchievementFile(const char *filename, const char *path)
{
	//COMPLOX STOOF
	fileHandle_t f;
	char buffer[1024];
	int i;
	int j = 1;
	int k = 0;
	char achieve[64];

	int len = trap_FS_FOpenFile(va("%s/%s.adx", path, filename), &f, FS_READ);

	//It's important with UI that we do this:
	for(i = 0; i < 256; i++)
		memset(&myAchievements[i], 0, sizeof(iAchieve_t));

	if(!f || !len)
	{
		if(!len)
			trap_FS_FCloseFile(f);
		abortc = qtrue;
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
			myAchievements[k].count = atoi(achieve);
			myAchievements[k].baseData = &GSA_achieveList[k];
			if(myAchievements[k].count >= myAchievements[k].baseData->acquireCount)
			{
				myAchievements[k].achieved = qtrue;
				if(myAchievements[k].baseData->feeder == FEEDER_FEATS)
				{
					myAchievements[k].baseData->feeder = FEEDER_FEATEARNED;
				}
				else
				{
					myAchievements[k].baseData->feeder = FEEDER_EARNED;
				}
			}
			else
				myAchievements[k].achieved = qfalse;
			k++;
			j = 1;
			achieve[0] = '\0';
		}
		i++;
	}
}

int GSA_UI_NumberInAchieveFeeder(unsigned int feeder)
{
	int i;
	int count = 0;
	for (i = 0; i < A_MAX; i++)
	{
		if(myAchievements[i].baseData->feeder == feeder)
			count++;
	}
	return count;
}

void GSA_UI_SortIntoFeeders(void)
{
	int i;
	int j[1+FEEDER_FEATEARNED-FEEDER_FEATS];
	if(abortc)
		return;
	for(i = 0; i < 1+FEEDER_FEATEARNED-FEEDER_FEATS; i++)
		j[i] = 0;
	for(i = 0; i < 1+FEEDER_FEATEARNED-FEEDER_FEATS; i++)
	{
		int j = 0;
		for(;j < A_MAX; j++)
			achievementFeeders[i][j][0] = '\0';
	}
	for(i = 0; i < A_MAX; i++)
	{
		int feeder = myAchievements[i].baseData->feeder;
		//j is the feeders
		strcpy(achievementFeeders[feeder-FEEDER_FEATS][j[feeder-FEEDER_FEATS]], myAchievements[i].baseData->name);
		j[feeder-FEEDER_FEATS]++;
	}
}

int retrieveAchieveIndexByName(const char *comparison)
{
	int i;
	for(i = 0; i < A_MAX; i++)
	{
		if(Q_stricmp(GSA_achieveList[i].name, comparison) == 0)
			return i;
	}
	return -1;
}

void GSA_UI_AchieveFeederAction(int index, int feederID)
{
	int ijk = retrieveAchieveIndexByName(achievementFeeders[feederID-FEEDER_FEATS][index]);
	if(ijk < 0)
		return;
	trap_Cvar_Set( "ui_currentAchieve", achievementFeeders[feederID-FEEDER_FEATS][index] );
	trap_Cvar_Set( "ui_currentAchieveDesc", GSA_achieveList[ijk].description );
	CAP(myAchievements[ijk].count, myAchievements[ijk].baseData->acquireCount);
	trap_Cvar_Set( "ui_currentAchieveProgress", va("%i/%i", myAchievements[ijk].count, myAchievements[ijk].baseData->acquireCount) );
}
#endif