#ifndef _CG_PLAYERSOUNDS_H
#define _CG_PLAYERSOUNDS_H

#include "cg_local.h"
#include "cg_ini.h"

typedef struct
{
	//This struct defines what goes into a character's sound effects
	int numDeathSounds;			//Number of death sounds
	int numPainSounds;			//Number of pain sounds
	qboolean JKApainLike;		//Uses JKA styling for pain sounds (pain100, pain75, pain50, ...)
	int numTaunts;				//Number of taunts
	int numLands;				//Number of hard landing sounds
	int numJumps;				//Number of jumping sounds
	int numChokeSounds;			//Number of choke sounds
	int	numBlindSounds;			//Number of blind sounds (UNUSED)
	char directory[MAX_QPATH];	//Directory where sounds can be found
	char model[MAX_QPATH];		//The model that this characterSound_t uses.
} characterSound_t;

characterSound_t characterSounds[512];
static int lastUsedIndex;		

int RegisterCharacterSound(char *model);
int GetCharacterIndexFromModel(char *model);
void PrecacheClientSound(clientInfo_t *ci);
void PlayClientSound(clientInfo_t *ci, byte soundType, unsigned short clientNum, vec3_t origin);
void InitPlayerSounds(void);

qboolean FS_FileValid(char *path);

#endif