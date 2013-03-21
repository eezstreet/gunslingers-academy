#ifndef __KILLSTREAKS_H
#define __KILLSTREAKS_H

#include "q_shared.h"
#include "db_simple.h"

#define MAX_KILLSTREAK_AWARDS	16

#ifdef _WIN32 //un-ifdef this once there's actually something here; gcc doesn't seem to like empty enums -redsaurus
typedef enum {
} rewards_t;
#endif

typedef enum {
	KSLINE_KILLS,
	KSLINE_REWARD,
	KSLINE_DISPLAYTEXT,
	KSLINE_SOUND,
	KSLINE_EOL				//Almost forgot to put an EOL here. Massive "fffffffuuuuuuu" ensues
} rewardLines_t;

typedef enum {
	RADAR_RECON,
	RADAR_RADAR,
	RADAR_COMM_TAP,
	RADAR_COUNTER_UAV,
	RADAR_JAMMER
} radarItemTypes_t;

typedef struct ksData_s {
	unsigned int kill;
	unsigned int reward;
	unsigned int rewardVariation;
	char displayText[64];
	char soundPath[MAX_QPATH];
	unsigned int eol;
} ksData_t;

void GSA_LoadKillstreakTXT(const char *directory, const char *BSPName, ksData_t *killstreakArray);
qboolean rewardExistsForKill(int numkills, ksData_t *killstreakArray);
int rewardVariationForReward(unsigned int reward);
int rewardIndexForGiTag(unsigned int giTag);


extern ksData_t killstreakData[MAX_KILLSTREAK_AWARDS];
extern unsigned int numKS;

#endif