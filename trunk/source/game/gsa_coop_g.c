#include "gsa_coop_g.h"
#include "g_local.h"

void GSA_InitFriendlyNPCs(void)
{
	int i;
	for(i = 0; i < MAX_FRIENDLY_NPCS; i++)
	{
		fNPCsNames[i] = -1;
		trap_SendServerCommand(-1, va("fnpc %i -1", i));
		trap_SetConfigstring(CS_NPCS + i, "NULL");
	}
}

void GSA_FriendlyNPCRegister(gentity_t *npc)
{
	int i;
	qboolean found = qfalse;

	if(!Q_stricmp(npc->fullName, "NULL_NAME"))
		return; //No name = no purpose.

	for(i = 0; i < MAX_FRIENDLY_NPCS; i++)
	{ //Find us a slot that isn't occupied.
		if(fNPCsNames[i] < 0){
			found = qtrue;
			break;
		}
	}

	if(!found){
		//Damn. No slots available.
#ifdef _DEBUG
		trap_SendServerCommand(-1, va("print \"%s : No hoverover name.\n\"", npc->fullName));
#endif
		return;
	}

	fNPCsNames[i] = npc->s.number;
	trap_SendServerCommand(-1, va("fnpc %i %i", i, npc->s.number));
	trap_SetConfigstring(CS_NPCS+i, npc->fullName);
}

void GSA_FriendlyNPCDeregister(gentity_t *npc)
{
	int i;
	qboolean found = qfalse;

	//Find our slot.
	for(i = 0; i < MAX_FRIENDLY_NPCS; i++){
		if(fNPCsNames[i] == npc->client->ps.clientNum){
			found = qtrue;
			break;
		}
	}
	if(!found)
		return;

	fNPCsNames[i] = -1;
	trap_SendServerCommand(-1, va("fnpc %i -1", i));
	trap_SetConfigstring(CS_NPCS + i, "NULL");
}