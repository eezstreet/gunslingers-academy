#include "g_local.h"
#include "q_helper.h"
//didz: Domination game type
#define BASE_THINK_TIME 150
//end didz
//eezstreet: Capture_Base
void Capture_Base(gentity_t *self){
	int i = 0, j, stringLength = 0;
	char string[128];
	char entry[8];
	if(!level.baseCount)
		return;
	while(level.baseControl[i] != -1 && i < level.baseCount){
		Com_sprintf(entry, sizeof(entry), "%i ", level.baseControl[i]);
		j = strlen(entry);
		if(stringLength + j > 128)
			break;
		strcpy(string + stringLength, entry);
		stringLength += j;
		i++;
	}
	trap_SendServerCommand(-1, va("sbd %i %s", level.baseCount, string)); //send base data
	trap_SendServerCommand(-1, va("sbn %i %s", level.baseCount, Q_bSA(level.baseNames, level.baseCount))); //send base names
}

//end eezstreet
//didz
void Domination_Base_Think ( gentity_t *self )
{
	int			i				= 0;
	signed int	redSum			= 0;
	float		finalCharge		= 0.0f;
	qboolean	redInside		= qfalse;
	qboolean	blueInside		= qfalse;
	vec3_t		playerVector;
	vec_t		playerDistance;
	gclient_t	*client;
	int			thinkDelta		= level.time - self->timestamp;
	
	for (i=0; i < level.maxclients; i++)
	{
		client = &level.clients[i];

		// This is an ArbModding comment, it might apply to GSA too, idk, possible bug
		//FIXME: Bases think dead bots that don't respawn are living players so... Dead bots are taking mah bases!
		//Haven't tested this with real clients though, maybe it's the same O_O
		//I'm guessing it's some death cam bug
		if (!client ||										//	This dude ain't a client o_O
			(client->sess.sessionTeam != TEAM_RED &&		//	Not red team...
				client->sess.sessionTeam != TEAM_BLUE) ||	//	And not blue team...WTF
			client->ps.stats[STAT_HEALTH] <= 0 ||			//	dead...we don't care
			(client->ps.eFlags & EF_DEAD) ||				//	dead...DO NOT WANT
			client->pers.connected != CON_CONNECTED )		//	Not connected even
			continue;
		
		VectorSubtract( self->s.origin, client->ps.origin, playerVector );
		playerDistance = VectorLength( playerVector );

		if ( playerDistance < self->radius )
		{//This fucker is in our PRIVATE SPACE!
			client->ps.duelIndex = self->s.number;

			switch (client->sess.sessionTeam)
			{//Count how many people of each team are in the radius..for adjusting capture speed!
				case TEAM_RED:
					redInside = qtrue;
					redSum++;
					break;
				case TEAM_BLUE:
					blueInside = qtrue;
					redSum--;
					break;
				default:
					break;
			}
		}
		else if ( client->ps.duelIndex == self->s.number )
		{
			client->ps.duelIndex = ENTITYNUM_NONE;
		}
	}
	
	if ( !redSum ) {
		goto endBaseThink;
	}

	if ( self->s.teamowner == TEAM_RED )
	{
		if ( !blueInside )
		{
			self->preciseHealth = (float)self->maxHealth;
			self->s.health = self->s.maxhealth;
			goto endBaseThink;
		}
		else
		{
			finalCharge = redSum * (float)((self->speed * thinkDelta/1000)*2);
		}
	}
	else if ( self->s.teamowner == TEAM_BLUE )
	{
		if ( !redInside )
		{
			self->preciseHealth = (float)-self->s.maxhealth;
			self->s.health = -self->s.maxhealth;
			goto endBaseThink;
		}
		else
		{
			finalCharge = redSum * (self->speed * thinkDelta/1000);
		}
	}
	else
	{
		finalCharge = redSum * (self->speed * thinkDelta/1000);
	}

	self->preciseHealth += finalCharge;

	if ( self->s.teamowner == TEAM_RED && self->preciseHealth <= 0 )
	{
		self->s.teamowner = TEAM_FREE;
		level.baseControl[self->s.userInt1] = self->s.teamowner;
		Capture_Base(self);
		
	}
	else if ( self->s.teamowner == TEAM_BLUE && self->preciseHealth >= 0 )
	{
		self->s.teamowner = TEAM_FREE;
		level.baseControl[self->s.userInt1] = self->s.teamowner;
		Capture_Base(self);
	}

	if ( self->preciseHealth >= (float)self->s.maxhealth )
	{
		self->preciseHealth = (float)self->s.maxhealth;
		self->s.teamowner = TEAM_RED;
		level.baseControl[self->s.userInt1] = self->s.teamowner;
		Capture_Base(self);
		trap_SendServerCommand(-1, va("dom_cbp %i 1", self->s.userInt1));
	}
	else if ( self->preciseHealth <= (float)-self->s.maxhealth )
	{
		self->preciseHealth = (float)-self->s.maxhealth;
		self->s.teamowner = TEAM_BLUE;
		level.baseControl[self->s.userInt1] = self->s.teamowner;
		Capture_Base(self);
		trap_SendServerCommand(-1, va("dom_cbp %i 2", self->s.userInt1));
	}
	self->s.health = (int)self->preciseHealth; //set our global health...int precision, but the float remains precise (so it's still balanced)

endBaseThink:
	if(level.lastScore == 5){
		if(self->s.teamowner == TEAM_RED || self->s.teamowner == TEAM_BLUE){ //eezstreet edit: Add score
			AddTeamScore(client->ps.origin, self->s.teamowner, 1);
			CalculateRanks();
		}
	}
	level.baseControl[self->s.userInt1] = self->s.teamowner;
	self->timestamp = level.time;
	self->nextthink = level.time + BASE_THINK_TIME;
	Capture_Base(self);
}
void SP_info_domination_base ( gentity_t *ent ){
	char *s; //model, icon, letter
	float f; //radius
	int i; //base index
	IFNGAME(GT_DOMINATION){ //these won't exist, except in the Domination game type
		G_FreeEntity(ent);
		return;
	}
	G_SpawnInt( "index", "0", &i);
	ent->s.userInt1 = i;
	G_SpawnString( "letter", "A", &s );
	if ( !Q_stricmp( s, "A" ) ) {
		ent->s.generic1 = 0;
	} else if ( !Q_stricmp( s, "B" ) ) {
		ent->s.generic1 = 1;
	} else {
		ent->s.generic1 = 2;
	}
	strcpy(level.baseNames[ent->s.userInt1], s);
	G_SpawnString( "model", va("models/map_objects/domination/base_%s.md3", level.baseNames[ent->s.userInt1]), &s );
	ent->model = s;
	ent->s.modelindex = G_ModelIndex( ent->model );
	G_SetOrigin(ent, ent->s.origin);
	ent->s.eType = ET_GENERAL; // Type of entity is just ET_GENERAL
	ent->r.svFlags |= SVF_BROADCAST; // Broadcast the base to all players, in view or not
	ent->s.weapon = WP_NUM_WEAPONS; // Just so client-side knows this is an ET_GENERAL Capturable base :)
	ent->s.eFlags |= EF_RADAROBJECT; // Add base to the players' radars
	G_SpawnString( "icon", "ui/assets/statusbar/camp", &s );
	ent->s.genericenemyindex = G_IconIndex( s ); // Set the radar icon for the base
	G_SpawnFloat( "radius", "200", &f );
	ent->radius = f;
	G_SpawnFloat( "speed", "50", &f );
	ent->speed = f;
	G_SpawnInt( "maxhealth", "100", &i );
	ent->s.maxhealth = i;
	ent->maxHealth = i; //eezstreet edit: Didz confused between s.maxhealth and maxHealth, no adverse effects to making these the same.
	ent->think = Domination_Base_Think;
	ent->nextthink = level.time + BASE_THINK_TIME;
	level.baseCount++;
	trap_LinkEntity(ent);
	Capture_Base(ent);
}
