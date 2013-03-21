#include "../g_local.h"
#include "../g_weapon.h"

extern vec3_t	wp_forward, wp_vright, wp_up;
extern vec3_t	wp_muzzle;
/*
======================================================================

BOWCASTER

======================================================================
*/

static void WP_JumboAltFire( gentity_t *ent, vec3_t start, vec3_t dir, qboolean altFire )
{
	int	weaponNum	= (ent->s.eType == ET_NPC) ? ent->s.weapon : ent->client->ps.weapon;
	gentity_t *missile;
	weaponData_t	*wp				= &weaponData[weaponNum];
	int damage		= (!altFire) ? wp->damage : wp->altDamage;
	
	if (ent->s.eType == ET_NPC)
	{ //animent
		damage = 10;
	}
	if(g_instagib.integer == 1)
		damage = 3000;

	missile = CreateMissile( start, dir, ((altFire) ? wp->altBulletSpeed : wp->bulletSpeed), 10000, ent, qtrue );

	missile->classname = "bowcaster_proj";
	missile->s.weapon = weaponNum;

	missile->damage = damage;
	missile->dflags = DAMAGE_DEATH_KNOCKBACK;
	missile->methodOfDeath = (!altFire) ? wp->methodOfDeath : wp->altMethodOfDeath;
	missile->clipmask = MASK_SHOT | CONTENTS_LIGHTSABER;

	VectorSet( missile->r.maxs, 2.5, 2.5, 2.5 );
	VectorScale( missile->r.maxs, -1, missile->r.mins );

	// we don't want it to bounce forever
	missile->bounceCount = 4;
}

//---------------------------------------------------------
/*static void WP_BowcasterMainFire( gentity_t *ent )
//---------------------------------------------------------
{
	int			damage	= BOWCASTER_DAMAGE, count;
	float		vel;
	vec3_t		angs, dir;
	gentity_t	*missile;
	int i;

	if (!ent->client)
	{
		count = 1;
	}
	else
	{
		count = ( level.time - ent->client->ps.weaponChargeTime ) / BOWCASTER_CHARGE_UNIT;
	}

	if ( count < 1 )
	{
		count = 1;
	}
	else if ( count > 5 )
	{
		count = 5;
	}

	if ( !(count & 1 ))
	{
		// if we aren't odd, knock us down a level
		count--;
	}

	//scale the damage down based on how many are about to be fired
	if (count <= 1)
	{
		damage = 50;
	}
	else if (count == 2)
	{
		damage = 45;
	}
	else if (count == 3)
	{
		damage = 40;
	}
	else if (count == 4)
	{
		damage = 35;
	}
	else
	{
		damage = 30;
	}

	for (i = 0; i < count; i++ )
	{
		// create a range of different velocities
		vel = BOWCASTER_VELOCITY * ( crandom() * BOWCASTER_VEL_RANGE + 1.0f );

		vectoangles( forward, angs );

		// add some slop to the alt-fire direction
		angs[PITCH] += crandom() * ( ent->client->ps.stats[STAT_ACCURACY] * weaponData[ent->client->ps.weapon].accuracyThreshold );
		angs[YAW] += crandom() * ( ent->client->ps.stats[STAT_ACCURACY] * weaponData[ent->client->ps.weapon].accuracyThreshold );
		
		AngleVectors( angs, dir, NULL, NULL );

		missile = CreateMissile( muzzle, dir, vel, 10000, ent, qtrue );

		missile->classname = "bowcaster_alt_proj";
		missile->s.weapon = WP_BOWCASTER;

		VectorSet( missile->r.maxs, BOWCASTER_SIZE, BOWCASTER_SIZE, BOWCASTER_SIZE );
		VectorScale( missile->r.maxs, -1, missile->r.mins );

		missile->damage = damage;
		missile->dflags = DAMAGE_DEATH_KNOCKBACK;
		missile->methodOfDeath = MOD_BOWCASTER;
		missile->clipmask = MASK_SHOT | CONTENTS_LIGHTSABER;

		// we don't want it to bounce
		missile->bounceCount = 0;
	}
}*/

//The above is never called.

//---------------------------------------------------------
void WF_FireJumbo( gentity_t *ent, qboolean altFire )
//---------------------------------------------------------
{
	vec3_t	dir, angs;
	vec3_t	start;
	VectorCopy(wp_muzzle, start);
//	int		accuracy	=	ent->client->ps.stats[STAT_ACCURACY];

	vectoangles( wp_forward, angs );

	if ( altFire )
	{
		// add some slop to the alt-fire direction
		//angs[PITCH] += crandom() * BLASTER_SPREAD;
		//angs[YAW]	+= crandom() * BLASTER_SPREAD;
	}
	angs[PITCH] += crandom() * ( ent->client->ps.stats[STAT_ACCURACY] * weaponData[ent->client->ps.weapon].accuracyThreshold );
	angs[YAW] += crandom() * ( ent->client->ps.stats[STAT_ACCURACY] * weaponData[ent->client->ps.weapon].accuracyThreshold );

	AngleVectors( angs, dir, NULL, NULL );

	// FIXME: if temp_org does not have clear trace to inside the bbox, don't shoot!
	//WP_FireBlasterMissile( ent, muzzle, dir, altFire );
/*	if ( altFire )
	{
		WP_BowcasterAltFire( ent, muzzle, dir );
	}
	else
	{*/
	if(ent->client)
	{
		if(ent->client->ps.zoomMode == 1 || ent->client->pers.cmd.buttons & BUTTON_SIGHTS /*&& weaponNum < WP_AUXILIARY1*/)
		{
			VectorCopy( ent->client->ps.origin, start );
			start[2] += ent->client->ps.viewheight;//By eyes
		}
	}
	WP_JumboAltFire( ent, start, dir, altFire );
	//}
}