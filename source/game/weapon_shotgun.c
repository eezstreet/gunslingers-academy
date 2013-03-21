#include "g_local.h"
#include "g_weapon.h"

extern vec3_t	wp_forward, wp_vright, wp_up;
extern vec3_t	wp_muzzle;
#ifdef AKIMBO
extern vec3_t	wp_muzzle2;
#endif
extern gentity_t *ent_list[MAX_GENTITIES-1];
/*
======================================================================

TACTICAL SHOTGUN

======================================================================
*/

//---------------------------------------------------------
static void WP_FlechetteShotThink( gentity_t *ent )
{
	ent->damage -= 1;
	if(ent->damage <= 0)
		ent->think = G_FreeEntity;
	ent->nextthink = level.time + 100;
}

//---------------------------------------------------------
#ifdef AKIMBO
static void WP_FlechetteMainFire( gentity_t *ent, qboolean akimboGun )
#else
static void WP_FlechetteMainFire( gentity_t *ent )
#endif
//---------------------------------------------------------
{
#ifdef AKIMBO
	weaponData_t	*wp;
#else
	weaponData_t	*wp				= &weaponData[ent->s.weapon];
#endif
	vec3_t		fwd, angs;
	gentity_t	*missile;
	int i;
	int damage;
	vec3_t start;
#ifdef AKIMBO
	if(akimboGun)
	{
		VectorCopy(wp_muzzle2, start);
		wp = &weaponData[ent->s.weapon2];
	}
	else
	{
		VectorCopy(wp_muzzle, start);
		wp = &weaponData[ent->s.weapon];
	}
#else
	VectorCopy(wp_muzzle, start);
#endif
	if(ent->client)
	{
		if(ent->client->ps.zoomMode == 1 || ent->client->pers.cmd.buttons & BUTTON_SIGHTS /*&& weaponNum < WP_AUXILIARY1*/)
		{
			VectorCopy( ent->client->ps.origin, start );
			start[2] += ent->client->ps.viewheight;//By eyes
		}
	}


	for (i = 0; i < wp->numShots; i++ )
	{
		vectoangles( wp_forward, angs );

		if (i != 0)
		{ //do nothing on the first shot, it will hit the crosshairs
			angs[PITCH] += crandom() * wp->spread;
			angs[YAW]	+= crandom() * wp->spread;
		}
		if(g_instagib.integer == 1)
			damage = 3000;

		AngleVectors( angs, fwd, NULL, NULL );
		missile = CreateMissile( start, fwd, wp->bulletSpeed, 10000, ent, qfalse);

		missile->classname = "flech_proj";
#ifdef AKIMBO
		missile->s.weapon = (akimboGun) ? ent->s.weapon2 : ent->s.weapon;
#else
		missile->s.weapon = ent->s.weapon;
#endif

		VectorSet( missile->r.maxs, FLECHETTE_SIZE, FLECHETTE_SIZE, FLECHETTE_SIZE );
		VectorScale( missile->r.maxs, -1, missile->r.mins );

		missile->damage = wp->damage;
		missile->dflags = DAMAGE_DEATH_KNOCKBACK;
		missile->methodOfDeath = wp->methodOfDeath;
		missile->clipmask = MASK_SHOT | CONTENTS_LIGHTSABER;
		missile->think = WP_FlechetteShotThink;
		missile->nextthink = level.time + 100;

	}
}
//---------------------------------------------------------
#ifdef AKIMBO
static void WP_FlechetteAltFire( gentity_t *ent, qboolean akimboGun )
#else
static void WP_FlechetteAltFire( gentity_t *ent )
#endif
//---------------------------------------------------------
{
#ifdef AKIMBO
	weaponData_t	*wp;
#else
	weaponData_t	*wp				= &weaponData[ent->s.weapon];
#endif
	vec3_t		fwd, angs;
	gentity_t	*missile;
	int i;
	int damage;// = wp->altDamage;

#ifdef AKIMBO
	if(akimboGun)
	{
		wp = &weaponData[ent->s.weapon2];
	}
	else
	{
		wp = &weaponData[ent->s.weapon];
	}
#endif
	damage = wp->altDamage;
	for (i = 0; i < wp->altNumShots; i++ )
	{
		vectoangles( wp_forward, angs );

		if (i != 0)
		{ //do nothing on the first shot, it will hit the crosshairs
			angs[PITCH] += crandom() * wp->altSpread;
			angs[YAW]	+= crandom() * wp->altSpread;
		}
		if(g_instagib.integer == 1)
			damage = 3000;

		AngleVectors( angs, fwd, NULL, NULL );
#ifndef AKIMBO
		missile = CreateMissile( wp_muzzle, fwd, FLECHETTE_VEL, 10000, ent, qfalse);
#else
		if(akimboGun)
		{
			missile = CreateMissile( wp_muzzle2, fwd, wp->altBulletSpeed, 10000, ent, qfalse);
		}
		else
		{
			missile = CreateMissile( wp_muzzle, fwd, wp->altBulletSpeed, 10000, ent, qfalse);
		}
#endif

		missile->classname = "flech_proj";
		missile->s.weapon = ent->s.weapon;

		VectorSet( missile->r.maxs, FLECHETTE_SIZE, FLECHETTE_SIZE, FLECHETTE_SIZE );
		VectorScale( missile->r.maxs, -1, missile->r.mins );

		missile->damage = damage;
		missile->dflags = DAMAGE_DEATH_KNOCKBACK;
		missile->methodOfDeath = wp->methodOfDeath;
		missile->clipmask = MASK_SHOT | CONTENTS_LIGHTSABER;

	}
}
//---------------------------------------------------------
#ifdef AKIMBO
void WP_FireFlechette( gentity_t *ent, qboolean altFire, qboolean primWeapon )
#else
void WP_FireFlechette( gentity_t *ent, qboolean altFire )
#endif
//---------------------------------------------------------
{
#ifdef AKIMBO
	if(primWeapon)
	{
		if(altFire){
			WP_FlechetteAltFire( ent, qfalse );
		}
		else{
			WP_FlechetteMainFire( ent, qfalse );
		}
	}
	else
	{
		if(altFire){
			WP_FlechetteAltFire( ent, qtrue );
		}
		else{
			WP_FlechetteMainFire( ent, qtrue );
		}
	}
#else
	if(altFire){
		WP_FlechetteAltFire( ent );
	}
	else{
		WP_FlechetteMainFire( ent );
	}
#endif
}