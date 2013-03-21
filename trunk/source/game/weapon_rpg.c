#include "g_local.h"
#include "g_weapon.h"

extern vec3_t	wp_forward, wp_vright, wp_up;
extern vec3_t	wp_muzzle;
/*
======================================================================

ROCKET LAUNCHER

======================================================================
*/
void RocketDie(gentity_t *self, gentity_t *inflictor, gentity_t *attacker, int damage, int mod)
{
	self->die = 0;
	self->r.contents = 0;

	G_ExplodeMissile( self );

	level.explodeTime = level.time + 15000;

	self->think = G_FreeEntity;
	self->nextthink = level.time;
}
//---------------------------------------------------------
void rocketThink( gentity_t *ent )
//---------------------------------------------------------
{
	vec3_t newdir, targetdir, 
			up={0,0,1}, right; 
	vec3_t	org;
	float dot, dot2, dis;
	int i;
	float vel = (ent->spawnflags&1)?ent->speed:ROCKET_VELOCITY;

	if ( ent->genericValue1 && ent->genericValue1 < level.time )
	{//time's up, we're done, remove us
		if ( ent->genericValue2 )
		{//explode when die
			RocketDie( ent, &g_entities[ent->r.ownerNum], &g_entities[ent->r.ownerNum], 0, MOD_UNKNOWN );
		}
		else
		{//just remove when die
			G_FreeEntity( ent );
		}
		return;
	}
	if ( !ent->enemy 
		|| !ent->enemy->client 
		|| ent->enemy->health <= 0 
		|| ent->enemy->client->ps.powerups[PW_CLOAKED] )
	{//no enemy or enemy not a client or enemy dead or enemy cloaked
		if ( !ent->genericValue1  )
		{//doesn't have its own self-kill time
			ent->nextthink = level.time + 10000;
			ent->think = G_FreeEntity;
		}
		return;
	}

	if ( (ent->spawnflags&1) )
	{//vehicle rocket
		if ( ent->enemy->client && ent->enemy->client->NPC_class == CLASS_VEHICLE )
		{//tracking another vehicle
			if ( ent->enemy->client->ps.speed+4000 > vel )
			{
				vel = ent->enemy->client->ps.speed+4000;
			}
		}
	}

	if ( ent->enemy && ent->enemy->inuse )
	{	
		float newDirMult = ent->angle?ent->angle*2.0f:1.0f;
		float oldDirMult = ent->angle?(1.0f-ent->angle)*2.0f:1.0f;

		VectorCopy( ent->enemy->r.currentOrigin, org );
		org[2] += (ent->enemy->r.mins[2] + ent->enemy->r.maxs[2]) * 0.5f;

		VectorSubtract( org, ent->r.currentOrigin, targetdir );
		VectorNormalize( targetdir );

		// Now the rocket can't do a 180 in space, so we'll limit the turn to about 45 degrees.
		dot = DotProduct( targetdir, ent->movedir );
		if ( (ent->spawnflags&1) )
		{//vehicle rocket
			if ( ent->radius > -1.0f )
			{//can lose the lock if DotProduct drops below this number
				if ( dot < ent->radius )
				{//lost the lock!!!
					//HMM... maybe can re-lock on if they come in front again?
					/*
					//OR: should it stop trying to lock altogether?
					if ( ent->genericValue1 )
					{//have a timelimit, set next think to that
						ent->nextthink = ent->genericValue1;
						if ( ent->genericValue2 )
						{//explode when die
							ent->think = G_ExplodeMissile;
						}
						else
						{
							ent->think = G_FreeEntity;
						}
					}
					else
					{
						ent->think = NULL;
						ent->nextthink = -1;
					}
					*/
					return;
				}
			}
		}


		// a dot of 1.0 means right-on-target.
		if ( dot < 0.0f )
		{	
			// Go in the direction opposite, start a 180.
			CrossProduct( ent->movedir, up, right );
			dot2 = DotProduct( targetdir, right );

			if ( dot2 > 0 )
			{	
				// Turn 45 degrees right.
				VectorMA( ent->movedir, 0.4f*newDirMult, right, newdir );
			}
			else
			{	
				// Turn 45 degrees left.
				VectorMA( ent->movedir, -0.4f*newDirMult, right, newdir );
			}

			// Yeah we've adjusted horizontally, but let's split the difference vertically, so we kinda try to move towards it.
			newdir[2] = ( (targetdir[2]*newDirMult) + (ent->movedir[2]*oldDirMult) ) * 0.5;

			// let's also slow down a lot
			vel *= 0.5f;
		}
		else if ( dot < 0.70f )
		{	
			// Still a bit off, so we turn a bit softer
			VectorMA( ent->movedir, 0.5f*newDirMult, targetdir, newdir );
		}
		else
		{	
			// getting close, so turn a bit harder
			VectorMA( ent->movedir, 0.9f*newDirMult, targetdir, newdir );
		}

		// add crazy drunkenness
		for (i = 0; i < 3; i++ )
		{
			newdir[i] += crandom() * ent->random * 1.25f;
		}

		// decay the randomness
		ent->random *= 0.9f;

		if ( ent->enemy->client
			&& ent->enemy->client->ps.groundEntityNum != ENTITYNUM_NONE )
		{//tracking a client who's on the ground, aim at the floor...?
			// Try to crash into the ground if we get close enough to do splash damage
			dis = Distance( ent->r.currentOrigin, org );

			if ( dis < 128 )
			{
				// the closer we get, the more we push the rocket down, heh heh.
				newdir[2] -= (1.0f - (dis / 128.0f)) * 0.6f;
			}
		}

		VectorNormalize( newdir );

		VectorScale( newdir, vel * 0.5f, ent->s.pos.trDelta );
		VectorCopy( newdir, ent->movedir );
		SnapVector( ent->s.pos.trDelta );			// save net bandwidth
		VectorCopy( ent->r.currentOrigin, ent->s.pos.trBase );
		ent->s.pos.trTime = level.time;
	}

	ent->nextthink = level.time + ROCKET_ALT_THINK_TIME;	// Nothing at all spectacular happened, continue.
	return;
}

void rpgThink( gentity_t *ent )
{
	int i;
	int multiplier;

	switch(ent->genericValue1)
	{
		case ROCKETFIRE_DRUNK:
			ent->s.pos.trDelta[0] += crandom() * 4.0f;
			ent->s.pos.trDelta[1] += crandom() * 4.0f;
			ent->s.pos.trDelta[2] += crandom() * 4.0f;
			ent->nextthink = level.time + 100;
			break;
		case ROCKETFIRE_PERFECT:
			ent->nextthink = level.time + 100;
			break;
		case ROCKETFIRE_VEER:
			for(i = 0; i < ent->genericValue5; i++)
			{
				if(i == 0)
				{
					if(ent->genericValue2 == 1)
						multiplier = -1;
					else
						multiplier = 1;
				}
				else if(i == 1)
				{
					if(ent->genericValue3 == 1)
						multiplier = -1;
					else
						multiplier = 1;
				}
				else
				{
					if(ent->genericValue4 == 1)
						multiplier = -1;
					else
						multiplier = 1;
				}
				ent->s.pos.trDelta[i] += (multiplier * (5.0f - ent->movedir[i]));
				ent->movedir[i] += (multiplier * -1) * 2.0f;
				ent->nextthink = level.time + 40;
			}
			break;
	}
}

extern void G_ExplodeMissile( gentity_t *ent );

//---------------------------------------------------------
static void WP_ShootBoomBoom( gentity_t *ent, vec3_t start, vec3_t dir, qboolean altFire ){
	weaponData_t	*wp				= &weaponData[ent->s.weapon];
	int	damage	= altFire ? wp->altDamage : wp->damage;
	int	vel = altFire ? wp->altBulletSpeed : wp->bulletSpeed;
	int dif = 0;
	float rTime;
	gentity_t *missile;
	vec3_t end;
	trace_t tr;
//	vec3_t	dir, angs;
//	int		accuracy	=	ent->client->ps.stats[STAT_ACCURACY];

	if ( altFire )
	{
		vel *= 0.5f;
	}


	missile = CreateMissile( start, dir, vel, 10000, ent, altFire );
	//ent->random = 1.0f;

	missile->classname = "rocket_proj";
	missile->s.weapon = WP_ROCKET_LAUNCHER;

	//CHOOSE YOUR FATE
	if(Q_irand(1,2) == 1)
	{
		missile->genericValue1 = ROCKETFIRE_DRUNK;
	}
	else
	{
		if(Q_irand(1,2) == 1)
		{
			missile->genericValue1 = ROCKETFIRE_PERFECT;
		}
		else
		{
			missile->genericValue1 = ROCKETFIRE_VEER;
			missile->genericValue5 = Q_irand(1,3);
			missile->genericValue2 = Q_irand(1,2);
			missile->genericValue3 = Q_irand(1,2);
			missile->genericValue4 = Q_irand(1,2);
		}
	}

	// Make it easier to hit things
	VectorSet( missile->r.maxs, ROCKET_SIZE, ROCKET_SIZE, ROCKET_SIZE );
	VectorScale( missile->r.maxs, -1, missile->r.mins );
	VectorMA( start, 8192.0f, wp_forward, end );

	trap_Trace(&tr, start, missile->r.mins, missile->r.maxs, end, ent->s.number, MASK_SHOT);
	VectorCopy(tr.endpos, missile->pos3);

	if (ent->client && ent->client->ps.rocketLockIndex != ENTITYNUM_NONE)
	{
		float lockTimeInterval = ((g_gametype.integer==GT_SIEGE)?2400.0f:1200.0f)/16.0f;
		rTime = ent->client->ps.rocketLockTime;

		if (rTime == -1)
		{
			rTime = ent->client->ps.rocketLastValidTime;
		}
		dif = ( level.time - rTime ) / lockTimeInterval;

		if (dif < 0)
		{
			dif = 0;
		}

		//It's 10 even though it locks client-side at 8, because we want them to have a sturdy lock first, and because there's a slight difference in time between server and client
		if ( dif >= 10 && rTime != -1 )
		{
			missile->enemy = &g_entities[ent->client->ps.rocketLockIndex];

			if (missile->enemy && missile->enemy->client && missile->enemy->health > 0 && !OnSameTeam(ent, missile->enemy))
			{ //if enemy became invalid, died, or is on the same team, then don't seek it
				missile->angle = 0.5f;
				missile->think = rocketThink;
				missile->nextthink = level.time + ROCKET_ALT_THINK_TIME;
			}
		}

		ent->client->ps.rocketLockIndex = ENTITYNUM_NONE;
		ent->client->ps.rocketLockTime = 0;
		ent->client->ps.rocketTargetTime = 0;
	}
	else
	{
		missile->think = rpgThink;
		missile->nextthink = level.time + 100;
	}

	missile->damage = damage;
	missile->dflags = DAMAGE_DEATH_KNOCKBACK;
	if (altFire)
	{
		missile->methodOfDeath = MOD_ROCKET_HOMING;
		missile->splashMethodOfDeath = MOD_ROCKET_HOMING_SPLASH;
	}
	else
	{
		missile->methodOfDeath = MOD_ROCKET;
		missile->splashMethodOfDeath = MOD_ROCKET_SPLASH;
	}
//===testing being able to shoot rockets out of the air==================================
	missile->health = 10;
	missile->takedamage = qtrue;
	missile->r.contents = MASK_SHOT;
	missile->die = RocketDie;
//===testing being able to shoot rockets out of the air==================================
	
	missile->clipmask = MASK_SHOT;
	missile->splashDamage = ROCKET_SPLASH_DAMAGE;
	missile->splashRadius = ROCKET_SPLASH_RADIUS;

	// we don't want it to ever bounce
	missile->bounceCount = 0;
	//missile->nextthink = level.time + 100;
}


//---------------------------------------------------------
void WP_FireRocket( gentity_t *ent, qboolean altFire )
//---------------------------------------------------------
{
	/*int	damage	= ROCKET_DAMAGE;
	int	vel = ROCKET_VELOCITY;
	int dif = 0;
	float rTime;
	gentity_t *missile;*/
	vec3_t	dir, angs;

	vectoangles( wp_forward, angs );

	angs[PITCH] += crandom() * ( ent->client->ps.stats[STAT_ACCURACY] * weaponData[ent->client->ps.weapon].accuracyThreshold );
	angs[YAW] += crandom() * ( ent->client->ps.stats[STAT_ACCURACY] * weaponData[ent->client->ps.weapon].accuracyThreshold );

	AngleVectors( angs, dir, NULL, NULL );

	if ( altFire )
		WP_ShootBoomBoom(ent, wp_muzzle, dir, altFire);
	else
		WP_ShootBoomBoom(ent, wp_muzzle, dir, qfalse);
}