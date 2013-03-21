//Gunslinger's Academy - Pregame Lobby Code
//Code (c) 2011 eezstreet
//Based on Raven's Intermission System

#include "g_local.h"

void MoveClientFromIntermission (gclient_t *client){
	ClientBegin(client->ps.clientNum, qtrue);
}

void LobbyServerCommand( gentity_t *ent ){
	char string[1400];
	char entry[1024];
	int numSorted = level.numConnectedClients, stringLength = 0, i=0, ping, j;
	gclient_t *client;

	string[0] = 0;

	CAP(numSorted, MAX_CLIENT_SCORE_SEND);

	for(i; i < numSorted; i++) {
		client = &level.clients[level.sortedClients[i]];
		if( client->pers.connected != CON_CONNECTED ) { 
			ping = -1;
		} else if( g_entities[level.sortedClients[i]].r.svFlags & SVF_BOT ) { 
			ping = Q_irand(50, 150); //Fake ping for bots, OJP edit.
		} else {
			ping = client->ps.ping < 999 ? client->ps.ping : 999;
		}

		Com_sprintf(entry, sizeof(entry),
			" %i %i", level.sortedClients[i], ping);

		j = strlen(entry);
		if(stringLength + j > 1022)
			break;
		strcpy(string + stringLength, entry);
		stringLength += j;
	}

	i = level.numConnectedClients;

	trap_SendServerCommand(ent-g_entities, va("lobby %i %i %i %s", i, level.lobbyStartTime, level.lobbyEndTime, string));
}

void SendLobbyMessageToAllClients ( void ) {
	int i;

	for( i = 0; i < level.maxclients; i++ ) {
		if(level.clients[i].pers.connected == CON_CONNECTED ) {
			LobbyServerCommand( g_entities + i );
		}
	}
}

void CheckLobbyExit( void )
{ //Check Lobby Exit conditions
	int		ready, notReady, readyMask;
	int		i;
	gclient_t	*client;

	//Initialize all variables to 0 (so we're not screwing with a random hunk of memory!)
	ready = 0;
	notReady = 0;
	readyMask = 0;

	if(level.lobbyDone)
		return; //Why bother?

	for( i = 0; i < g_maxclients.integer; i++ ) //Loop through clients.
	{ //This loop tells us how many clients are ready.
		client = level.clients + i;
		if(client->pers.connected != CON_CONNECTED){
			//We aren't connected.
			continue;
		}
		if( g_entities[client->ps.clientNum].r.svFlags & SVF_BOT )
		{
			//We're a bot. It's not like we matter at all, or have feelings. :D
			continue;
		}
		if(client->readyToExit){
			ready++; //Increment our "ready" amount.
			if( i < 16 ){
				readyMask |= 1 << i; //This mask determines STAT_CLIENTS_READY, explained later.
			}
		} else{
			notReady++; //Increment our "not ready" amount.
		}
	}

	for (i = 0 ; i < g_maxclients.integer ; i++) {		//What we do now, is copy the ready mask.
		client = level.clients + i;						//The ready mask determines who is ready.
		if ( client->pers.connected != CON_CONNECTED ) {	//This is used directly on the interface.
			continue;
		}
		client->ps.stats[STAT_CLIENTS_READY] = readyMask;
	}

	// First Check - Never exit in less than five seconds
	// Do this
	if ( level.time < level.lobbyStartTime + 5000 ) {
		return;
	}

	//Now we begin to run some checks as to who's ready and who's not.
	if((!g_pregameLobbyForceWait.integer && ready > (notReady*2)) || level.time > level.lobbyEndTime)
	{ //We have a 2:1 ratio (or better) of people ready than not ready.
		level.lobbyDone = qtrue;
		level.lobbyStartTime = 0;
		level.intermissiontime = 0;
		trap_SendServerCommand(-1, "lobbyEnd");
		for(i = 0; i < g_maxclients.integer; i++){
			client = level.clients + i;
			if(client->pers.connected == CON_CONNECTED)
				MoveClientFromIntermission (client);
		}
	}
	return;
}

void BeginLobby( void )
{
	int			i;
	gentity_t	*ent;

	if(level.lobbyInitted)
		return;
	if(!g_pregameLobby.integer)
		return;

	if(level.lobbyStartTime)
	{
		return; //Already started.
	}

	if(g_gametype.integer == GT_SIEGE || g_gametype.integer == GT_DUEL || g_gametype.integer == GT_POWERDUEL)
	{
		return; //Not supported.
	}

	level.lobbyStartTime = level.time;
	level.intermissiontime = level.time;
	level.lobbyEndTime = level.time + g_pregameLobbyTime.integer;
	FindIntermissionPoint();

	// Everybody! Go to the intermission point!
	for (i=0 ; i< level.maxclients ; i++) {
		ent = g_entities + i;
		if (!ent->inuse)
			continue;
		// respawn if dead
		if (ent->health <= 0) {
			if (g_gametype.integer != GT_POWERDUEL ||
				!ent->client ||
				ent->client->sess.sessionTeam != TEAM_SPECTATOR)
			{ //don't respawn spectators in powerduel or it will mess the line order all up
				respawn(ent);
			}
		}
		ent->client->ps.stats[STAT_CLIENTS_READY] = 0;
		ent->client->readyToExit = qfalse;
		MoveClientToIntermission( ent ); 
		//eez: Looks like I'm sending the client to intermission, eh?
		//That function just does a lot of useful stuff for us, like for
		//example, clearing the rocket lock index and other useful stuff.
	}

	// FIXME: send the proper message!
	SendLobbyMessageToAllClients();

	level.lobbyInitted = qtrue;
}

void CheckLobbyOtherStuff(void)
{
	if(!level.lobbyDone && g_pregameLobby.integer){
		BeginLobby();
	}
}

void CheckLobby(void) //CheckLobby is called each G_RunFrame()
{
	CheckLobbyOtherStuff();
	CheckLobbyExit();
}
