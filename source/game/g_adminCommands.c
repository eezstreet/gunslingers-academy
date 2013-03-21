#include "g_local.h"

//Helper functions

//AHelper_ResolveTeam
//Resolves a team for the text. 
static int AHelper_ResolveTeam( char *text )
{
	if(atoi(text))
	{
		return atoi(text);
	}

	if(!Q_stricmp(text, "spec")) {
		return TEAM_SPECTATOR;
	} else if(!Q_stricmp(text, "spectator")) {
		return TEAM_SPECTATOR;
	} else if(!Q_stricmp(text, "TEAM_SPECTATOR")) {
		return TEAM_SPECTATOR;
	}

	if(g_gametype.integer >= GT_SINGLE_PLAYER)
	{
		if(!Q_stricmp(text, "red"))
		{
			return TEAM_RED;
		} else if(!Q_stricmp(text, "TEAM_RED")) {
			return TEAM_RED;
		} else if(!Q_stricmp(text, g_redteam.string)) {
			return TEAM_RED;
		} else if(!Q_stricmp(text, "blue")) {
			return TEAM_BLUE;
		} else if(!Q_stricmp(text, "TEAM_BLUE")) {
			return TEAM_BLUE;
		} else if(!Q_stricmp(text, g_blueteam.string)) {
			return TEAM_BLUE;
		}
	}
	return TEAM_FREE;
}

//AHelper_ResolveClientNum
//Resolves a client number for the text
static int AHelper_ResolveClientNum( char *text )
{
	int i;
	if(atoi(text))
	{
		if(atoi(text) > level.numConnectedClients)
			return -1;
		else
			return atoi(text);
	}
	//Loop through connected clients and see what we get
	for(i = 0; i < level.numConnectedClients; i++)
	{
		if(!Q_strncmp(text, level.clients[i].pers.netname, (int)strlen(text)))
		{
			return i;
		}
	}
	return -1;
}

//Display client numbers
//Required: At least Rank 1
void ACmd_Status( gentity_t *ent )
{
	char textBuffer[1024];
	int i;
	if(ent->client->adminRank < 1)
	{
		trap_SendServerCommand(ent->client->ps.clientNum, "print \"You must be at least Rank 1 to carry out this action.\n\"");
		return;
	}
	strcpy(textBuffer, "cNum  |  Client Name\n");
	strcat(textBuffer, "------------------------------------\n");

	for(i = 0; i < level.numConnectedClients; i++)
	{
		strcat(textBuffer,  va("%i   | %s^7\n", level.clients[i].ps.clientNum, level.clients[i].pers.netname));
	}
	trap_SendServerCommand(ent->client->ps.clientNum, va("print \"%s\n\"", textBuffer));
}

//Force people to teams
//Required: At least Rank 2
void ACmd_ForceTeam( gentity_t *ent )
{
	//int teamNumber;
	int clientNum;
	char buffer[128];
	if(trap_Argc() < 3)
	{
		//Need more args!
		trap_SendServerCommand(ent->s.number, "print \"Usage: /acp_team <client name or number> <team>\n\"");
		return;
	}
	trap_Argv(1, buffer, 128);
	clientNum = AHelper_ResolveClientNum(buffer);
	if(clientNum < 0)
	{
		//Invalid client
		trap_SendServerCommand(ent->s.number, "print \"Invalid client number.\n\"");
		return;
	}
	trap_Argv(2, buffer, 128);
	//teamNumber = AHelper_ResolveTeam(buffer);
	if(!Q_stricmp(buffer, "spec") || !Q_stricmp(buffer, "spectator") || !Q_stricmp(buffer, "s"))
	{
		trap_SendServerCommand(-1, va("chat \"^3%s ^3was forced to spectate.\n\"", g_entities[clientNum].client->pers.netname));
		SetTeam(&g_entities[clientNum], "s");
	}
	else
	{
		SetTeam(&g_entities[clientNum], buffer);
	}
}