#include "g_local.h"
#include "g_weapon.h"

extern vec3_t	wp_forward, wp_vright, wp_up;
extern vec3_t	wp_muzzle;
extern void W_TraceSetStart( gentity_t *ent, vec3_t start, vec3_t mins, vec3_t maxs );
extern int G_GetHitLocation(gentity_t *target, vec3_t ppoint);
/*
======================================================================

DISRUPTOR

======================================================================
*/
//---------------------------------------------------------
static void WP_DisruptorMainFire( gentity_t *ent )
//---------------------------------------------------------
{
	vec3_t		fwd, angs;
	gentity_t	*missile;
	weaponData_t	*wp				= &weaponData[ent->client->ps.weapon];
	int damage = wp->damage;

	vectoangles( wp_forward, angs );

	if(ent->client->ps.zoomMode != 1)
	{
		angs[PITCH] += crandom() * ( ent->client->ps.stats[STAT_ACCURACY] * weaponData[ent->client->ps.weapon].accuracyThreshold );
		angs[YAW] += crandom() * ( ent->client->ps.stats[STAT_ACCURACY] * weaponData[ent->client->ps.weapon].accuracyThreshold );
	}
	if(g_instagib.integer == 1)
		damage = 3000;

	AngleVectors( angs, fwd, NULL, NULL );

	missile = CreateMissile( wp_muzzle, fwd, wp->bulletSpeed, 10000, ent, qfalse);

	missile->classname = "flech_proj";
	missile->s.weapon = WP_DISRUPTOR;

	VectorSet( missile->r.maxs, FLECHETTE_SIZE, FLECHETTE_SIZE, FLECHETTE_SIZE );
	VectorScale( missile->r.maxs, -1, missile->r.mins );

	missile->damage = damage;
	missile->dflags = DAMAGE_DEATH_KNOCKBACK;
	missile->methodOfDeath = wp->methodOfDeath;
	missile->clipmask = MASK_SHOT | CONTENTS_LIGHTSABER;

}

//---------------------------------------------------------
static void WP_DisruptorAltFire( gentity_t *ent )
//---------------------------------------------------------
{
	vec3_t		fwd, angs, start;
	gentity_t	*missile;
	weaponData_t	*wp				= &weaponData[ent->client->ps.weapon];
	int damage = wp->altDamage;

	vectoangles( wp_forward, angs );

	angs[PITCH] += crandom() * wp->altSpread;
	angs[YAW] += crandom() * wp->altSpread;
	if(g_instagib.integer == 1)
		damage = 3000;

	AngleVectors( angs, fwd, NULL, NULL );

	VectorCopy(wp_muzzle, start);

	if(ent->client)
	{
		if(ent->client->ps.zoomMode == 1 /*|| ent->client->pers.cmd.buttons & BUTTON_SIGHTS*/)
		{
			VectorCopy( ent->client->ps.origin, start );
			start[2] += ent->client->ps.viewheight;//By eyes
		}
	}

	missile = CreateMissile( start, fwd, wp->altBulletSpeed, 10000, ent, qfalse);

	missile->classname = "flech_proj";
	missile->s.weapon = ent->s.weapon;

	VectorSet( missile->r.maxs, FLECHETTE_SIZE, FLECHETTE_SIZE, FLECHETTE_SIZE );
	VectorScale( missile->r.maxs, -1, missile->r.mins );

	missile->damage = damage;
	missile->dflags = DAMAGE_DEATH_KNOCKBACK;
	missile->methodOfDeath = wp->altMethodOfDeath;
	missile->clipmask = MASK_SHOT | CONTENTS_LIGHTSABER;
}


//---------------------------------------------------------
void WP_FireDisruptor( gentity_t *ent, qboolean altFire )
//---------------------------------------------------------
{
	if (!ent || !ent->client || ent->client->ps.zoomMode != 1)
	{ //do not ever let it do the alt fire when not zoomed
		altFire = qfalse;
	}

	if (ent && ent->s.eType == ET_NPC && !ent->client)
	{ //special case for animents
		WP_DisruptorAltFire( ent );
		return;
	}

	if ( altFire )
	{
		WP_DisruptorAltFire( ent );
	}
	else
	{
		WP_DisruptorMainFire( ent );
	}
}