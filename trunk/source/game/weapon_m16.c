#include "g_local.h"
#include "g_weapon.h"

extern vec3_t	wp_forward, wp_vright, wp_up;
extern vec3_t	wp_muzzle;
#ifdef AKIMBO
extern vec3_t	wp_muzzle2;
#endif
/*
======================================================================

REPEATER

======================================================================
*/

void RepeaterAltThinkStandard( gentity_t *ent )
{
	if(ent->s.userInt1 < (level.time - 2500))
	{
		G_FreeEntity( ent );
	}
	else if(ent->s.userInt1 < level.time)
	{
		ent->s.pos.trDelta[0] += crandom() * 3.0f;
		ent->s.pos.trDelta[1] += crandom() * 3.0f;
		ent->s.pos.trDelta[2] += crandom() * 3.0f;
		ent->flags &= ~(FL_BOUNCE_HALF);
		ent->bounceCount = 0;
	}
	ent->nextthink = level.time;
}

//---------------------------------------------------------
#ifdef AKIMBO
static void WP_RepeaterMainFire( gentity_t *ent, vec3_t dir, qboolean akimboGun )
#else
static void WP_RepeaterMainFire( gentity_t *ent, vec3_t dir )
#endif
//---------------------------------------------------------
{
	weaponData_t	*wp				= &weaponData[akimboGun ? ent->s.weapon2 : ent->s.weapon];
	int damage = wp->damage;
	gentity_t *missile; 
	vec3_t start;
#ifndef AKIMBO
	VectorCopy(wp_muzzle, start);
#else
	if(akimboGun)
	{
		VectorCopy(wp_muzzle2, start);
	}
	else
	{
		VectorCopy(wp_muzzle, start);
	}
#endif

	if(ent->client)
	{
		if(ent->client->ps.zoomMode == 1 || ent->client->pers.cmd.buttons & BUTTON_SIGHTS /*&& weaponNum < WP_AUXILIARY1*/)
		{
			VectorCopy( ent->client->ps.origin, start );
			start[2] += ent->client->ps.viewheight;//By eyes
		}
	}

	missile = CreateMissile( start, dir, wp->bulletSpeed, 10000, ent, qfalse );
	if(g_instagib.integer == 1)
		damage = 3000;

	missile->classname = "repeater_proj";
	missile->s.weapon = ent->s.weapon;

	missile->alt_fire = qfalse;

	missile->damage = damage;
	missile->dflags = DAMAGE_DEATH_KNOCKBACK;
	missile->methodOfDeath = wp->methodOfDeath;
	missile->clipmask = MASK_SHOT | CONTENTS_LIGHTSABER;

	// we don't want it to bounce forever
	missile->bounceCount = 8;
}

//---------------------------------------------------------
static void WP_RepeaterAltFire( gentity_t *ent )
//---------------------------------------------------------
{
	weaponData_t	*wp				= &weaponData[ent->s.weapon];

	gentity_t *missile = CreateMissile( wp_muzzle, wp_forward, wp->altBulletSpeed, 10000, ent, qtrue );

	missile->classname = "repeater_alt_proj";
	missile->s.weapon = ent->s.weapon;

	VectorSet( missile->r.maxs, REPEATER_ALT_SIZE, REPEATER_ALT_SIZE, REPEATER_ALT_SIZE );
	VectorScale( missile->r.maxs, -1, missile->r.mins );
	missile->s.pos.trType = TR_GRAVITY;
	missile->s.pos.trDelta[2] += 40.0f; //give a slight boost in the wp_upward direction
	missile->damage = wp->altDamage;
	missile->dflags = DAMAGE_DEATH_KNOCKBACK;
	missile->methodOfDeath = wp->altMethodOfDeath;
	missile->splashMethodOfDeath = MOD_REPEATER_ALT_SPLASH;
	missile->clipmask = MASK_SHOT | CONTENTS_LIGHTSABER;
	missile->splashDamage = wp->altDamage;
	missile->s.eFlags |= EF_ALT_FIRING;
	missile->alt_fire = qtrue;
	if ( g_gametype.integer == GT_SIEGE )	// we've been having problems with this being too hyper-potent because of it's radius
	{
		missile->splashRadius = REPEATER_ALT_SPLASH_RAD_SIEGE;
	}
	else
	{
		missile->splashRadius = REPEATER_ALT_SPLASH_RADIUS;
	}

	// we don't want it to bounce forever
	missile->s.userInt1 = level.time + 200;
	missile->flags |= FL_BOUNCE_HALF;
	missile->think = RepeaterAltThinkStandard;
	missile->nextthink = level.time + 100;
	missile->bounceCount = -5;
}

//---------------------------------------------------------
#ifdef AKIMBO
void WP_FireRepeater( gentity_t *ent, qboolean altFire, qboolean primWeapon )
#else
void WP_FireRepeater( gentity_t *ent, qboolean altFire )
#endif
//---------------------------------------------------------
{
	vec3_t	dir, angs;

	vectoangles( wp_forward, angs );

	angs[PITCH] += crandom() * ( ent->client->ps.stats[STAT_ACCURACY] * weaponData[ent->client->ps.weapon].accuracyThreshold );
	angs[YAW] += crandom() * ( ent->client->ps.stats[STAT_ACCURACY] * weaponData[ent->client->ps.weapon].accuracyThreshold );

	AngleVectors( angs, dir, NULL, NULL );

	if ( altFire )
	{
		WP_RepeaterAltFire( ent );
	}
	else
	{
		// add some slop to the alt-fire direction
#ifndef AKIMBO
		WP_RepeaterMainFire( ent, dir );
#else
		if(!primWeapon)
		{
			WP_RepeaterMainFire( ent, dir, qtrue);
		}
		else
		{
			WP_RepeaterMainFire( ent, dir, qfalse );
		}
#endif
	}
}