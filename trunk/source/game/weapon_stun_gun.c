#include "g_local.h"
#include "g_weapon.h"

extern vec3_t	wp_forward, wp_vright, wp_up;
extern vec3_t	wp_muzzle;
//---------------------------------------------------------
// FireStunBaton
//---------------------------------------------------------


void WP_FireStunBatonMain( gentity_t *ent )
{
	weaponData_t	*wp				= &weaponData[ent->s.weapon];
	gentity_t	*tr_ent;
	trace_t		tr;
	vec3_t		mins, maxs, end;
	vec3_t		muzzleStun;


	if (!ent->client)
	{
		VectorCopy(ent->r.currentOrigin, muzzleStun);
		muzzleStun[2] += 8;
	}
	else
	{
		VectorCopy(ent->client->ps.origin, muzzleStun);
		muzzleStun[2] += ent->client->ps.viewheight-6;
	}

	VectorMA(muzzleStun, 20.0f, wp_forward, muzzleStun);
	VectorMA(muzzleStun, 4.0f, wp_vright, muzzleStun);

	VectorMA( muzzleStun, STUN_BATON_RANGE, wp_forward, end );

	VectorSet( maxs, 6, 6, 6 );
	VectorScale( maxs, -1, mins );

	trap_Trace ( &tr, muzzleStun, mins, maxs, end, ent->s.number, MASK_SHOT );

	if ( tr.entityNum >= ENTITYNUM_WORLD )
	{
		return;
	}

	tr_ent = &g_entities[tr.entityNum];

	if (tr_ent && tr_ent->takedamage && tr_ent->client)
	{ //see if either party is involved in a duel
		if (tr_ent->client->ps.duelInProgress &&
			tr_ent->client->ps.duelIndex != ent->s.number)
		{
			return;
		}

		if (ent->client &&
			ent->client->ps.duelInProgress &&
			ent->client->ps.duelIndex != tr_ent->s.number)
		{
			return;
		}
	}

	if ( tr_ent && tr_ent->takedamage )
	{
		G_PlayEffect( EFFECT_STUNHIT, tr.endpos, tr.plane.normal );

		G_Sound( tr_ent, CHAN_WEAPON, G_SoundIndex( va("sound/weapons/melee/punch%d", Q_irand(1, 4)) ) );
		G_Damage( tr_ent, ent, ent, wp_forward, tr.endpos, wp->damage, (DAMAGE_NO_KNOCKBACK|DAMAGE_HALF_ABSORB), wp->methodOfDeath );

		if (tr_ent->client)
		{ //if it's a player then use the shock effect
			if ( tr_ent->client->NPC_class == CLASS_VEHICLE )
			{//not on vehicles
				if ( !tr_ent->m_pVehicle
					|| tr_ent->m_pVehicle->m_pVehicleInfo->type == VH_ANIMAL 
					|| tr_ent->m_pVehicle->m_pVehicleInfo->type == VH_FLIER )
				{//can zap animals
					tr_ent->client->ps.electrifyTime = level.time + Q_irand( 3000, 4000 );
				}
			}
			else
			{
				tr_ent->client->ps.electrifyTime = level.time + 700;
			}
		}
	}
}

//---------------------------------------------------------

extern void G_ExplodeMissile( gentity_t *ent );
extern VectorNPos(vec3_t in, vec3_t out);

#define TASER_RANGE	384

void BG_SetAnimFinal(playerState_t *ps, animation_t *animations, int setAnimParts,int anim,int setAnimFlags, int blendTime);
extern void G_GetDismemberLoc(gentity_t *self, vec3_t boltPoint, int limbType);
void StunGunAttachedThink(gentity_t *ent)
{
	vec3_t targetOrigin;
	G_GetDismemberLoc( ent->target_ent, targetOrigin, ent->genericValue2 );
	VectorCopy( targetOrigin, ent->s.pos.trBase );
	VectorCopy( targetOrigin, ent->r.currentOrigin);
	if(ent->parent->taser_signal)
	{
		ent->parent->taser_signal = qfalse;
		G_PlayEffectID(G_EffectIndex("taser/cartridge_shock"), ent->r.currentOrigin, ent->target_ent->r.currentAngles);
		G_Damage(ent->target_ent, ent->parent, ent->parent, NULL, NULL, weaponData[ent->s.weapon].damage, DAMAGE_NO_KNOCKBACK, MOD_STUN_BATON);
		//Mega hax->make our enemies cower and become unable to attack!
		if(ent->target_ent->client && ent->target_ent->s.number < MAX_CLIENTS)
		{
			BG_SetAnimFinal(&ent->target_ent->client->ps, bgHumanoidAnimations, SETANIM_TORSO, Q_irand(BOTH_PAIN1, BOTH_PAIN18), SETANIM_FLAG_HOLD, 150);
			ent->target_ent->client->ps.weaponTime += 2000;
			ent->target_ent->client->ps.weaponTime2 += 2000;
			ent->target_ent->client->ps.electrifyTime = level.time + 1000;
		}
		else if(ent->target_ent->NPC)
		{
			ent->target_ent->client->ps.electrifyTime = level.time + 1000;
		}
	}
	else if(VectorDistance(ent->r.currentOrigin, ent->parent->r.currentOrigin) >= TASER_RANGE || ent->genericValue1 <= level.time) //Range
	{
		ent->parent->taser_attach = qfalse;
		ent->parent->client->ps.ammo[weaponData[ent->s.weapon].ammoIndex] -= weaponData[ent->s.weapon].altEnergyPerShot;
		G_PlayEffectID(G_EffectIndex("taser/cartridge_detach_body"), ent->r.currentOrigin, ent->target_ent->r.currentAngles);
		G_Damage(ent->target_ent, ent->parent, ent->parent, NULL, NULL, 45, DAMAGE_NO_KNOCKBACK, MOD_STUN_BATON);
		G_ExplodeMissile(ent);
	}
	ent->nextthink = level.time + 50;
}

extern int G_GetHitQuad( gentity_t *self, vec3_t hitloc );
void StunGunLoadedAttach(gentity_t *self, gentity_t *other, trace_t *trace)
{
	gentity_t *tent;
	if ( other 
		&& (other->flags&FL_BBRUSH)
		&& other->s.pos.trType == TR_STATIONARY
		&& other->s.apos.trType == TR_STATIONARY )
	{//a perfectly still breakable brush, let us attach directly to it!
		self->target_ent = other;//remember them when we blow up
	}
	else if ( other 
		&& other->s.number < ENTITYNUM_WORLD
		&& other->s.eType == ET_MOVER
		&& trace->plane.normal[2] > 0 )
	{//stick to it?
		self->s.groundEntityNum = other->s.number;
	}
	self->target_ent = other;
	self->touch = 0;
	self->think = StunGunAttachedThink;
	self->parent->taser_attach = qtrue;
	self->nextthink = level.time + 50;
	self->s.saberInFlight = qtrue;

	G_PlayEffectID(G_EffectIndex("taser/cartridge_attach_body"), self->r.currentOrigin, self->r.currentAngles);

	VectorClear(self->s.apos.trDelta);
	self->s.apos.trType = TR_STATIONARY;

	self->s.pos.trType = TR_STATIONARY;
	VectorCopy( self->r.currentOrigin, self->s.origin );
	VectorCopy( self->r.currentOrigin, self->s.pos.trBase );
	VectorClear( self->s.pos.trDelta );

	VectorClear( self->s.apos.trDelta );

	VectorNormalize(trace->plane.normal);

	vectoangles(trace->plane.normal, self->s.angles);
	VectorCopy(self->s.angles, self->r.currentAngles );
	VectorCopy(self->s.angles, self->s.apos.trBase);

	VectorCopy(trace->plane.normal, self->pos2);
	self->count = -1;
		
	tent = G_TempEntity( self->r.currentOrigin, EV_MISSILE_MISS );
	tent->s.weapon = 0;
	tent->parent = self;
	tent->r.ownerNum = self->s.number;

	//so that the owner can blow it up with projectiles
	self->r.svFlags |= SVF_OWNERNOTSHARED;

	self->taser_attach = qtrue;
	self->parent->client->ps.ammo[weaponData[self->s.weapon].ammoIndex] += weaponData[self->s.weapon].altEnergyPerShot;

	self->genericValue2 = G_GetHitQuad(other, self->r.currentOrigin);
}

void StunGunUnloadedAttach(gentity_t *self, gentity_t *other, trace_t *trace)
{
	gentity_t *tent;
	if ( other 
		&& (other->flags&FL_BBRUSH)
		&& other->s.pos.trType == TR_STATIONARY
		&& other->s.apos.trType == TR_STATIONARY )
	{//a perfectly still breakable brush, let us attach directly to it!
		self->target_ent = other;//remember them when we blow up
	}
	else if ( other 
		&& other->s.number < ENTITYNUM_WORLD
		&& other->s.eType == ET_MOVER
		&& trace->plane.normal[2] > 0 )
	{//stick to it?
		self->s.groundEntityNum = other->s.number;
	}
	self->touch = 0;
	self->think = StunGunAttachedThink;
	self->nextthink = level.time + 50;
	self->s.saberInFlight = qtrue;
	G_PlayEffectID(G_EffectIndex("taser/cartridge_attach_body"), self->r.currentOrigin, self->r.currentAngles);

	VectorClear(self->s.apos.trDelta);
	self->s.apos.trType = TR_STATIONARY;

	self->s.pos.trType = TR_STATIONARY;
	VectorCopy( self->r.currentOrigin, self->s.origin );
	VectorCopy( self->r.currentOrigin, self->s.pos.trBase );
	VectorClear( self->s.pos.trDelta );

	VectorClear( self->s.apos.trDelta );

	VectorNormalize(trace->plane.normal);

	vectoangles(trace->plane.normal, self->s.angles);
	VectorCopy(self->s.angles, self->r.currentAngles );
	VectorCopy(self->s.angles, self->s.apos.trBase);

	VectorCopy(trace->plane.normal, self->pos2);
	self->count = -1;
		
	tent = G_TempEntity( self->r.currentOrigin, EV_MISSILE_MISS );
	tent->s.weapon = 0;
	tent->parent = self;
	tent->r.ownerNum = self->s.number;

	//so that the owner can blow it up with projectiles
	self->r.svFlags |= SVF_OWNERNOTSHARED;
}

void StunGunDetach( gentity_t *ent )
{
	G_PlayEffectID(G_EffectIndex("taser/cartridge_shock"), ent->r.currentOrigin, ent->r.currentAngles);
	ent->flags &= ~FL_BOUNCE_SHRAPNEL;
	ent->s.pos.trType = TR_GRAVITY;
	ent->s.pos.trTime = level.time;
	VectorCopy( ent->r.currentOrigin, ent->s.pos.trBase );
	ent->physicsObject = qfalse;
	ent->think = G_ExplodeMissile;
	ent->nextthink = ent->genericValue1 + 5000;
	ent->touch = StunGunUnloadedAttach;
	ent->die = 0;
	ent->damage *= 2;
}

void StunGunThink( gentity_t *ent )
{
	if(VectorDistance(ent->r.currentOrigin, ent->parent->r.currentOrigin) >= TASER_RANGE) //Range
	{
		StunGunDetach(ent);
		return;
	}
	ent->nextthink = level.time + 25;
}


//---------------------------------------------------------
void WP_FireStunBatonAlt( gentity_t *ent )
//---------------------------------------------------------
{
	weaponData_t	*wp				= &weaponData[ent->s.weapon];
	int	damage	= wp->damage;

#define TASER_SPEED 1100
	gentity_t *missile;

	if(ent->taser_attach)
	{
		//Refund ammo, and shock the bastard
		ent->client->ps.ammo[wp->ammoIndex] += wp->altEnergyPerShot;
		ent->taser_signal = qtrue;
		return;
	}
	missile =	CreateMissile( wp_muzzle, wp_forward, wp->altBulletSpeed, 10000, ent, qtrue );

	missile->classname = "baton_alt_proj";
	missile->s.weapon = ent->s.weapon;

	VectorSet( missile->r.maxs, 2, 2, 2 );
	VectorScale( missile->r.maxs, -1, missile->r.mins );
	missile->damage = 10;
	missile->dflags = DAMAGE_DEATH_KNOCKBACK;
	missile->methodOfDeath = wp->methodOfDeath;
	missile->clipmask = MASK_SHOT | CONTENTS_LIGHTSABER;
	missile->physicsObject = qtrue;
	missile->flags |= FL_BOUNCE_SHRAPNEL;
	missile->touch = StunGunLoadedAttach;
	missile->genericValue1 = level.time + 30000;
	VectorSet( missile->r.maxs, ROCKET_SIZE, ROCKET_SIZE, ROCKET_SIZE );

	// we don't want it to bounce forever
	missile->bounceCount = 8;

	//This bitch needs a think function and a good die function to make it tase people properly
	missile->think = StunGunThink;
	missile->nextthink = level.time+50;
}
void WP_FireStunBaton( gentity_t *ent, qboolean alt_fire )
{
	if ( alt_fire )
	{
		//WP_FlechetteProxMine( ent );
		WP_FireStunBatonAlt( ent );
	}
	else
	{
		WP_FireStunBatonMain( ent );
	}
}