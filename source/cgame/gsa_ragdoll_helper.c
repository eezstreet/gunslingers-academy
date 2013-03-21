#include "cg_local.h"

void RD_KickBonesAtOrigin(void *ghoul2, vec3_t origin, vec3_t angles, vec3_t velocity,  int time, int basePose)
{
	//sharedSetBoneIKStateParams_t ikP;
	if(!ghoul2)
		return; //No GHOUL2 instances associated

	//Restrict the jointing

}