#include "g_local.h"

//Always send the whole text command as a parameter

void GSA_ChatCommandMe( gentity_t *ent, char *text )
{
	char *meCommand = text + 4;
	trap_SendServerCommand( -1, va("chat \"^5*^7 %s %s ^5*^7\"", ent->client->pers.netname, meCommand) );
}

void GSA_ChatCommandRoll ( gentity_t *ent, char *text )
{
	char *rollCommand = text + 5;
	int rollNum = atoi(rollCommand);

	if(rollNum > 32000)
	{
		trap_SendServerCommand( ent->s.number, "chat \"You cannot use that high of a maximum.\"" );
		return;
	}
	if(!rollNum)
	{
		trap_SendServerCommand( -1, va("chat \"^3* ^7 %s rolled a %i ^3*^7\"", ent->client->pers.netname, Q_irand(1, 6)));
	}
	else
	{
		trap_SendServerCommand( -1, va("chat \"^3* ^7 %s rolled a %i ^3*^7\"", ent->client->pers.netname, Q_irand(1, rollNum)));
	}
}

//=======================================
// RUSSIAN ROULETTE
//=======================================

//Init the Russian Roulette minigame
void GSA_InitRussianRoulette ( void )
{
	rRGame.currentChamber = 0;
	rRGame.gameActive = qfalse;
	rRGame.gameOwner = -1;
	rRGame.numChambers = 0;
	memset(&rRGame, 0, MAX_CLIENTS);
}

//Is this client an owner of a Russian Roulette game in progress?
qboolean GSA_IsClientRouletteOwner ( int clientNum )
{
	if(clientNum > MAX_CLIENTS || clientNum < 0)
	{
		//...probably not.
		return qfalse;
	}

	if(!rRGame.gameActive)
	{
		//No game is active, so no.
		return qfalse;
	}

	if(rRGame.gameOwner != clientNum)
	{
		return qfalse;
	}
	return qtrue;
}

void GSA_EchoRouletteMessage ( char *text )
{
	//Echo a roulette-related message to spectators
	int i;
	for(i = 0; i < level.numConnectedClients; i++)
	{
		if(level.clients[i].sess.sessionTeam == TEAM_SPECTATOR)
		{
			trap_SendServerCommand(level.clients[i].ps.clientNum, va("chat \"%s\"", text));
		}
	}
}

void GSA_EndRouletteGame( char *winningText )
{
	trap_SendServerCommand(-1, "chat \"^3The game of Russian Roulette has reached a conclusion.\"");
}

void GSA_CheckRouletteDisconnect ( gentity_t *ent )
{
	//If a client disconnects, then forget it.
	if(rRGame.gameOwner == ent->client->ps.clientNum)
	{
		//We're the owner. Say game over.
		GSA_EchoRouletteMessage("^3Game over. The host has left.");
	}
}

void GSA_ChatCommandRoulette ( gentity_t *ent, char *text )
{
	if(ent->client->sess.sessionTeam != TEAM_SPECTATOR)
	{
		trap_SendServerCommand( ent->client->ps.clientNum, "chat \"^1You can only use /roulette while in spectator.\"");
		return;
	}
	if(rRGame.gameActive && rRGame.gameOwner >= 0)
	{
		trap_SendServerCommand( ent->client->ps.clientNum, "chat \"^1There is already a game active.\"");
		return;
	}
}