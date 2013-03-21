#include "g_local.h"
#include "g_weapon.h"

vec3_t	forward, vright, up;
vec3_t	muzzle;
extern void touch_NULL( gentity_t *ent, gentity_t *other, trace_t *trace );
extern void WP_TraceSetStart( gentity_t *ent, vec3_t start, vec3_t mins, vec3_t maxs );
void flashThinkStandard(gentity_t *ent);
void fb_targetsInRadius(int *size, int *output, vec3_t origin, float radius)
{
	trace_t tr;
	int i;
	int j = 0;

	for(i = 0; i < *size; i++)
	{
		if(g_entities[i].client)
		{
			if(VectorDistance(origin, g_entities[i].client->ps.origin) <= radius)
			{
				trap_Trace(&tr, origin, vec3_origin, vec3_origin, g_entities[i].client->ps.origin, -1, CONTENTS_SOLID);
				if(VectorCompare(g_entities[i].client->ps.origin, tr.endpos))
				{
					output[j] = i;
					j++;
				}
			}
		}
	}
	*size = j;
	return;
}
void laserTrapExplode(gentity_t *ent){
	if ( !ent->count )
	{
		//G_Sound( ent, CHAN_WEAPON, G_SoundIndex( "sound/weapons/thermal/warning.wav" ) );
		ent->count = 1;
		ent->genericValue5 = level.time + 500;
		ent->think = flashThinkStandard;
		ent->nextthink = level.time;
		ent->r.svFlags |= SVF_BROADCAST;//so everyone hears/sees the explosion?
	}
	else
	{
		//int *flashedTargets = malloc(MAX_CLIENTS * sizeof(int));
		int flashedTargets[MAX_CLIENTS];
		int size = MAX_CLIENTS;
		int i;
		vec3_t	origin;
		vec3_t	dir={0,0,1};
		char *wut = va( "flashbang %f %f %f", ent->s.origin[0], ent->s.origin[1], ent->s.origin[2] );

		BG_EvaluateTrajectory( &ent->s.pos, level.time, origin );
		origin[2] += 8;
		SnapVector( origin );
		G_SetOrigin( ent, origin );

		ent->s.eType = ET_GENERAL;
		G_AddEvent( ent, EV_MISSILE_MISS, DirToByte( dir ) );
		ent->freeAfterEvent = qtrue;

		if (G_RadiusDamage( ent->r.currentOrigin, ent->parent,  ent->splashDamage, ent->splashRadius, 
				ent, ent, ent->splashMethodOfDeath))
		{
			g_entities[ent->r.ownerNum].client->accuracy_hits++;
		}
		fb_targetsInRadius(&size, flashedTargets, ent->r.currentOrigin, 4096);
		for(i = 0; i < size; i++)
		{
			vec3_t angleTemp;
			gentity_t *flashedEnt = &g_entities[flashedTargets[i]];
			if(flashedEnt->client)
			{
				VectorSubtract( ent->r.currentOrigin, flashedEnt->client->ps.origin, angleTemp );
				vectoangles(angleTemp, angleTemp);
				if(InFieldOfVision( flashedEnt->client->ps.viewangles, 100, angleTemp ))
				{ //Within 100 degrees of our view
					trap_SendServerCommand(flashedTargets[i], wut); //Flash the target
					if(VectorDistance(ent->r.currentOrigin, flashedEnt->r.currentOrigin) < 1024)
					{ //Play blinding event when..blinded?
						G_AddEvent(flashedEnt, EV_BLIND, 0); //Don't need an eventParm with blinding
					}
					//Display hitmarker on the owner of the flashbang
					trap_SendServerCommand( ent->r.ownerNum, "poof" );
				}
			}
		}

		level.explodeTime = level.time + 15000;
		trap_LinkEntity( ent );
	}
}
void flashThinkStandard(gentity_t *ent)
{
	if (ent->genericValue5 < level.time)
	{
		ent->think = laserTrapExplode;
		ent->nextthink = level.time;
		return;
	}

	G_RunObject(ent);
	ent->nextthink = level.time;
}
//---------------------------------------------------------
gentity_t *WP_FireFlashbang( gentity_t *ent, qboolean altFire )
//---------------------------------------------------------
{
	weaponData_t	*wp				= &weaponData[WP_TRIP_MINE];
	gentity_t	*bolt;
	vec3_t		dir, start;
	float chargeAmount = 1.0f; // default of full charge

	AngleVectors( ent->client->ps.viewangles, forward, vright, up );
	CalcMuzzlePoint( ent, forward, vright, up, muzzle );

	VectorCopy( forward, dir );
	VectorCopy( muzzle, start );

	bolt = G_Spawn();
	
	bolt->physicsObject = qtrue;

	bolt->classname = "thermal_detonator";
	bolt->think = flashThinkStandard;
	bolt->nextthink = level.time;
	bolt->touch = touch_NULL;

	// How 'bout we give this thing a size...
	VectorSet( bolt->r.mins, -3.0f, -3.0f, -3.0f );
	VectorSet( bolt->r.maxs, 3.0f, 3.0f, 3.0f );
	bolt->clipmask = MASK_SHOT;

	WP_TraceSetStart( ent, start, bolt->r.mins, bolt->r.maxs );//make sure our start point isn't on the other side of a wall

	if ( ent->client )
	{
		chargeAmount = level.time;
	}

	// get charge amount
	chargeAmount = chargeAmount / (float)TD_VELOCITY;

	if ( chargeAmount > 1.0f )
	{
		chargeAmount = 1.0f;
	}
	else if ( chargeAmount < TD_MIN_CHARGE )
	{
		chargeAmount = TD_MIN_CHARGE;
	}

	// normal ones bounce, alt ones explode on impact
	bolt->genericValue5 = level.time + TD_TIME; // How long 'til she blows
	bolt->s.pos.trType = TR_GRAVITY;
	bolt->parent = ent;
	bolt->r.ownerNum = ent->s.number;
	VectorScale( dir, wp->bulletSpeed, bolt->s.pos.trDelta );

	if ( ent->health >= 0 )
	{
		bolt->s.pos.trDelta[2] += 120;
	}
	bolt->flags |= FL_BOUNCE_HALF;

//	bolt->s.loopSound = G_SoundIndex( "sound/weapons/thermal/thermloop.wav" );
	bolt->s.loopIsSoundset = qfalse;

	bolt->damage = wp->damage;
	bolt->dflags = 0;
	bolt->splashDamage = wp->damage;
	bolt->splashRadius = LT_SPLASH_RAD;

	bolt->s.eType = ET_MISSILE;
	bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
	bolt->s.weapon = WP_TRIP_MINE;

	bolt->methodOfDeath = wp->methodOfDeath;
	bolt->splashMethodOfDeath = wp->methodOfDeath;

	bolt->s.pos.trTime = level.time;		// move a bit on the very first frame
	VectorCopy( start, bolt->s.pos.trBase );
	
	SnapVector( bolt->s.pos.trDelta );			// save net bandwidth
	VectorCopy (start, bolt->r.currentOrigin);

	VectorCopy( start, bolt->pos2 );

	bolt->bounceCount = -5;

	return bolt;
}

void WP_DropFlashbang( gentity_t *ent )
{
	WP_FireFlashbang( ent, qfalse );
}