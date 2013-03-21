#include "g_local.h"

extern void InitMover( gentity_t *ent );
extern void InitTrigger( gentity_t *self );
void SP_func_breakable( gentity_t *self );

typedef enum {
	DOORS_CLOSED,
	DOORS_OPENED1,
	DOORS_OPENED2,
	DOORS_CLOSETOOPEN1,
	DOORS_CLOSETOOPEN2,
	DOORS_OPENED1TOCLOSE,
	DOORS_OPENED2TOCLOSE,
} doorState_t;

/*
================================================================================================

DOOR SOUNDS
These are vastly different from func_door's approach

================================================================================================
*/

void G_PlayNewDoorSound( gentity_t *ent, int type )
{
	int soundSet;
	//This code look familiar yet?
	if(!ent->soundSet || !ent->soundSet[0])
	{
		return;
	}
	soundSet = BG_GetDoorSoundIndex(ent->soundSet);
	if(soundSet < 0)
		return;

	ent->s.soundSetIndex = soundSet;

	G_AddEvent(ent, EV_NEW_DOORSOUND, type);
}

//==============================================================================================
//
// DOOR CODE GOES BELOW
//
//==============================================================================================

void door_new_use( gentity_t *ent, gentity_t *other, gentity_t *activator, int desiredState );

qboolean G_PointInBounds( vec3_t point, vec3_t mins, vec3_t maxs );
void trigger_doorX_touch( gentity_t *ent, gentity_t *other, trace_t *trace )
{
	//Grab the door entity
	gentity_t *doorEnt;// = G_Find(NULL, FOFS(isTrigger1 ? target : target2), ent->targetname);
	if(other->client->pers.cmd.buttons & BUTTON_USE)
	{
		if(ent->wait > level.time)
		{
			return;
		}
		if(ent->isTrigger1)
		{
			doorEnt = G_Find(NULL, FOFS(target), ent->targetname);
		}
		else
		{
			doorEnt = G_Find(NULL, FOFS(target2), ent->targetname);
		}
		if(!doorEnt)
		{
			Com_Error(ERR_DISCONNECT, "trigger_doorX_use: No target");
			return;
		}
		//We're unlocked.
		if(!doorEnt->lockCount)
		{
			if(other->client->pers.cmd.buttons & BUTTON_SIGHTS && ent->genericValue7)
			{
				//Locking the door.
				if(!G_PointInBounds(other->client->ps.origin, ent->r.absmin, ent->r.absmax))
				{
					return;
				}
				else if(other->client->isHacking != ent->s.number && other->s.number < MAX_CLIENTS)
				{
					//Start the locking
					other->client->isHacking = ent->s.number;
					VectorCopy(other->client->ps.viewangles, other->client->hackingAngles);
					other->client->ps.hackingTime = level.time + ent->genericValue7;
					other->client->ps.hackingBaseTime = ent->genericValue7;
					if (other->client->ps.hackingBaseTime > 60000)
					{
						//Capped at 60 seconds
						other->client->ps.hackingTime = level.time + 60000;
						other->client->ps.hackingBaseTime = 60000;
					}
					return;
				}
				else if (other->client->ps.hackingTime < level.time)
				{
					//Lock is locking
					other->client->isHacking = 0;
					other->client->ps.hackingTime = 0;
				}
				else
				{
					//Locking is in progress
					return;
				}
				//Lock the door
				ent->wait = 500;
				G_AddEvent(doorEnt, EV_DOOR_LOCKED, 1);
				doorEnt->lockCount++;
				trap_SendServerCommand(other->s.number, "cp \"^3Door locked.\"");
			}
			else
			{
				if(ent->isTrigger1)
				{
					door_new_use(doorEnt, ent, other, DOORS_CLOSETOOPEN1);
				}
				else
				{
					door_new_use(doorEnt, ent, other, DOORS_CLOSETOOPEN2);
				}
			}
		}
		//Locked.
		else if(doorEnt->lockCount)
		{
			if(other->client->pers.cmd.buttons & BUTTON_SIGHTS && ent->genericValue7)
			{
				//Holding down ADS, therefore we're trying to pick the lock.
				if(!G_PointInBounds(other->client->ps.origin, ent->r.absmin, ent->r.absmax))
				{
					return;
				}
				else if(other->client->isHacking != ent->s.number && other->s.number < MAX_CLIENTS)
				{
					//Start the lock picking
					other->client->isHacking = ent->s.number;
					VectorCopy(other->client->ps.viewangles, other->client->hackingAngles);
					other->client->ps.hackingTime = level.time + ent->genericValue7;
					other->client->ps.hackingBaseTime = ent->genericValue7;
					if (other->client->ps.hackingBaseTime > 60000)
					{
						//Capped at 60 seconds
						other->client->ps.hackingTime = level.time + 60000;
						other->client->ps.hackingBaseTime = 60000;
					}
					return;
				}
				else if (other->client->ps.hackingTime < level.time)
				{
					//Lock is picked.
					other->client->isHacking = 0;
					other->client->ps.hackingTime = 0;
				}
				else
				{
					//Picking lock is in progress
					return;
				}
				//Unlock the door
				ent->wait = 500;
				G_AddEvent(doorEnt, EV_DOOR_LOCKED, 1);
				doorEnt->lockCount--;
			}
			else
			{
				//Not holding ADS. Skip it.
				G_AddEvent(doorEnt, EV_DOOR_LOCKED, 0);
				doorEnt->wait = level.time + 750;
				ent->wait = level.time + 750;
				other->client->pers.cmd.buttons &= ~BUTTON_USE;
			}
		}
	}
}

void SP_trigger_doorX ( gentity_t *ent )
{
	if(!Q_stricmp(ent->classname, "trigger_door1"))
	{
		ent->isTrigger1 = qtrue;
	}
	else
	{
		ent->isTrigger1 = qfalse;
	}

	G_SpawnInt("useTime", "5000", &ent->genericValue7);

	ent->touch = trigger_doorX_touch;
	InitTrigger( ent );
	trap_LinkEntity (ent);
}

//=========================================================================================================================

void door_new_blocked( gentity_t *ent, gentity_t *other )
{
	if((level.time - ent->s.apos.trTime) > ((ent->s.apos.trTime+ent->s.apos.trDuration)/3))
	{
		//Not more than a third the distance gone, don't bother going backwards
		return;
	}
	if(ent->doorState == DOORS_CLOSED || ent->doorState == DOORS_OPENED1 || ent->doorState == DOORS_OPENED2)
	{
		//Can't be blocked if we're in a stationary state.
		return;
	}
	if(ent->damage)
	{
		//Only do damage when we're opening the door.
		if(ent->doorState == DOORS_CLOSETOOPEN1 || ent->doorState == DOORS_CLOSETOOPEN1)
		{
			G_Damage(other, ent, ent->enemy, NULL, NULL, ent->damage, 0, MOD_CRUSH);
		}
	}

	//K. Check our state, and send us in the opposite direction.
	switch(ent->doorState)
	{
		case DOORS_CLOSETOOPEN1:
			break;
		case DOORS_CLOSETOOPEN2:
			break;
		case DOORS_OPENED1TOCLOSE:
			break;
		case DOORS_OPENED2TOCLOSE:
			break;
	}
}

void door_new_reach_open( gentity_t *ent )
{
	if(ent->doorState == DOORS_CLOSETOOPEN1)
	{
		ent->doorState = DOORS_OPENED1;
		//VectorCopy(ent->r.currentAngles, ent->s.apos.trBase);
	}
	else if(ent->doorState == DOORS_CLOSETOOPEN2)
	{
		ent->doorState = DOORS_OPENED2;
		//VectorCopy(ent->r.currentAngles, ent->s.apos.trBase);
	}
}

void door_new_reach_closed( gentity_t *ent )
{
	ent->doorState = DOORS_CLOSED;
	G_PlayNewDoorSound(ent, NDS_CLOSE);
	ent->reached = 0;
}

void door_new_go( gentity_t *ent )
{
	//Start moving the door along the desired axis.
	gentity_t *activator = ent->activator;
	vec3_t absZero = {0, 0, 0};

	if(ent->doorState == DOORS_CLOSED)
	{
		//This is interesting because we need to rotate the door based on where we are.
		
		// use targets..good for map-based booby traps
		//G_UseTargets( ent, ent->activator );
		// open areaportal
		if ( ent->teammaster == ent || !ent->teammaster ) {
			trap_AdjustAreaPortalState( ent, qfalse );
		}
	}
	if(ent->doorState == DOORS_CLOSETOOPEN1)
	{
		if(!ent->noDeathSound) //Don't want sounds to play if we're getting breached
			G_PlayNewDoorSound(ent, NDS_OPEN);
		VectorCopy(ent->pos1, ent->s.apos.trBase);
		VectorCopy(ent->s.apos.trDelta, ent->doorAngles);
		if(ent->doorAngles[0] < 0)
			ent->doorAngles[0] *= -1;
		if(ent->doorAngles[1] < 0)
			ent->doorAngles[1] *= -1;
		if(ent->doorAngles[1] < 0)
			ent->doorAngles[2] *= -1;
		/*ent->doorAngles[0] = 0;
		ent->doorAngles[1] = 0;
		ent->doorAngles[2] = 0;*/
		VectorCopy(ent->doorAngles, ent->s.apos.trDelta);

		/*VectorCopy( ent->s.origin, ent->s.pos.trBase );
		VectorCopy( ent->s.pos.trBase, ent->r.currentOrigin );
		VectorCopy( ent->s.apos.trBase, ent->r.currentAngles );*/

		ent->s.apos.trTime = level.time;
		ent->s.apos.trType = TR_LINEAR_STOP;
		ent->s.apos.trDuration = (VectorDistance(absZero, ent->s.apos.trDelta)/ent->speed)*1000;
		ent->reached = door_new_reach_open;
	}
	else if(ent->doorState == DOORS_CLOSETOOPEN2)
	{
		if(!ent->noDeathSound)
			G_PlayNewDoorSound(ent, NDS_OPEN);
		VectorCopy(ent->pos1, ent->s.apos.trBase);
		VectorCopy(ent->s.apos.trDelta, ent->doorAngles);
		if(ent->doorAngles[0] > 0)
			ent->doorAngles[0] *= -1;
		if(ent->doorAngles[1] > 0)
			ent->doorAngles[1] *= -1;
		if(ent->doorAngles[2] > 0)
			ent->doorAngles[2] *= -1;
		//ent->speed = VectorLength(ent->doorAngles);
		VectorCopy(ent->doorAngles, ent->s.apos.trDelta);

		/*VectorCopy( ent->s.origin, ent->s.pos.trBase );
		VectorCopy( ent->s.pos.trBase, ent->r.currentOrigin );
		VectorCopy( ent->s.apos.trBase, ent->r.currentAngles );*/

		ent->s.apos.trTime = level.time;
		ent->s.apos.trType = TR_LINEAR_STOP;
		ent->s.apos.trDuration = (VectorDistance(absZero, ent->s.apos.trDelta)/ent->speed)*1000;
		ent->reached = door_new_reach_open;
	}
	else if(ent->doorState == DOORS_OPENED1TOCLOSE || ent->doorState == DOORS_OPENED2TOCLOSE)
	{
		if(!ent->noDeathSound)
			G_PlayNewDoorSound(ent, NDS_SQUEAK);
		ent->s.apos.trDuration = (VectorDistance(absZero, ent->s.apos.trDelta)/ent->speed)*1000;
		VectorCopy(ent->s.apos.trDelta, ent->s.apos.trBase);
		VectorSubtract( ent->pos1, ent->s.apos.trBase, ent->s.apos.trDelta );
		ent->s.apos.trTime = level.time;
		ent->s.apos.trType = TR_LINEAR_STOP;
		ent->reached = door_new_reach_closed;
	}
}

void door_new_use( gentity_t *ent, gentity_t *other, gentity_t *activator, int desiredState )
{
	if(!ent->use)
	{
		//not able to use me
		return;
	}
	if(ent->wait > level.time)
	{
		return;
	}

	if( ent->flags & FL_INACTIVE )
	{
		//Not able to use me
		return;
	}

	if(ent->doorBroken)
	{
		//Door's pretty broken, boss
		trap_SendServerCommand( activator->s.number, "send_event \"The door is broken; it cannot be used.\"" );
		return;
	}

	if( ent->doorState != DOORS_CLOSED && ent->doorState != DOORS_OPENED1 && ent->doorState != DOORS_OPENED2 )
	{
		//Not able to use me!
		return;
	}
	if(ent->doorState == DOORS_CLOSED && ent->lockType > 0 && ent->lockCount > 0)
	{
		//Locked.
		G_AddEvent(ent, EV_DOOR_LOCKED, 0);
		ent->wait = level.time + 500;
		activator->client->pers.cmd.buttons &= ~BUTTON_USE;
		return;
	}
	switch(ent->doorState)
	{
		case DOORS_OPENED1:
			{
				switch(desiredState)
				{
					case DOORS_CLOSETOOPEN2:
						desiredState = DOORS_OPENED1TOCLOSE;
						ent->noDeathSound = qfalse;
						break;
					case DOORS_CLOSETOOPEN1:
					case DOORS_OPENED2TOCLOSE:
						return;
				}
			}
			break;
		case DOORS_OPENED2:
			{
				switch(desiredState)
				{
					case DOORS_CLOSETOOPEN1:
						desiredState = DOORS_OPENED2TOCLOSE;
						ent->noDeathSound = qfalse;
						break;
					case DOORS_CLOSETOOPEN2:
					case DOORS_OPENED1TOCLOSE:
						return;
				}
			}
			break;
	}

	ent->enemy = other;
	ent->activator = activator;
	ent->doorState = desiredState;
	if(ent->delay)
	{
		ent->think = door_new_go;
		ent->nextthink = level.time + ent->delay;
	}
	else
	{
		door_new_go(ent);
	}
}

void door_new_use_function( gentity_t *ent, gentity_t *other, gentity_t *activator )
{
	if(ent->doorState == DOORS_OPENED1)
	{
		door_new_use( ent, other, activator, DOORS_OPENED1TOCLOSE );
	}
	else if(ent->doorState == DOORS_OPENED2)
	{
		door_new_use( ent, other, activator, DOORS_OPENED2TOCLOSE );
	}
}
void SP_func_door_new (gentity_t *ent) {
	vec3_t spinangles;
	/*if ( ent->health )
	{
		int sav_spawnflags = ent->spawnflags;
		ent->spawnflags = 0;
		SP_func_breakable( ent );
		ent->spawnflags = sav_spawnflags;
	}
	else*/
	{
		trap_SetBrushModel( ent, ent->model );
		InitMover( ent );

		VectorCopy( ent->s.origin, ent->s.pos.trBase );
		VectorCopy( ent->s.pos.trBase, ent->r.currentOrigin );
		VectorCopy( ent->s.apos.trBase, ent->r.currentAngles );

		trap_LinkEntity( ent );
	}
	VectorCopy(ent->s.apos.trBase, ent->pos1);
#ifdef _XBOX
	int	tempModelScale;
	G_SpawnInt("model2scale", "0", &tempModelScale);
	ent->s.iModelScale = tempModelScale;
#else
	G_SpawnInt("model2scale", "0", &ent->s.iModelScale);
#endif
	if (ent->s.iModelScale < 0)
	{
		//[Asteroids]
		//NOTE: -1 scale is x -100% (so -3 is 300%)
		ent->s.legsFlip = qtrue;//treat it as a scalar
		ent->s.iModelScale = -ent->s.iModelScale;
		//[/Asteroids]
	}
	else if (ent->s.iModelScale > 1023)
	{
		ent->s.iModelScale = 1023;
	}

	G_SpawnFloat("speed", "45", &ent->speed);
	G_SpawnInt("lockType", "0", &ent->lockType);
	if(ent->lockType > 0)
	{
		ent->lockCount = 1;
	}

	ent->blocked = door_new_blocked;

	if ( G_SpawnVector( "spinangles", "0 90 0", spinangles ) )
	{
		ent->speed = VectorLength( spinangles );
		// set the axis of rotation
		VectorCopy( spinangles, ent->s.apos.trDelta );
		//VectorCopy( spinangles, ent->doorAngles ); 
	}
	else
	{
		/*if ( !ent->speed ) {
			ent->speed = 100;
		}
		// set the axis of rotation
		if ( ent->spawnflags & 4 ) {
			ent->s.apos.trDelta[2] = ent->speed;
		} else if ( ent->spawnflags & 8 ) {
			ent->s.apos.trDelta[0] = ent->speed;
		} else {
			ent->s.apos.trDelta[1] = ent->speed;
		}*/
		trap_UnlinkEntity(ent);
		return;
	}

	//[CoOp]
	if(ent->targetname)
	{//we're used as a target of another entity, don't move until we're triggered
		ent->s.apos.trType = TR_STATIONARY;
		// stop the sound if it stops moving
		ent->s.loopSound = 0;
		ent->s.loopIsSoundset = qfalse;
	}
	else
	{
		if ( ent->soundSet && ent->soundSet[0] )
		{
			ent->s.soundSetIndex = G_SoundSetIndex(ent->soundSet);
			//G_AddEvent( self, EV_BMODEL_SOUND, BMS_START );
			ent->s.loopSound = BMS_MID;
			ent->s.loopIsSoundset = qtrue;
		}
		ent->s.apos.trType = TR_STATIONARY;
	}
	//ent->s.apos.trType = TR_LINEAR;
	//[CoOp]

	if (!ent->damage) {
		if ( (ent->spawnflags&16) )//IMPACT
		{
			ent->damage = 10000;
		}
		else
		{
			ent->damage = 2;
		}
	}
	if ( (ent->spawnflags&2) )//RADAR
	{//show up on Radar at close range and play impact sound when close...?  Range based on my size
		ent->s.speed = Distance( ent->r.absmin, ent->r.absmax )*0.5f;
		ent->s.eFlags |= EF_RADAROBJECT;
	}

	//[CoOp]
	//make func_rotating toggle when used.
	ent->doorState = DOORS_CLOSED;
	ent->use = door_new_use_function;

	//Make it breachable
	ent->takedamage = qtrue;
	//[/CoOp]
}

//=================================================================================================================



/*
================================================================================================

BREACHING A DOOR
Three weapons can breach a door:
- Shotgun
- C4
- Melee

Melee will not break the door. Other methods will break the door.
Also, you can pick the lock.

================================================================================================
*/
//These functions get called whenever we get breached by an object that "breaks" us
void DoorNew_BreachDie_1(gentity_t *self, gentity_t *inflictor, gentity_t *attacker, int damage, int meansOfDeath)
{
	self->doorBroken = qtrue;
	
}
void DoorNew_BreachDie_2(gentity_t *self, gentity_t *inflictor, gentity_t *attacker, int damage, int meansOfDeath)
{
	self->doorBroken = qtrue;
}

void G_CheckForDoorDamage( gentity_t *ent, gentity_t *inflictor, int damage, int methodOfDeath )
{
	gentity_t *triggerEnt;
	qboolean stopThatJaden = qfalse;
	if(Q_stricmp(ent->classname, "func_door_new"))
	{
		//Not a door that can be breached
		return;
	}
	if(!ent->lockCount)
	{
		//Not locked.
		if(methodOfDeath == MOD_MELEE)
		{
meleeUnlock:
			triggerEnt = G_Find(NULL, FOFS(targetname), ent->target);
			if(!triggerEnt)
			{
				//Probably not set up properly
				return;
			}
			if(trap_EntityContact(triggerEnt->r.absmin, triggerEnt->r.absmax, inflictor))
			{
				//Entity is within trigger_door1, swing the other way!
				ent->noDeathSound = qtrue;
				door_new_use(ent, inflictor, inflictor, DOORS_CLOSETOOPEN1);
				ent->noDeathSound = qfalse;
				return;
			}
			triggerEnt = G_Find(NULL, FOFS(targetname), ent->target2);
			if(!triggerEnt)
				return;

			if(trap_EntityContact(triggerEnt->r.absmin, triggerEnt->r.absmax, inflictor))
			{
				ent->noDeathSound = qtrue;
				door_new_use(ent, inflictor, inflictor, DOORS_CLOSETOOPEN2);
				ent->noDeathSound = qfalse;
				return;
			}
			if(stopThatJaden)
			{
				return;
			}
		}
		/*switch(methodOfDeath)
		{
			//All of these break the door.
			case MOD_FLECHETTE:
			case MOD_M1014:
			case MOD_SAWED_OFF:
				//These are special in that they require the inflictor to be in range.
				{
					if(ent->health-damage <= 0)
					{
						//Set us up to "die" 

					}
				}
				break;
		}*/
	}
	else if(ent->lockCount)
	{
		if((ent->health-(damage+Q_irand(-20, 10))) <= 0)
		{
			if(methodOfDeath == MOD_MELEE)
			{
				G_AddEvent(ent, EV_DOOR_LOCKED, 1);
				ent->lockCount = 0;
				trap_SendServerCommand(inflictor->s.number, "aprog 59 1");
				goto meleeUnlock;
			}
		}
	}
}