#ifndef _MEANSOFDEATH_H
#define _MEANSOFDEATH_H

#include "q_shared.h"
#include "db_simple.h"

typedef enum 
{
	MODD_INDEX,
	MODD_INTERNAL,
	MODD_DEATH,
	MODD_SUICIDE,
	MODD_SUICIDE_FEMALE,
	MODD_SUICIDE_NEUTER,
	MODD_HEADSHOT,
	MODD_HS_LINE,
	MODD_BLOODSPRAY,
	MODD_BLOODSPLASHTIME,
	MODD_BLOODSPLASHRADIUS,
	MODD_USECHOKESOUND,
	MODD_EOL
} modDataLines_t;

typedef struct modData_s
{
	int index;
	char internalName[64];
	char deathMessage[128];
	char suicideString[128];
	char femaleSuicideString[128];
	char neuterSuicideString[128];
	signed char headShot; //whether this MOD_ can get a headshot or not
	unsigned char headShotLine;
	qboolean bloodSpray;			//sprays blood all over nearby clients' screen if true
	unsigned int bloodSplashTime;	//sprays blood all over nearby clients' screen for this much time
	unsigned int bloodSplashRadius;	//radius for blood spray
	qboolean useChokeSound;			//If true, uses choke sound when damaging
} modData_t;

void GSA_LoadMODTXT(const char *directory, const char *fileName, modData_t *modDataArray);

#define MAX_MEANSOFDEATH 256
extern modData_t modData[MAX_MEANSOFDEATH];

#endif
