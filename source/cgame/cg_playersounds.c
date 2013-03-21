#include "cg_playersounds.h"

qboolean FS_FileValid(char *path)
{
	int len;
	fileHandle_t f;

	len = trap_FS_FOpenFile(path, &f, FS_READ);
	if(!f || !len || len == -1)
	{
		trap_FS_FCloseFile(f);
		return qfalse;
	}
	trap_FS_FCloseFile(f);
	return qtrue;
}

void InitPlayerSounds(void)
{
	lastUsedIndex = 0;
}

//Copies path from sounds.cfg into the buffer
static void GrabSoundCFGPath(char *cfgName, char *buffer)
{
	fileHandle_t f;
	char buffer2[32];
	int len;
	int i = 0;
	if(!FS_FileValid(cfgName))
	{
		GrabSoundCFGPath("models/players/kyle/sounds.cfg", buffer);
		return;
	}
	len = trap_FS_FOpenFile(cfgName, &f, FS_READ);
	//Already performed file validity checks, no need to do them again.
	trap_FS_Read(buffer2, len, f);
	trap_FS_FCloseFile(f);

	//Parse up to the first line break
	while(buffer2[i] != '\0' && buffer2[i] != '\n' && buffer2[i] != '\r')
	{
		buffer[i] = buffer2[i];
		i++;
	}
	buffer[i] = '\0';
}

static void LoadCharacterINI(char *path, char *characterName)
{
	dictionary *file;
	characterSound_t *charS = &characterSounds[lastUsedIndex];
	int numPain;

	GrabSoundCFGPath(va("models/players/%s/sounds.cfg", characterName), characterName);

	//Check and make sure our file exists
	if(!FS_FileValid(path))
	{
		//Specify defaults and get the hell out of here
		strcpy(charS->directory, va("sound/chars/%s/misc", characterName));
		charS->JKApainLike = qtrue;
		strcpy(charS->model, characterName);
		charS->numBlindSounds = 0;
		charS->numChokeSounds = 3;
		charS->numDeathSounds = 3;
		charS->numJumps = 1;
		charS->numLands = 1;
		charS->numPainSounds = 4;
		charS->numTaunts = 1;
		return;
	}

	//Use our INI file to find out everything
	file = iniparser_load(path);

	strcpy(charS->directory, iniparser_getstring(file, "Core:Directory", va("sound/chars/%s/misc", characterName)));
	strcpy(charS->model, characterName);
	charS->JKApainLike = iniparser_getboolean(file, "Soundcount:ActiveJKAPainNaming", qtrue);
	charS->numBlindSounds = iniparser_getint(file, "Soundcount:numBlindSounds", 0);
	charS->numChokeSounds = iniparser_getint(file, "Soundcount:numChokeSounds", 3);
	charS->numDeathSounds = iniparser_getint(file, "Soundcount:numDeathSounds", 3);
	charS->numJumps = iniparser_getint(file, "Soundcount:numJumpSounds", 1);
	charS->numLands = iniparser_getint(file, "Soundcount:numLandSounds", 1);
	numPain = iniparser_getint(file, "Soundcount:numPainSounds", 4);
	if(charS->JKApainLike && numPain != 4)
	{
		Com_Printf("^3WARNING: numPainSounds has no functionality when ActiveJKAPainNaming is on\n");
	}
	if(charS->JKApainLike)
	{
		charS->numPainSounds = 4;
	}
	else
	{
		charS->numPainSounds = numPain;
	}
	charS->numTaunts = iniparser_getint(file, "Soundcount:numTauntSounds", 1);
}

int RegisterCharacterSound(char *model)
{
	int i;
	//Check and make sure this model hasn't been registered before
	for(i = 0; i < lastUsedIndex; i++)
	{
		if(!Q_stricmp(model, characterSounds[i].model))
		{
			return i;
		}
	}

	LoadCharacterINI(va("models/players/%s/soundini.ini", model), model);
	lastUsedIndex++;
	return lastUsedIndex;
}

int GetCharacterIndexFromModel(char *model)
{
	int i;
	for(i = 0; i < lastUsedIndex; i++)
	{
		if(!Q_stricmp(characterSounds[i].model, model))
		{
			return i;
		}
	}
	return -1;
}

void PrecacheClientSound(clientInfo_t *ci)
{
	unsigned int i;
	int index = RegisterCharacterSound(ci->modelName);
	characterSound_t msd;

	if(index < 0)
	{
		Com_Printf("^1ERROR: Could not precache client sound for model %s\n", ci->modelName);
		return;
	}

	msd = characterSounds[index];

	//Death sounds
	for(i = 1; i < msd.numDeathSounds+1; i++)
	{
		trap_S_RegisterSound(va("%s/death%i.wav", msd.directory, i));
	}
	//Pain sounds
	if(msd.JKApainLike)
	{
		trap_S_RegisterSound(va("%s/pain100.wav", msd.directory));
		trap_S_RegisterSound(va("%s/pain75.wav", msd.directory));
		trap_S_RegisterSound(va("%s/pain50.wav", msd.directory));
		trap_S_RegisterSound(va("%s/pain25.wav", msd.directory));
	}
	else
	{
		for(i = 1; i < msd.numPainSounds+1; i++)
		{
			trap_S_RegisterSound(va("%s/pain%i.wav", msd.directory, i));
		}
	}
	//Taunts
	for(i = 1; i < msd.numTaunts+1; i++)
	{
		trap_S_RegisterSound(va("%s/taunt%i.wav", msd.directory, i));
	}
	//Landings
	for(i = 1; i < msd.numLands+1; i++)
	{
		trap_S_RegisterSound(va("%s/land%i.wav", msd.directory, i));
	}
	//Jumps
	for(i = 1; i < msd.numJumps+1; i++)
	{
		trap_S_RegisterSound(va("%s/jump%i.wav", msd.directory, i));
	}
	//Choking
	for(i = 1; i < msd.numChokeSounds+1; i++)
	{
		trap_S_RegisterSound(va("%s/choke%i.wav", msd.directory, i));
	}
	//Blinding
	for(i = 1; i < msd.numBlindSounds+1; i++)
	{
		trap_S_RegisterSound(va("%s/blind%i.wav", msd.directory, i));
	}
}

void PlayClientSound(clientInfo_t *ci, byte soundType, unsigned short clientNum, vec3_t origin)
{
	int index = GetCharacterIndexFromModel(ci->modelName);
	characterSound_t msd;
	sfxHandle_t soundHandle;
	if(index < 0)
	{
		Com_Printf("^1ERROR: Invalid Client Sound index for model %s\n", ci->modelName);
		return;
	}
	msd = characterSounds[index];

	switch(soundType)
	{
		//TODO: Store the handles and use them instead of calling the register sound function
		case CST_DEATH:
			soundHandle = trap_S_RegisterSound(va("%s/death%i.wav", msd.directory, Q_irand(1, msd.numDeathSounds)));
			break;
		case CST_PAIN:
			if(msd.JKApainLike)
			{
				//Uses JKA naming conventions for sounds
				int randomizeMe = Q_irand(1, 4);
				int number;
				switch(randomizeMe)
				{
					case 1:
						number = 100;
						break;
					case 2:
						number = 75;
						break;
					case 3:
						number = 50;
						break;
					case 4:
					default:
						number = 25;
						break;
				}
				soundHandle = trap_S_RegisterSound(va("%s/pain%i", msd.directory, number));
			}
			else
			{
				soundHandle = trap_S_RegisterSound(va("%s/pain%i", msd.directory, Q_irand(1, msd.numPainSounds)));
			}
			break;
		case CST_TAUNT:
			soundHandle = trap_S_RegisterSound(va("%s/taunt%i", msd.directory, Q_irand(1, msd.numTaunts)));
			break;
		case CST_LAND:
			soundHandle = trap_S_RegisterSound(va("%s/land%i", msd.directory, Q_irand(1, msd.numLands)));
			break;
		case CST_JUMP:
			{
				int number;
				number = Q_irand(1, msd.numJumps);
				soundHandle = trap_S_RegisterSound(va("%s/jump%i", msd.directory, number));
			}
			break;
		case CST_CHOKE:
			soundHandle = trap_S_RegisterSound(va("%s/choke%i", msd.directory, Q_irand(1, msd.numChokeSounds)));
			break;
		case CST_BLIND:
			//These are not native to JA so they get special handling
			if(msd.numBlindSounds > 0)
			{
				soundHandle = trap_S_RegisterSound(va("%s/blind%i", msd.directory, Q_irand(1, msd.numBlindSounds)));
			}
			else
			{
				//Play *pushed*.wav
				CG_TryPlayCustomSound( NULL, clientNum, CHAN_VOICE, va("*pushed%i.wav", Q_irand(1, 3)) );
				return;
			}
			break;
		default:
			return;
	}
	trap_S_StartSound(origin, clientNum, CHAN_VOICE, soundHandle);
}