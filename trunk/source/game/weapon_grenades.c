#include "g_weapon.h"
#include "g_local.h"

static vec3_t	forward, vright, up;
static vec3_t	muzzle;
extern void touch_NULL( gentity_t *ent, gentity_t *other, trace_t *trace );
extern void WP_TraceSetStart( gentity_t *ent, vec3_t start, vec3_t mins, vec3_t maxs );
void thermalThinkStandard(gentity_t *ent);
//---------------------------------------------------------
void thermalDetonatorExplode( gentity_t *ent )
//---------------------------------------------------------
{
	if ( !ent->count )
	{
		//G_Sound( ent, CHAN_WEAPON, G_SoundIndex( "sound/weapons/thermal/warning.wav" ) );
		ent->count = 1;
		ent->genericValue5 = level.time + 500;
		ent->think = thermalThinkStandard;
		ent->nextthink = level.time;
		ent->r.svFlags |= SVF_BROADCAST;//so everyone hears/sees the explosion?
	}
	else
	{
		vec3_t	origin;
		vec3_t	dir={0,0,1};

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

		level.explodeTime = level.time + 15000;

		trap_LinkEntity( ent );
	}
}

void thermalThinkStandard(gentity_t *ent)
{
	if (ent->genericValue5 < level.time)
	{
		ent->think = thermalDetonatorExplode;
		ent->nextthink = level.time;
		trap_SendServerCommand(-1, va("gid %i", ent->s.number));
		return;
	}
	ent->s.eFlags = EF_RADAROBJECT;			//Grenade indicator
	ent->s.eFlags2 = EF_RADAROBJECT;			//Grenade indicator
	G_RunObject(ent);
	ent->nextthink = level.time;
}

//---------------------------------------------------------
gentity_t *WP_FireThermalDetonator( gentity_t *ent, qboolean altFire )
//---------------------------------------------------------
{
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
	bolt->think = thermalThinkStandard;
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

	bolt->genericValue5 = level.time + TD_TIME; // How long 'til she blows
	bolt->s.pos.trType = TR_GRAVITY;
	bolt->parent = ent;
	bolt->r.ownerNum = ent->s.number;
	VectorScale( dir, weaponData[WP_THERMAL].bulletSpeed, bolt->s.pos.trDelta );

	if ( ent->health >= 0 )
	{
		bolt->s.pos.trDelta[2] += 120;
	}
	bolt->flags |= FL_BOUNCE_HALF;
//	bolt->s.loopSound = G_SoundIndex( "sound/weapons/thermal/thermloop.wav" );
	bolt->s.loopIsSoundset = qfalse;

	bolt->damage = TD_DAMAGE;
	bolt->dflags = 0;
	bolt->splashDamage = TD_SPLASH_DAM;
	bolt->fleshBounces = 0; //change to 1 to disable grenade bouncing on enemies
	bolt->splashRadius = TD_SPLASH_RAD;

	bolt->s.eType = ET_MISSILE;
	bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
	bolt->s.weapon = WP_THERMAL;

	bolt->methodOfDeath = MOD_THERMAL;
	bolt->splashMethodOfDeath = MOD_THERMAL_SPLASH;

	bolt->s.pos.trTime = level.time;		// move a bit on the very first frame
	VectorCopy( start, bolt->s.pos.trBase );
	
	SnapVector( bolt->s.pos.trDelta );			// save net bandwidth
	VectorCopy (start, bolt->r.currentOrigin);

	VectorCopy( start, bolt->pos2 );

	bolt->bounceCount = -5;

	return bolt;
}

gentity_t *WP_DropThermal( gentity_t *ent )
{
	AngleVectors( ent->client->ps.viewangles, forward, vright, up );
	return (WP_FireThermalDetonator( ent, qfalse ));
}
