#include "gsa_achieve.h"

#ifdef ACHIEVEMENTS

void GSA_BasicAchieveFunc(qboolean load)
{
	if(load)
	{
		trap_S_StartLocalSound(trap_S_RegisterSound(thisAchieve->baseData->achieveSound), CHAN_LOCAL);
		CG_EVBox_AddString(va("Achievement Unlocked: %s\n%s", thisAchieve->baseData->name, thisAchieve->baseData->description));
	}
	return;
}

void GSA_BasicAchieveFunc_NS(qboolean load)
{
	if(load)
	{
		CG_EVBox_AddString(va("Achievement Unlocked: %s\n%s", thisAchieve->baseData->name, thisAchieve->baseData->description));
	}
	return;
}
#endif
