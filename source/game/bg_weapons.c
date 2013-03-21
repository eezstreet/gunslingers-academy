// Copyright (C) 2001-2002 Raven Software
//
// bg_weapons.c -- part of bg_pmove functionality

#include "q_shared.h"
#include "bg_public.h"
#include "bg_local.h"

// Muzzle point table...
vec3_t WP_MuzzlePoint[WP_NUM_WEAPONS] = 
{//	Fwd,	right,	up.
	{0,		0,		0	},	// WP_NONE,
	{0	,	8,		0	},	// WP_STUN_BATON,
	{0	,	8,		0	},	// WP_MELEE,
	{8	,	16,		0	},	// WP_SABER,				 
	{12,	6,		-6	},	// WP_BRYAR_PISTOL,
	{12,	6,		-6	},	// WP_BLASTER,
	{12,	6,		-6	},	// WP_DISRUPTOR,
	{12,	2,		-6	},	// WP_BOWCASTER,
	{12,	4.5,	-6	},	// WP_REPEATER,
	{12,	6,		-6	},	// WP_DEMP2,
	{12,	6,		-6	},	// WP_FLECHETTE,
	{12,	8,		-4	},	// WP_ROCKET_LAUNCHER,
	{12,	0,		-4	},	// WP_THERMAL,
	{12,	0,		-10	},	// WP_TRIP_MINE,
	{12,	0,		-4	},	// WP_DET_PACK,
	{12,	6,		-6	},	// WP_CONCUSSION
	{12,	6,		-6	},	// WP_BRYAR_OLD,
	{12,	6,		-6	},	// WP_AUXILIARY1
};

#ifdef AKIMBO
// Akimbo muzzle point table
// Credit: Raz0r
vec3_t WP_MuzzlePoint2[WP_NUM_WEAPONS] =
{//	Fwd,	right,	up.
	{0,		0,		0	},	// WP_NONE,
	{12,	-6		-6	},	// WP_STUN_BATON,
	{0	,	8,		0	},	// WP_MELEE,
	{8	,	16,		0	},	// WP_SABER,				
	{12,	-6,		-6	},	// WP_PISTOL,
	{12,	-6,		-6	},	// WP_BLASTER,
	{12,	-6,		-6	},	// WP_DISRUPTOR,
	{12,	-8,		-6	},	// WP_BOWCASTER,
	{12,	-6.5,	-6	},	// WP_REPEATER,
	{12,	-7,		-6	},	// WP_DEMP2,
	{12,	-8,		-6	},	// WP_FLECHETTE,
	{12,	-8,		-4	},	// WP_ROCKET_LAUNCHER,
	{12,	-5.5,	-4	},	// WP_THERMAL,
	{12,	0,		-10	},	// WP_TRIP_MINE,
	{12,	0,		-4	},	// WP_DET_PACK,
	{12,	-6,		-6	},	// WP_CONCUSSION
	{12,	-6,		-6	},	// WP_BRYAR_OLD,
};
#endif

weaponData_t weaponData[WP_NUM_WEAPONS];
weaponData_t copyArray[256];

ammoData_t ammoData[AMMO_MAX];

int PMW_GetFiringMode(int weaponNum, int firingMode, qboolean alt){
	//PMW_GetFiringMode retrieves the index of a firing mode
	weaponData_t weapon;
	int i;
	weapon = weaponData[weaponNum];

	for(i = 0; i < MAX_FIRINGMODES; i++){
		if(alt){
			if(weapon.altFireModes[i] == firingMode)
				return i;
		} else{
			if(weapon.fireModes[i] == firingMode)
				return i;
		}
	}
	return -1;
}

#define WEAPON_TXT_FILE_SIZE	262142
#define AMMO_TXT_FILE_SIZE		1024

void ParseVectorTwoField(vec2_t *field, char *buffer)
{
	//Read until a comma for the first value
	int i = 0;
	int j;
	char bufferField[16];
	for(i = 0; buffer[i] != ',' && buffer[i] != '\0' && buffer[i] != '\t' && buffer[i] != '\n' && buffer[i] != '\r'; i++)
	{
		bufferField[i] = buffer[i];
	}
	bufferField[i] = '\0';

	(*(field))[0] = atof(bufferField);

	bufferField[0] = '\0';
	i++;
	//Read until tab or newline or exit
	for(j = 0; buffer[i] != ',' && buffer[i] != '\0' && buffer[i] != '\t' && buffer[i] != '\n' && buffer[i] != '\r'; i++)
	{
		bufferField[j] = buffer[i];
		j++;
	}
	bufferField[j] = '\0';

	(*(field))[1] = atof(bufferField);
}

#if MAC_PORT //not really sure what i'm doing, but the version with all the ##s didn't seem to make sense -redsaurus
#define READVALUE(a,b,c,d,e)	if(DB_RetrieveValue(database, a, b, buffer, c)) d[b].e = atoi(buffer);
#define READSTRING(a,b,c,d,e)	if(DB_RetrieveValue(database, a, b, buffer, c)) strncpy(d[b].e, buffer, c);
#define READCSTRING(a,b,c,d,e)	if(DB_RetrieveValue(database, a, b, buffer, c)) strncpy((char *)d[b].e, buffer, c);
#define READSHORT(a,b,c,d,e)	if(DB_RetrieveValue(database, a, b, buffer, c)) d[b].e = (short)atoi(buffer);
#define READUSHORT(a,b,c,d,e)	if(DB_RetrieveValue(database, a, b, buffer, c)) d[b].e = (unsigned short)atoi(buffer);
#define READFLOAT(a,b,c,d,e)	if(DB_RetrieveValue(database, a, b, buffer, c)) d[b].e = atof(buffer);
#define READUINT(a,b,c,d,e)		if(DB_RetrieveValue(database, a, b, buffer, c)) d[b].e = (unsigned int)atoi(buffer);
#define READQBOOL(a,b,c,d,e)	if(DB_RetrieveValue(database, a, b, buffer, c)) d[b].e = (qboolean)atoi(buffer);
#define READVEC2(a,b,c,d,e)		if(DB_RetrieveValue(database, a, b, buffer, c)) ParseVectorTwoField(&d[b].e, buffer);
#else
#define READVALUE(a,b,c,d,e)	if(DB_RetrieveValue(database, ##a##, ##b##, buffer, ##c##)) ##d##[##b##].##e## = atoi(buffer);
#define READSTRING(a,b,c,d,e)	if(DB_RetrieveValue(database, ##a##, ##b##, buffer, ##c##)) strncpy(##d##[##b##].##e##, buffer, ##c##);
#define READCSTRING(a,b,c,d,e)	if(DB_RetrieveValue(database, ##a##, ##b##, buffer, ##c##)) strncpy((char *)##d##[##b##].##e##, buffer, ##c##);
#define READSHORT(a,b,c,d,e)	if(DB_RetrieveValue(database, ##a##, ##b##, buffer, ##c##)) ##d##[##b##].##e## = (short)atoi(buffer);
#define READUSHORT(a,b,c,d,e)	if(DB_RetrieveValue(database, ##a##, ##b##, buffer, ##c##)) ##d##[##b##].##e## = (unsigned short)atoi(buffer);
#define READFLOAT(a,b,c,d,e)	if(DB_RetrieveValue(database, ##a##, ##b##, buffer, ##c##)) ##d##[##b##].##e## = atof(buffer);
#define READUINT(a,b,c,d,e)		if(DB_RetrieveValue(database, ##a##, ##b##, buffer, ##c##)) ##d##[##b##].##e## = (unsigned int)atoi(buffer);
#define READQBOOL(a,b,c,d,e)	if(DB_RetrieveValue(database, ##a##, ##b##, buffer, ##c##)) ##d##[##b##].##e## = (qboolean)atoi(buffer);
#define READVEC2(a,b,c,d,e)		if(DB_RetrieveValue(database, ##a##, ##b##, buffer, ##c##)) ParseVectorTwoField(&##d##[##b##].##e##, buffer);
#endif

#ifdef QAGAME
#define QAGAME_OR_UI
#elif defined UI
#define QAGAME_OR_UI
#endif

#ifdef UI
#define CGAME_OR_UI
#elif defined CGAME
#define CGAME_OR_UI
#endif

extern internalDatabase_t *DB_OpenFile(const char * const fileName);
extern int DB_RetrieveValue(internalDatabase_t *database, int column, int row, char *buffer, int bufferLen);
extern void DB_CloseDatabase(internalDatabase_t *database);
void GSA_DefineFireFunction(weaponData_t *wepDat, qboolean altFire, const char *comparison);
void GSA_OpenWeaponsTXT(weaponData_t *weaponsArray, const char *path);
#ifdef QAGAME
extern unsigned int primaryWeaponList[MAX_WEAPONS];
extern unsigned int sidearmList[MAX_WEAPONS];
extern unsigned int explosiveList[MAX_WEAPONS];
extern unsigned int numPrimaryWeapons;
extern unsigned int numSidearms;
extern unsigned int numExplosives;
#endif
void GSA_LoadWeaponTXT(const char *directory, char *mapName, weaponData_t *weaponsArray)
{
	//This ought to show MB2 what's up
	int i, j;
	int second = T_second();
	int secondTime, timeDif;
	char buffer[WEAPON_TXT_FILE_SIZE];
	char filePath[MAX_QPATH];
	fileHandle_t f;
#ifdef QAGAME_OR_UI
	int len = trap_FS_FOpenFile(va("%s/%s.txt", directory, mapName), &f, FS_READ);
	internalDatabase_t *database;
#ifdef QAGAME
	memset(primaryWeaponList, 0, sizeof(primaryWeaponList));
	memset(sidearmList, 0, sizeof(sidearmList));
	memset(explosiveList, 0, sizeof(explosiveList));
	numPrimaryWeapons = 0;
	numSidearms = 0;
	numExplosives = 0;
#endif
#else
	//Extra code is needed in CGAME to strip the .bsp extension
	internalDatabase_t *database;
	int len;
	char fixedPath[MAX_QPATH];
	char *path = (char *)malloc(MAX_QPATH);

	COM_StripExtension(mapName, path);

	strncpy(fixedPath, va("%s/%s.txt", directory, path+5), MAX_QPATH); //Remove /maps/ in front of string

	len = trap_FS_FOpenFile(fixedPath, &f, FS_READ);
	free(path);
#endif
	if(!f || !len)
	{
		if(!len)
			trap_FS_FCloseFile(f);
		GSA_OpenWeaponsTXT(copyArray, "ext_data/weapons.txt");
		for(i = 0; i < WP_NUM_WEAPONS; i++)
		{
			weaponData[i] = copyArray[i];
#ifdef QAGAME
			if(weaponData[i].weaponType == WT_PRIMARY)
			{
				primaryWeaponList[numPrimaryWeapons++] = i;
			}
			else if(weaponData[i].weaponType == WT_SIDEARM)
			{
				sidearmList[numSidearms++] = i;
			}
			else if(weaponData[i].weaponType == WT_EXPLOSIVE)
			{
				explosiveList[numExplosives++] = i;
			}
#endif
		}
		return;
	}
	else{
#ifdef QAGAME_OR_UI
		database = DB_OpenFile(va("%s/%s.txt", directory, mapName));
#else
		database = DB_OpenFile(fixedPath);
#endif

	}
	if(!DB_RetrieveValue(database, 1, 0, filePath, MAX_QPATH)) 
		strcpy(filePath, "ext_data/weapons.txt");

	memset(buffer, '\0', sizeof(buffer));
	GSA_OpenWeaponsTXT(copyArray, filePath);

	for( i = 1, j = 0; i < database->rows-1 && j < WP_NUM_WEAPONS; i++, j = i-1 )
	{
		DB_RetrieveValue(database, 0, i, buffer, WEAPON_TXT_FILE_SIZE);
		if(DB_RetrieveValue(database, 1, i, buffer, WEAPON_TXT_FILE_SIZE))
		{
			int number = atoi(buffer);
			weaponsArray[j] = copyArray[number];
#ifdef QAGAME
			if(weaponsArray[j].weaponType == WT_PRIMARY)
			{
				primaryWeaponList[numPrimaryWeapons++] = j;
			}
			else if(weaponsArray[j].weaponType == WT_SIDEARM)
			{
				sidearmList[numSidearms++] = j;
			}
			else if(weaponsArray[j].weaponType == WT_EXPLOSIVE)
			{
				explosiveList[numExplosives++] = j;
			}
#endif
		}
	}
	DB_CloseDatabase(database);
	secondTime = T_second();
	timeDif = secondTime - second;
	if(timeDif < 1)
	{
		Com_Printf("^2Weapon Database loaded in less than one second.\n");
	}
	else
	{
		Com_Printf("^2Weapon Database loaded in %i seconds.\n", timeDif);
	}
}

void GSA_LoadAmmoTXT(const char *fileName, ammoData_t *ammoArray)
{
	int i;
	char buffer[AMMO_TXT_FILE_SIZE];
	internalDatabase_t *database = DB_OpenFile(fileName);

	memset(buffer, '\0', sizeof(buffer));

	for(i = 0; i < database->rows-1; i++){
		READVALUE(1, i, AMMO_TXT_FILE_SIZE, ammoArray, max)
	}
	DB_CloseDatabase(database);
}

#ifdef QAGAME
#ifdef AKIMBO
extern void WP_FireRepeater( void *ent, qboolean altFire, qboolean primWeap );
extern void WP_FireStunBaton( void *ent, qboolean alt_fire, qboolean primWeap );
extern void WP_FireMelee( void *ent, qboolean alt_fire, qboolean primWeap );
extern void WP_FireDisruptor( void *ent, qboolean altFire, qboolean primWeap );
extern void WP_FireBowcaster( void *ent, qboolean altFire, qboolean primWeap );
extern void WP_FireFlechette( void *ent, qboolean altFire, qboolean primWeap );
extern void WF_LaserSight( void *ent );
extern void WF_FireJumbo( void *ent, qboolean altFire, qboolean primWeap );
#else
extern void WP_FireRepeater( void *ent, qboolean altFire );
extern void WP_FireStunBaton( void *ent, qboolean alt_fire );
extern void WP_FireMelee( void *ent, qboolean alt_fire );
extern void WP_FireDisruptor( void *ent, qboolean altFire );
extern void WP_FireBowcaster( void *ent, qboolean altFire );
extern void WP_FireFlechette( void *ent, qboolean altFire );
extern void WF_LaserSight( void *ent );
extern void WF_FireJumbo( void *ent, qboolean altFire );
#endif


void GSA_DefineFireFunction(weaponData_t *wepDat, qboolean altFire, const char *comparison)
{
	if(!Q_stricmp(comparison, "WP_REPEATER"))
	{
		if(altFire)
			wepDat->altFireFunc = WP_FireRepeater;
		else
			wepDat->fireFunc = WP_FireRepeater;
	}
	else if(!Q_stricmp(comparison, "WP_STUN_BATON"))
	{
		if(altFire)
			wepDat->altFireFunc = WP_FireStunBaton;
		else
			wepDat->fireFunc = WP_FireStunBaton;
	}
	else if(!Q_stricmp(comparison, "WP_MELEE"))
	{
		if(altFire)
			wepDat->altFireFunc = WP_FireMelee;
		else
			wepDat->fireFunc = WP_FireMelee;
	}
	else if(!Q_stricmp(comparison, "WP_DISRUPTOR"))
	{
		if(altFire)
			wepDat->altFireFunc = WP_FireDisruptor;
		else
			wepDat->fireFunc = WP_FireDisruptor;
	}
	else if(!Q_stricmp(comparison, "WP_BOWCASTER"))
	{
		if(altFire)
			wepDat->altFireFunc = WP_FireBowcaster;
		else
			wepDat->fireFunc = WP_FireBowcaster;
	}
	else if(!Q_stricmp(comparison, "WP_FLECHETTE"))
	{
		if(altFire)
			wepDat->altFireFunc = WP_FireFlechette;
		else
			wepDat->fireFunc = WP_FireFlechette;
	}
	else if(!Q_stricmp(comparison, "_lasersight"))
	{
		if(altFire)
			wepDat->altFireFunc = WF_LaserSight;
		else
			wepDat->fireFunc = WF_LaserSight;
	}
	else if(!Q_stricmp(comparison, "jumbo_hitbox"))
	{
		if(altFire)
			wepDat->altFireFunc = WF_FireJumbo;
		else
			wepDat->fireFunc = WF_FireJumbo;
	}
}
#else
void GSA_DefineFireFunction(weaponData_t *wepDat, qboolean altFire, const char *comparison)
{
}
#endif

#ifdef CGAME_OR_UI
void GSA_BuildWeaponsArrays()
{
	int i = 0;
	int j = 1, k = 1, l = 1;
	PrimaryWeapons[0] = -1;
	SidearmWeapons[0] = -1;
	Explosives[0] = -1;
	for(; i < WP_NUM_WEAPONS; i++)
	{
		switch(weaponData[i].weaponType)
		{
			case WT_PRIMARY:
				PrimaryWeapons[j] = i;
				j++;
				break;
			case WT_SIDEARM:
				SidearmWeapons[k] = i;
				k++;
				break;
			case WT_EXPLOSIVE:
				Explosives[l] = i;
				l++;
				break;
			case WT_OTHER:
			default:
				break;
		}
	}
}
#endif

int ConvertToPARM(int mode, int weapon)
{
	switch(mode)
	{
		case 0:
			switch(weapon)
			{
				case WP_BOWCASTER:
					return PARM_UMP;
				case WP_REPEATER:
					return PARM_MSIXTEEN;
				case WP_DISRUPTOR:
					return PARM_SNIPER;
				case WP_FLECHETTE:
					return PARM_SHOTGUN;
				case WP_BRYAR_OLD:
					return PARM_AK;
				case WP_CONCUSSION:
					return PARM_LMG;
				case -1:
				default:
					return 0;
			}
		case 1:
			switch(weapon)
			{
				case WP_BRYAR_PISTOL:
					return SARM_NINE;
				case WP_DEMP2:
					return SARM_REVOLVER;
				case WP_BLASTER:
					return SARM_STEYR;
				case -1:
				default:
					return 0;
			}
		case 2:
			switch(weapon)
			{
				case WP_ROCKET_LAUNCHER:
					return EARM_RPG;
				case WP_THERMAL:
					return EARM_GRENADES;
				case WP_TRIP_MINE:
					return EARM_FLASHBANG;
				case WP_DET_PACK:
					return EARM_PLASTIC_EXPLOSIVE;
				case -1:
				default:
					return 0;
			}
		default:
			return 0;
	}
}

void GSA_OpenWeaponsTXT(weaponData_t *weaponsArray, const char *path)
{
	int i, j;
	char buffer[WEAPON_TXT_FILE_SIZE];
	fileHandle_t f;
	int len = trap_FS_FOpenFile(va("%s", path), &f, FS_READ);
	internalDatabase_t *database = DB_OpenFile(path);
	if(!f || !len)
	{
		if(!len)
			trap_FS_FCloseFile(f);
		return;
	}
	else
	{
		database = DB_OpenFile(path);
		trap_FS_FCloseFile(f);
	}

	memset(buffer, '\0', sizeof(buffer));

	for( i = 0; i < database->rows-1; i++)
	{
		memset(weaponsArray[i].weaponName, 0, sizeof(weaponsArray[i].weaponName));
		memset(weaponsArray[i].internalName, 0, sizeof(weaponsArray[i].internalName));
		memset(weaponsArray[i].flashSound, 0, sizeof(weaponsArray[i].flashSound));
		memset(weaponsArray[i].altFlashSound, 0, sizeof(weaponsArray[i].altFlashSound));
		memset(weaponsArray[i].muzzleEffect, 0, sizeof(weaponsArray[i].muzzleEffect));
		memset(weaponsArray[i].altMuzzleEffect, 0, sizeof(weaponsArray[i].altMuzzleEffect));
		memset(weaponsArray[i].selectSound, 0, sizeof(weaponsArray[i].selectSound));
		memset(weaponsArray[i].missileModel, 0, sizeof(weaponsArray[i].muzzleEffect));
		READSTRING(WPLINE_NAME, i, WEAPON_TXT_FILE_SIZE, weaponsArray, weaponName)
		READSTRING(WPLINE_INTERNAL, i, WEAPON_TXT_FILE_SIZE, weaponsArray, internalName)

		READSHORT(WPLINE_ID, i, WEAPON_TXT_FILE_SIZE, weaponsArray, weaponID)
		READSHORT(WPLINE_HCIDX, i, WEAPON_TXT_FILE_SIZE, weaponsArray, hcIdx)
		READSHORT(WPLINE_VARIATION, i, WEAPON_TXT_FILE_SIZE, weaponsArray, variationIdx)

		READUINT(WPLINE_AMMOINDEX, i, WEAPON_TXT_FILE_SIZE, weaponsArray, ammoIndex)
		READUINT(WPLINE_AMMOLOW, i, WEAPON_TXT_FILE_SIZE, weaponsArray, ammoLow)
		READUINT(WPLINE_AMMOCLIPLOW, i, WEAPON_TXT_FILE_SIZE, weaponsArray, ammoClipLow)
		READUINT(WPLINE_AMMOSTART, i, WEAPON_TXT_FILE_SIZE, weaponsArray, startingAmmo)

		READUINT(WPLINE_ENERGYPERSHOT, i, WEAPON_TXT_FILE_SIZE, weaponsArray, energyPerShot)
		READUINT(WPLINE_ALTENERGYPERSHOT, i, WEAPON_TXT_FILE_SIZE, weaponsArray, altEnergyPerShot)

		//This gets handled reaaaally strangely.
		if(DB_RetrieveValue(database, WPLINE_FIREFUNC, i, buffer, WEAPON_TXT_FILE_SIZE))
		{
#ifdef QAGAME
			GSA_DefineFireFunction(&weaponsArray[i], qfalse, buffer);
#else
			READCSTRING(WPLINE_FIREFUNC, i, WEAPON_TXT_FILE_SIZE, weaponsArray, fireFunc)
#endif
		}
		if(DB_RetrieveValue(database, WPLINE_ALTFIREFUNC, i, buffer, WEAPON_TXT_FILE_SIZE))
		{
#ifdef QAGAME
			GSA_DefineFireFunction(&weaponsArray[i], qtrue, buffer);
#else
			READCSTRING(WPLINE_ALTFIREFUNC, i, WEAPON_TXT_FILE_SIZE, weaponsArray, altFireFunc)
#endif
		}
		READQBOOL(WPLINE_ALTSCOPE, i, WEAPON_TXT_FILE_SIZE, weaponsArray, altFireScope)

		READFLOAT(WPLINE_SPREAD, i, WEAPON_TXT_FILE_SIZE, weaponsArray, spread)
		READFLOAT(WPLINE_ALTSPREAD, i, WEAPON_TXT_FILE_SIZE, weaponsArray, altSpread)
		READUSHORT(WPLINE_NUMSHOTS, i, WEAPON_TXT_FILE_SIZE, weaponsArray, numShots)
		READUSHORT(WPLINE_ALTNUMSHOTS, i, WEAPON_TXT_FILE_SIZE, weaponsArray, altNumShots)

		READFLOAT(WPLINE_RECOIL, i, WEAPON_TXT_FILE_SIZE, weaponsArray, recoil)
		READFLOAT(WPLINE_ALTRECOIL, i, WEAPON_TXT_FILE_SIZE, weaponsArray, altRecoil)
		READFLOAT(WPLINE_AKIMBORECOIL, i, WEAPON_TXT_FILE_SIZE, weaponsArray, akimboRecoil)
		READFLOAT(WPLINE_HRECOIL, i, WEAPON_TXT_FILE_SIZE, weaponsArray, hRecoil)
		READFLOAT(WPLINE_ALTHRECOIL, i, WEAPON_TXT_FILE_SIZE, weaponsArray, hAltRecoil)
		READFLOAT(WPLINE_AKIMBOHRECOIL, i, WEAPON_TXT_FILE_SIZE, weaponsArray, hAkimboRecoil)

		READSHORT(WPLINE_DAM, i, WEAPON_TXT_FILE_SIZE, weaponsArray, damage)
		READSHORT(WPLINE_ALTDAM, i, WEAPON_TXT_FILE_SIZE, weaponsArray, altDamage)
		READSHORT(WPLINE_MOD, i, WEAPON_TXT_FILE_SIZE, weaponsArray, methodOfDeath)
		READSHORT(WPLINE_ALTMOD, i, WEAPON_TXT_FILE_SIZE, weaponsArray, altMethodOfDeath)
		READFLOAT(WPLINE_SPEEDMOD, i, WEAPON_TXT_FILE_SIZE, weaponsArray, speedModifier)
		READFLOAT(WPLINE_ADSSPEEDMOD, i, WEAPON_TXT_FILE_SIZE, weaponsArray, ADSspeedModifier)
		READSHORT(WPLINE_ADSFOVMOD, i, WEAPON_TXT_FILE_SIZE, weaponsArray, ADSfov)
		READUINT(WPLINE_ADSTIME, i, WEAPON_TXT_FILE_SIZE, weaponsArray, ADSTime)

		READFLOAT(WPLINE_ZOOMSTARTFOV, i, WEAPON_TXT_FILE_SIZE, weaponsArray, zoomStartFOV)
		READFLOAT(WPLINE_ZOOMENDFOV, i, WEAPON_TXT_FILE_SIZE, weaponsArray, zoomEndFOV)
		READFLOAT(WPLINE_ZOOMSPEED, i, WEAPON_TXT_FILE_SIZE, weaponsArray, zoomSpeed)
		READUSHORT(WPLINE_ZOOMTYPE, i, WEAPON_TXT_FILE_SIZE, weaponsArray, zoomType)
		
		READUSHORT(WPLINE_WTYPE, i, WEAPON_TXT_FILE_SIZE, weaponsArray, weaponType)
		READQBOOL(WPLINE_ALWAYSSEMI, i, WEAPON_TXT_FILE_SIZE, weaponsArray, alwaysSemi)
		READQBOOL(WPLINE_ALTALWAYSSEMI, i, WEAPON_TXT_FILE_SIZE, weaponsArray, altAlwaysSemi)
		READUSHORT(WPLINE_NUMFIREMODES, i, WEAPON_TXT_FILE_SIZE, weaponsArray, numFireModes)
		for(j = 0; j < MAX_FIRINGMODES; j++)
		{
			READSHORT(WPLINE_FMODE1+j, i, WEAPON_TXT_FILE_SIZE, weaponsArray, fireModes[j])
			READSHORT(WPLINE_AFMODE1+j, i, WEAPON_TXT_FILE_SIZE, weaponsArray, altFireModes[j])
			//READUINT(WPLINE_FTFMODE1+j, i, WEAPON_TXT_FILE_SIZE, weaponsArray, fireTime[j])
			//READUINT(WPLINE_AFTFMODE1+j, i, WEAPON_TXT_FILE_SIZE, weaponsArray, altFireTime[j])
			if(DB_RetrieveValue(database, WPLINE_FTFMODE1+j, i, buffer, WEAPON_TXT_FILE_SIZE)) weaponsArray[i].fireTime[j] = (unsigned int)(1000 /(atof(buffer) / 60));
			if(DB_RetrieveValue(database, WPLINE_FTFMODE1+j, i, buffer, WEAPON_TXT_FILE_SIZE)) weaponsArray[i].altFireTime[j] = (unsigned int)(1000 /(atof(buffer) / 60));
			READUSHORT(WPLINE_ADRAINFM1+j, i, WEAPON_TXT_FILE_SIZE, weaponsArray, accuracyDrain[j])
			READUSHORT(WPLINE_FDELAY1+j, i, WEAPON_TXT_FILE_SIZE, weaponsArray, fireModeDelays[j])
		}
		READQBOOL(WPLINE_DFMODE, i, WEAPON_TXT_FILE_SIZE, weaponsArray, fireModeDisplay)
		READFLOAT(WPLINE_ATHRESHOLD, i, WEAPON_TXT_FILE_SIZE, weaponsArray, accuracyThreshold)
		READUSHORT(WPLINE_CLIPSIZE, i, WEAPON_TXT_FILE_SIZE, weaponsArray, clipSize)
		READUSHORT(WPLINE_RTIME, i, WEAPON_TXT_FILE_SIZE, weaponsArray, reloadTime)
		READUSHORT(WPLINE_RTYPE, i, WEAPON_TXT_FILE_SIZE, weaponsArray, reloadType)
		READQBOOL(WPLINE_AUTORELOAD, i, WEAPON_TXT_FILE_SIZE, weaponsArray, autoReload)

		//READINT(WPLINE_BULLETSPEED, i, WEAPON_TXT_FILE_SIZE, weaponsArray, bulletSpeed)
		if(DB_RetrieveValue(database, WPLINE_BULLETSPEED, i, buffer, WEAPON_TXT_FILE_SIZE))
		{
			weaponsArray[i].bulletSpeed = (unsigned int)atoi(buffer);
		}
		READUINT(WPLINE_ALTBULLETSPEED, i, WEAPON_TXT_FILE_SIZE, weaponsArray, altBulletSpeed)

		for(j = 0; j < NUM_LIMBS; j++)
		{
			READFLOAT(WPLINE_HMOD+j, i, WEAPON_TXT_FILE_SIZE, weaponsArray, Mods[j])
			READFLOAT(WPLINE_AHMOD+j, i, WEAPON_TXT_FILE_SIZE, weaponsArray, AltMods[j])
		}

		READCSTRING(WPLINE_FLASHSOUND, i, WEAPON_TXT_FILE_SIZE, weaponsArray, flashSound)
		READCSTRING(WPLINE_ALTFLASHSOUND, i, WEAPON_TXT_FILE_SIZE, weaponsArray, altFlashSound)
		READCSTRING(WPLINE_MUZZLEEFFECT, i, WEAPON_TXT_FILE_SIZE, weaponsArray, muzzleEffect)
		READCSTRING(WPLINE_ALTMUZZLEEFFECT, i, WEAPON_TXT_FILE_SIZE, weaponsArray, altMuzzleEffect)
		READCSTRING(WPLINE_SELECTSOUND, i, WEAPON_TXT_FILE_SIZE, weaponsArray, selectSound)

		READUSHORT(WPLINE_SWAPICONL, i, WEAPON_TXT_FILE_SIZE, weaponsArray, weaponSwapL)
		READUSHORT(WPLINE_SWAPICONW, i, WEAPON_TXT_FILE_SIZE, weaponsArray, weaponSwapW)

		READCSTRING(WPLINE_MISSILEMODEL, i, WEAPON_TXT_FILE_SIZE, weaponsArray, missileModel)
		READCSTRING(WPLINE_MODELFOLDER, i, WEAPON_TXT_FILE_SIZE, weaponsArray, modelFolder)
		READUINT(WPLINE_NUMBARRELS, i, WEAPON_TXT_FILE_SIZE, weaponsArray, numBarrels)
		READCSTRING(WPLINE_MD3NAME, i, WEAPON_TXT_FILE_SIZE, weaponsArray, md3Name)
		READCSTRING(WPLINE_GHOUL2, i, WEAPON_TXT_FILE_SIZE, weaponsArray, ghoul2Name)
		READCSTRING(WPLINE_READYANIM, i, WEAPON_TXT_FILE_SIZE, weaponsArray, readyAnim)
		READCSTRING(WPLINE_LEGSREADYANIM, i, WEAPON_TXT_FILE_SIZE, weaponsArray, legsReadyAnim)
		READCSTRING(WPLINE_ATTACKANIM, i, WEAPON_TXT_FILE_SIZE, weaponsArray, attackAnim)
		READCSTRING(WPLINE_AKIMBOANIM, i, WEAPON_TXT_FILE_SIZE, weaponsArray, akimboAttackAnim)
		READUSHORT(WPLINE_ATTACKANIMMS, i, WEAPON_TXT_FILE_SIZE, weaponsArray, attackAnimMS)
		READUSHORT(WPLINE_AKIMBOATTACKANIMMS, i, WEAPON_TXT_FILE_SIZE, weaponsArray, akimboAttackAnimMS)

		READQBOOL(WPLINE_LIMBDISMEMBER, i, WEAPON_TXT_FILE_SIZE, weaponsArray, limbDismember)

		READFLOAT(WPLINE_BOBPITCH, i, WEAPON_TXT_FILE_SIZE, weaponsArray, bobbing[PITCH])
		READFLOAT(WPLINE_BOBYAW, i, WEAPON_TXT_FILE_SIZE, weaponsArray, bobbing[YAW])
		READFLOAT(WPLINE_BOBROLL, i, WEAPON_TXT_FILE_SIZE, weaponsArray, bobbing[ROLL])

		READFLOAT(WPLINE_SWAYPITCH, i, WEAPON_TXT_FILE_SIZE, weaponsArray, sway[PITCH])
		READFLOAT(WPLINE_SWAYYAW, i, WEAPON_TXT_FILE_SIZE, weaponsArray, sway[YAW])
		READFLOAT(WPLINE_SWAYROLL, i, WEAPON_TXT_FILE_SIZE, weaponsArray, sway[ROLL])
		READFLOAT(WPLINE_SWAYTIMESCALE, i, WEAPON_TXT_FILE_SIZE, weaponsArray, swayTimeScale)
		READUINT(WPLINE_SWAYFREQUENCY, i, WEAPON_TXT_FILE_SIZE, weaponsArray, swayFrequency)
		READFLOAT(WPLINE_BOBCROUCHMOD, i, WEAPON_TXT_FILE_SIZE, weaponsArray, bobCrouchModifier)

		READFLOAT(WPLINE_BOBX, i, WEAPON_TXT_FILE_SIZE, weaponsArray, bobbingOrigin[PITCH])
		READFLOAT(WPLINE_BOBY, i, WEAPON_TXT_FILE_SIZE, weaponsArray, bobbingOrigin[YAW])
		READFLOAT(WPLINE_BOBZ, i, WEAPON_TXT_FILE_SIZE, weaponsArray, bobbingOrigin[ROLL])
		READFLOAT(WPLINE_SWAYX, i, WEAPON_TXT_FILE_SIZE, weaponsArray, swayOrigin[PITCH])
		READFLOAT(WPLINE_SWAYY, i, WEAPON_TXT_FILE_SIZE, weaponsArray, swayOrigin[YAW])
		READFLOAT(WPLINE_SWAYZ, i, WEAPON_TXT_FILE_SIZE, weaponsArray, swayOrigin[ROLL])

		READVEC2(WPLINE_SCREENSHAKE, i, WEAPON_TXT_FILE_SIZE, weaponsArray, screenShake)
		READVEC2(WPLINE_ALTSCREENSHAKE, i, WEAPON_TXT_FILE_SIZE, weaponsArray, altScreenShake)
		READVEC2(WPLINE_ADSSCREENSHAKE, i, WEAPON_TXT_FILE_SIZE, weaponsArray, ADSscreenShake)
		READVEC2(WPLINE_ALTADSSCREENSHAKE, i, WEAPON_TXT_FILE_SIZE, weaponsArray, ADSaltScreenShake)
		READVEC2(WPLINE_AKIMBOSCREENSHAKE, i, WEAPON_TXT_FILE_SIZE, weaponsArray, akimboScreenShake)

		READQBOOL(WPLINE_CANROLL, i, WEAPON_TXT_FILE_SIZE, weaponsArray, canRoll)
		READQBOOL(WPLINE_CANJUMP, i, WEAPON_TXT_FILE_SIZE, weaponsArray, canJump)
		READQBOOL(WPLINE_CANSPRINT, i, WEAPON_TXT_FILE_SIZE, weaponsArray, canSprint)
		READQBOOL(WPLINE_CANSHOOTWHILEROLLING, i, WEAPON_TXT_FILE_SIZE, weaponsArray, canShootWhileRolling)
		READQBOOL(WPLINE_AKIMBO, i, WEAPON_TXT_FILE_SIZE, weaponsArray, akimbo)

		READCSTRING(WPLINE_ICONPATH, i, WEAPON_TXT_FILE_SIZE, weaponsArray, iconPath)
		READCSTRING(WPLINE_SWAPICONPATH, i, WEAPON_TXT_FILE_SIZE, weaponsArray, swapIconPath)

		//lerp lerp lerp yer bote, jently down zee ztream...
		READFLOAT(WPLINE_XLERPORIGIN, i, WEAPON_TXT_FILE_SIZE, weaponsArray, lerpOrigin[0])
		READFLOAT(WPLINE_YLERPORIGIN, i, WEAPON_TXT_FILE_SIZE, weaponsArray, lerpOrigin[1])
		READFLOAT(WPLINE_ZLERPORIGIN, i, WEAPON_TXT_FILE_SIZE, weaponsArray, lerpOrigin[2])
		READFLOAT(WPLINE_PITCHLERPANGLES, i, WEAPON_TXT_FILE_SIZE, weaponsArray, lerpAngles[0])
		READFLOAT(WPLINE_YAWLERPANGLES, i, WEAPON_TXT_FILE_SIZE, weaponsArray, lerpAngles[1])
		READFLOAT(WPLINE_ROLLLERPANGLES, i, WEAPON_TXT_FILE_SIZE, weaponsArray, lerpAngles[2])

		READFLOAT(WPLINE_XNUDGE, i, WEAPON_TXT_FILE_SIZE, weaponsArray, nudge[0])
		READFLOAT(WPLINE_YNUDGE, i, WEAPON_TXT_FILE_SIZE, weaponsArray, nudge[1])
		READFLOAT(WPLINE_ZNUDGE, i, WEAPON_TXT_FILE_SIZE, weaponsArray, nudge[2])
		READFLOAT(WPLINE_ADSXNUDGE, i, WEAPON_TXT_FILE_SIZE, weaponsArray, ADSnudge[0])
		READFLOAT(WPLINE_ADSYNUDGE, i, WEAPON_TXT_FILE_SIZE, weaponsArray, ADSnudge[1])
		READFLOAT(WPLINE_ADSZNUDGE, i, WEAPON_TXT_FILE_SIZE, weaponsArray, ADSnudge[2])
		READFLOAT(WPLINE_AKIMBOXNUDGE, i, WEAPON_TXT_FILE_SIZE, weaponsArray, akimboNudge[0])
		READFLOAT(WPLINE_AKIMBOYNUDGE, i, WEAPON_TXT_FILE_SIZE, weaponsArray, akimboNudge[1])
		READFLOAT(WPLINE_AKIMBOZNUDGE, i, WEAPON_TXT_FILE_SIZE, weaponsArray, akimboNudge[2])

		READUSHORT(WPLINE_XBOXRUMBLEINTENSITY, i, WEAPON_TXT_FILE_SIZE, weaponsArray, xboxRumbleIntensity)
		READUSHORT(WPLINE_XBOXRUMBLELENGTH, i, WEAPON_TXT_FILE_SIZE, weaponsArray, xboxRumbleDuration)
	}
	DB_CloseDatabase(database);
}