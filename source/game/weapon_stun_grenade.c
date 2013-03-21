#include "g_local.h"
#include "g_weapon.h"

vec3_t	forward, vright, up;
vec3_t	muzzle;

extern void touch_NULL( gentity_t *ent, gentity_t *other, trace_t *trace );
extern void WP_TraceSetStart( gentity_t *ent, vec3_t start, vec3_t mins, vec3_t maxs );

gentity_t *WP_FireStunGrenade( gentity_t *ent )
{
	weaponData_t	*wp				= &weaponData[WP_AUXILIARY6];
	gentity_t	*bolt;
	vec3_t		dir, start;

	AngleVectors( ent->client->ps.viewangles, forward, vright, up );
	CalcMuzzlePoint( ent, forward, vright, up, muzzle );

	VectorCopy( forward, dir );
	VectorCopy( muzzle, start );

	bolt = G_Spawn();
	
	bolt->physicsObject = qtrue;

	bolt->classname = "thermal_detonator";
	bolt->nextthink = level.time;
	bolt->touch = touch_NULL;

	// How 'bout we give this thing a size...
	VectorSet( bolt->r.mins, -3.0f, -3.0f, -3.0f );
	VectorSet( bolt->r.maxs, 3.0f, 3.0f, 3.0f );
	bolt->clipmask = MASK_SHOT;

	WP_TraceSetStart( ent, start, bolt->r.mins, bolt->r.maxs );//make sure our start point isn't on the other side of a wall

	bolt->s.pos.trType = TR_GRAVITY;
	bolt->parent = ent;
	bolt->r.ownerNum = ent->s.number;
	VectorScale( dir, wp->bulletSpeed, bolt->s.pos.trDelta );

	if ( ent->health >= 0 )
	{
		bolt->s.pos.trDelta[2] += 120;
	}

	bolt->damage = wp->damage;
	bolt->dflags = 0;
	bolt->splashDamage = wp->damage;
	bolt->splashRadius = LT_SPLASH_RAD*1.5;

	bolt->s.eType = ET_MISSILE;
	bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
	bolt->s.weapon = WP_AUXILIARY6;

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