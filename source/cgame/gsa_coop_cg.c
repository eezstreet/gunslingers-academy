#include "cg_local.h"
#include "gsa_coop_cg.h"

void GSA_CG_SetFriendlyNPC(int slot, int value)
{
	cg_fNPCsNums[slot] = value;
}

void GSA_CG_SetFriendlyNPCName(const char *string, int num)
{
	Q_strncpyz(cg_fNPCNames[num-CS_NPCS], string, MAX_QPATH);
}

qboolean GSA_CG_IsFriendlyNPC(int clientNum)
{
	int i;
	centity_t *npc = &cg_entities[clientNum];
	if(npc->currentState.eType != ET_NPC ||
		npc->currentState.NPC_class == CLASS_VEHICLE){
			//Nope.
			return qfalse;
	}
	for(i = 0; i < MAX_FRIENDLY_NPCS; i++)
	{
		if(cg_fNPCsNums[i] == clientNum)
		{
			return qtrue;
		}
	}
	return qfalse;
}

char *GSA_CG_RetrieveNPCName(int clientNum)
{
	int i;
	qboolean found = qfalse;
	if(cg_entities[clientNum].currentState.eType != ET_NPC ||
		cg_entities[clientNum].currentState.NPC_class == CLASS_VEHICLE){
			//Nope.
			return "NULL_NAME";
	}
	for(i = 0; i < MAX_FRIENDLY_NPCS; i++){
		if(cg_fNPCsNums[i] == clientNum){
			found = qtrue;
			break;
		}
	}
	if(!found)
		return "NULL_NAME";
	return cg_fNPCNames[i];
}