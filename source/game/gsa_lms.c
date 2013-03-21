#include "g_local.h"

qboolean winning = qfalse;
void CheckExitRules( void );

qboolean GSA_IsLMSValid(void)
{
	if(g_LMS.integer && g_gametype.integer != GT_DUEL && g_gametype.integer != GT_POWERDUEL && g_gametype.integer != GT_SIEGE && g_gametype.integer != GT_SINGLE_PLAYER)
	{
		return qtrue;
	}
	return qfalse;
}

void GSA_DisplayLifeCount(gentity_t *ent)
{
	if(!ent->client)
		return;
	trap_SendServerCommand(ent->client->ps.clientNum, va("print \"You have %i lives left\n\"", ent->client->pers.livesLeft));
}

//Set up life count
void GSA_SetupLMS (gentity_t *ent)
{
	ent->client->pers.livesLeft = g_LMSlives.integer;
	ent->client->pers.livesUsed = 1;
	winning = qfalse;
	level.warmupDebounce = level.time + 10000;
	SetTeam(ent, "f");
}

//Reset the LMS
void GSA_ResetLMS(void)
{
	int i;
	for(i = 0; i < level.numConnectedClients; i++)
	{
		level.clients[i].pers.livesLeft = g_LMSlives.integer;
		level.clients[i].pers.livesUsed = 0;
	}
}

//Deduct our lives
void GSA_LMSDeductLife (gentity_t *ent)
{
	if(level.warmupTime)
	{
		return;
	}
	if(!ent->client->pers.livesUsed)
	{
		//Not init'd. Let's go.
		GSA_SetupLMS(ent);
		return;
	}
	if(!level.warmupTime && GSA_IsLMSValid())
	{
		ent->client->pers.livesLeft--;
		ent->client->pers.livesUsed++;
		GSA_DisplayLifeCount(ent);
		ClientSpawn(ent);
	}
}

//Checks whether we have the proper life count and sets us into spectator if necessary, or respawns us
void GSA_LMSLiveCheck (gentity_t *ent)
{
	if(!GSA_IsLMSValid())
	{
		ClientSpawn(ent);
		return;
	}
	if( level.warmupTime )
	{ //In pre-match warmup
		ent->client->pers.livesUsed = 0;
		ClientSpawn(ent);
		level.warmupDebounce = 0;
		return;
	}
	if(ent->client->pers.livesLeft <= 0 && ent->client->pers.livesUsed > 0)
	{
		StopFollowing(ent);
		ent->client->sess.spectatorClient = 0;
		ent->client->pers.teamState.state = TEAM_BEGIN;
		Cmd_FollowCycle_f(ent, 1);
		SetTeam(ent, "spectator");
		trap_SendServerCommand(-1, va("send_event \"%s ^7is OUT!\n\"", ent->client->pers.netname));
		trap_SendServerCommand(-1, va("chat \"%s ^7is OUT!\n\"", ent->client->pers.netname));
	}
	else
	{
		GSA_LMSDeductLife(ent);
	}
}

void CheckLMSWinConditions(void)
{
	int i;
	short teamCounts[2] = {0, 0}; //Number of people on each team
	byte whoAppearsWinner; //for FFA
	short numSurviving = 0;
	if(!GSA_IsLMSValid())
	{ //Not in non-LMS
		return;
	}
	if(level.warmupTime)
	{ //Not during warmups.
		return;
	}
	if(!level.warmupDebounce || level.warmupDebounce > level.time)
	{ //Not while waiting for warmup debounce.
		return;
	}
	//First, determine whether we're in a team game or not.
	if(g_gametype.integer >= GT_TEAM)
	{ //Team game
		//Check who's on what team and if they're alive.
		for(i = 0; i < level.numPlayingClients; i++)
		{
			if(level.clients[i].sess.sessionTeam == TEAM_RED && level.clients[i].pers.livesLeft >= 0)
			{
				teamCounts[0]++;
			}
			else if(level.clients[i].sess.sessionTeam == TEAM_BLUE && level.clients[i].pers.livesLeft >= 0)
			{
				teamCounts[1]++;
			}
		}
		//OK...let's see if that count reaches zero.
		if(teamCounts[0] <= 0)
		{
			trap_SendServerCommand(-1, "cp \"^4BLUE ^3wins the match!\"");
			winning = qtrue;
			LogExit("^4BLUE ^7wins the match!");
			CheckExitRules();
			level.warmupDebounce = level.time + 5000;
		}
		else if(teamCounts[1] <= 0)
		{
			trap_SendServerCommand(-1, "cp \"^1RED ^3wins the match!\"");
			winning = qtrue;
			LogExit("^1RED ^7wins the match!");
			CheckExitRules();
			level.warmupDebounce = level.time + 5000;
		}
	}
	else
	{ //Free-for-all setup
		//Loop through all the clients, and check their lifecount.
		for(i = 0; i < level.numPlayingClients; i++)
		{
			//if(level.clients[i].sess.sessionTeam != TEAM_SPECTATOR)
			{
				whoAppearsWinner = i;
				numSurviving++;
			}
		}
		if(numSurviving == 1)
		{
			if(!winning)
			{
				trap_SendServerCommand(-1, va("cp \"%s ^7has won the match!\n\"", level.clients[whoAppearsWinner].pers.netname));
				winning = qtrue;
				LogExit(va("%s has won the match!", level.clients[whoAppearsWinner].pers.netname));
				CheckExitRules();
			}
		}
		else if(numSurviving == 0)
		{
			if(!winning)
			{
				winning = qtrue;
				LogExit("No winners. Restarting match.");
				CheckExitRules();
			}
		}
	}
}