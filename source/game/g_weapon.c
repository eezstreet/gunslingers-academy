// Copyright (C) 1999-2000 Id Software, Inc.
//
// g_weapon.c 
// perform the server side effects of a weapon firing

#include "g_local.h"
#include "be_aas.h"
#include "bg_saga.h"
#include "../ghoul2/G2.h"
#include "q_shared.h"
#include "g_weapon.h"
//[WEAPONSDAT]
#include "bg_weaponsdat.h"
//[/WEAPONSDAT]

static	float	s_quadFactor;

vec3_t	wp_forward, wp_vright, wp_up;
vec3_t	wp_muzzle;
#ifdef AKIMBO
vec3_t	wp_muzzle2;
#endif


//externs
extern void WP_FireBryarPistol( gentity_t *ent, qboolean altFire );
extern void WP_FireBlasterMissile( gentity_t *ent, vec3_t start, vec3_t dir, qboolean altFire );
extern void WP_FireTurretMissile( gentity_t *ent, vec3_t start, vec3_t dir, qboolean altFire, int damage, int velocity, int mod, gentity_t *ignore );
extern void WP_FireGenericBlasterMissile( gentity_t *ent, vec3_t start, vec3_t dir, qboolean altFire, int damage, int velocity, int mod );
extern void WP_FireTurboLaserMissile( gentity_t *ent, vec3_t start, vec3_t dir );
extern void WP_FireEmplacedMissile( gentity_t *ent, vec3_t start, vec3_t dir, qboolean altFire, gentity_t *ignore );
extern void WP_FireBlaster( gentity_t *ent, qboolean altFire );
extern void WP_FireDisruptor( gentity_t *ent, qboolean altFire );
extern void WP_FireBowcaster( gentity_t *ent, qboolean altFire );
extern void WP_FireRepeater( gentity_t *ent, qboolean altFire );
extern void WP_FireDEMP2( gentity_t *ent, qboolean altFire );
extern void WP_FireFlechette( gentity_t *ent, qboolean altFire );
extern void WP_FireConcussionAlt( gentity_t *ent );
extern void WP_FireConcussion( gentity_t *ent, qboolean altFire );
extern void WP_FireRocket( gentity_t *ent, qboolean altFire );
extern void rocketThink( gentity_t *ent );
extern void RocketDie(gentity_t *self, gentity_t *inflictor, gentity_t *attacker, int damage, int mod);
extern gentity_t *WP_FireThermalDetonator( gentity_t *ent, qboolean altFire );
extern void WP_DropDetPack( gentity_t *ent, qboolean alt_fire );
extern gentity_t *WP_FireFlashbang( gentity_t *ent, qboolean altFire );
extern void WP_FireStunBaton( gentity_t *ent, qboolean alt_fire );
extern void WP_FireMelee( gentity_t *ent, qboolean alt_fire );

extern gentity_t *WP_FireSmokeGrenade( gentity_t *ent );
extern gentity_t *WP_FireGasGrenade( gentity_t *ent );
extern gentity_t *WP_FireStunGrenade( gentity_t *ent );
extern void WF_FireJumbo( gentity_t *ent, qboolean altFire );
//end externs


extern qboolean G_BoxInBounds( vec3_t point, vec3_t mins, vec3_t maxs, vec3_t boundsMins, vec3_t boundsMaxs );
extern qboolean G_HeavyMelee( gentity_t *attacker );
extern void Jedi_Decloak( gentity_t *self );

static void WP_FireEmplaced( gentity_t *ent, qboolean altFire );
//Poor old eezstreet was stubborn enough to get rid of this, little did he know it was still being used! Not for long anyway.
gentity_t *ent_list[MAX_GENTITIES-1];

void touch_NULL( gentity_t *ent, gentity_t *other, trace_t *trace )
{

}

void laserTrapExplode( gentity_t *self );
void RocketDie(gentity_t *self, gentity_t *inflictor, gentity_t *attacker, int damage, int mod);

//[Asteroids]
extern vmCvar_t		g_vehAutoAimLead;
//[/Asteroids]

//We should really organize weapon data into tables or parse from the ext data so we have accurate info for this,
float WP_SpeedOfMissileForWeapon( int wp, qboolean alt_fire )
{
	return 500;
}
#ifdef AKIMBO
void WP_SetAkimboWeapon( gentity_t *ent, int newWeapon, int weaponIndex)
{
	if(newWeapon == WP_NONE)
	{
		ent->client->isAkimbo[weaponIndex] = qfalse;
	}
	else
	{
		ent->client->isAkimbo[weaponIndex] = qtrue;
	}
	ent->client->akimboCounterPart[weaponIndex] = newWeapon;
	if(weaponIndex == ent->client->ps.weapon)
	{
		ent->client->ps.weapon2 = newWeapon;
	}
	G_AddEvent(ent, EV_RECEIVE_AKIMBO, newWeapon);
}
#endif

//-----------------------------------------------------------------------------
void WP_TraceSetStart( gentity_t *ent, vec3_t start, vec3_t mins, vec3_t maxs )
//-----------------------------------------------------------------------------
{
//make sure our start point isn't on the other side of a wall
	trace_t tr;
	vec3_t  entMins;
	vec3_t  entMaxs;

	VectorAdd( ent->r.currentOrigin, ent->r.mins, entMins );
	VectorAdd( ent->r.currentOrigin, ent->r.maxs, entMaxs );

	if ( G_BoxInBounds( start, mins, maxs, entMins, entMaxs ) )
	{
		return;
	}

	if ( !ent->client )
	{
		return;
	}

	trap_Trace( &tr, ent->client->ps.origin, mins, maxs, start, ent->s.number, MASK_SOLID|CONTENTS_SHOTCLIP );

	if ( tr.startsolid || tr.allsolid )
	{
		return;
	}

	if ( tr.fraction < 1.0f )
	{
		VectorCopy( tr.endpos, start );
	}
}

//-----------------------------------------------------------------------------
void W_TraceSetStart( gentity_t *ent, vec3_t start, vec3_t mins, vec3_t maxs )
//-----------------------------------------------------------------------------
{
	//make sure our start point isn't on the other side of a wall
	trace_t	tr;
	vec3_t	entMins;
	vec3_t	entMaxs;
	vec3_t	eyePoint;

	VectorAdd( ent->r.currentOrigin, ent->r.mins, entMins );
	VectorAdd( ent->r.currentOrigin, ent->r.maxs, entMaxs );

	if ( G_BoxInBounds( start, mins, maxs, entMins, entMaxs ) )
	{
		return;
	}

	if ( !ent->client )
	{
		return;
	}

	VectorCopy( ent->s.pos.trBase, eyePoint);
	eyePoint[2] += ent->client->ps.viewheight;
		
	trap_Trace( &tr, eyePoint, mins, maxs, start, ent->s.number, MASK_SOLID|CONTENTS_SHOTCLIP );

	if ( tr.startsolid || tr.allsolid )
	{
		return;
	}

	if ( tr.fraction < 1.0f )
	{
		VectorCopy( tr.endpos, start );
	}
}


/*
----------------------------------------------
	PLAYER WEAPONS
----------------------------------------------
*/


int G_GetHitLocation(gentity_t *target, vec3_t ppoint);


//[CoOp]
void WP_Explode( gentity_t *self )
{//make this entity explode
	gentity_t	*attacker = self;
	vec3_t		forward={0,0,1};

	// stop chain reaction runaway loops
	self->takedamage = qfalse;

	self->s.loopSound = 0;

	if ( !self->client )
	{
		AngleVectors( self->s.angles, forward, NULL, NULL );
	}

	/* RAFIXME - need to figure this thingy out.
	if ( self->fxID > 0 )
	{
		G_PlayEffect( self->fxID, self->r.currentOrigin, forward );
	}
	*/
	
	if ( self->s.owner )
	{
		attacker = &g_entities[self->s.owner];
	}
	else if ( self->activator )
	{
		attacker = self->activator;
	}

	if ( self->splashDamage > 0 && self->splashRadius > 0 )
	{ 
		G_RadiusDamage( self->r.currentOrigin, attacker, self->splashDamage, 
			self->splashRadius, self/*don't ignore attacker*/, self,
			/*RAFIXME - impliment this mod? MOD_EXPLOSIVE_SPLASH*/ MOD_ROCKET_SPLASH );
	}

	if ( self->target )
	{
		G_UseTargets( self, attacker );
	}

	G_SetOrigin( self, self->r.currentOrigin );

	self->nextthink = level.time + 50;
	self->think = G_FreeEntity;
}
//[/CoOp]


void WP_ExplosiveDie(gentity_t *self, gentity_t *inflictor, gentity_t *attacker, int damage, int mod)
{
	laserTrapExplode(self);
}

//---------------------------------------------------------
qboolean WP_LobFire( gentity_t *self, vec3_t start, vec3_t target, vec3_t mins, vec3_t maxs, int clipmask, 
				vec3_t velocity, qboolean tracePath, int ignoreEntNum, int enemyNum,
				float minSpeed, float maxSpeed, float idealSpeed, qboolean mustHit )
//---------------------------------------------------------
{ //for the galak mech NPC
	float	targetDist, shotSpeed, speedInc = 100, travelTime, impactDist, bestImpactDist = Q3_INFINITE;//fireSpeed, 
	vec3_t	targetDir, shotVel, failCase; 
	trace_t	trace;
	trajectory_t	tr;
	qboolean	blocked;
	int		elapsedTime, skipNum, timeStep = 500, hitCount = 0, maxHits = 7;
	vec3_t	lastPos, testPos;
	gentity_t	*traceEnt;
	
	if ( !idealSpeed )
	{
		idealSpeed = 300;
	}
	else if ( idealSpeed < speedInc )
	{
		idealSpeed = speedInc;
	}
	shotSpeed = idealSpeed;
	skipNum = (idealSpeed-speedInc)/speedInc;
	if ( !minSpeed )
	{
		minSpeed = 100;
	}
	if ( !maxSpeed )
	{
		maxSpeed = 900;
	}
	while ( hitCount < maxHits )
	{
		VectorSubtract( target, start, targetDir );
		targetDist = VectorNormalize( targetDir );

		VectorScale( targetDir, shotSpeed, shotVel );
		travelTime = targetDist/shotSpeed;
		shotVel[2] += travelTime * 0.5 * g_gravity.value;

		if ( !hitCount )		
		{//save the first (ideal) one as the failCase (fallback value)
			if ( !mustHit )
			{//default is fine as a return value
				VectorCopy( shotVel, failCase );
			}
		}

		if ( tracePath )
		{//do a rough trace of the path
			blocked = qfalse;

			VectorCopy( start, tr.trBase );
			VectorCopy( shotVel, tr.trDelta );
			tr.trType = TR_GRAVITY;
			tr.trTime = level.time;
			travelTime *= 1000.0f;
			VectorCopy( start, lastPos );
			
			//This may be kind of wasteful, especially on long throws... use larger steps?  Divide the travelTime into a certain hard number of slices?  Trace just to apex and down?
			for ( elapsedTime = timeStep; elapsedTime < floor(travelTime)+timeStep; elapsedTime += timeStep )
			{
				if ( (float)elapsedTime > travelTime )
				{//cap it
					elapsedTime = floor( travelTime );
				}
				BG_EvaluateTrajectory( &tr, level.time + elapsedTime, testPos );
				trap_Trace( &trace, lastPos, mins, maxs, testPos, ignoreEntNum, clipmask );

				if ( trace.allsolid || trace.startsolid )
				{
					blocked = qtrue;
					break;
				}
				if ( trace.fraction < 1.0f )
				{//hit something
					if ( trace.entityNum == enemyNum )
					{//hit the enemy, that's perfect!
						break;
					}
					else if ( trace.plane.normal[2] > 0.7 && DistanceSquared( trace.endpos, target ) < 4096 )//hit within 64 of desired location, should be okay
					{//close enough!
						break;
					}
					else
					{//FIXME: maybe find the extents of this brush and go above or below it on next try somehow?
						impactDist = DistanceSquared( trace.endpos, target );
						if ( impactDist < bestImpactDist )
						{
							bestImpactDist = impactDist;
							VectorCopy( shotVel, failCase );
						}
						blocked = qtrue;
						//see if we should store this as the failCase
						if ( trace.entityNum < ENTITYNUM_WORLD )
						{//hit an ent
							traceEnt = &g_entities[trace.entityNum];
							if ( traceEnt && traceEnt->takedamage && !OnSameTeam( self, traceEnt ) )
							{//hit something breakable, so that's okay
								//we haven't found a clear shot yet so use this as the failcase
								VectorCopy( shotVel, failCase );
							}
						}
						break;
					}
				}
				if ( elapsedTime == floor( travelTime ) )
				{//reached end, all clear
					break;
				}
				else
				{
					//all clear, try next slice
					VectorCopy( testPos, lastPos );
				}
			}
			if ( blocked )
			{//hit something, adjust speed (which will change arc)
				hitCount++;
				shotSpeed = idealSpeed + ((hitCount-skipNum) * speedInc);//from min to max (skipping ideal)
				if ( hitCount >= skipNum )
				{//skip ideal since that was the first value we tested
					shotSpeed += speedInc;
				}
			}
			else
			{//made it!
				break;
			}
		}
		else
		{//no need to check the path, go with first calc
			break;
		}
	}

	if ( hitCount >= maxHits )
	{//NOTE: worst case scenario, use the one that impacted closest to the target (or just use the first try...?)
		VectorCopy( failCase, velocity );
		return qfalse;
	}
	VectorCopy( shotVel, velocity );
	return qtrue;
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////


/*
======================
SnapVectorTowards

Round a vector to integers for more efficient network
transmission, but make sure that it rounds towards a given point
rather than blindly truncating.  This prevents it from truncating 
into a wall.
======================
*/
void SnapVectorTowards( vec3_t v, vec3_t to ) {
	int		i;

	for ( i = 0 ; i < 3 ; i++ ) {
		if ( to[i] <= v[i] ) {
			v[i] = (int)v[i];
		} else {
			v[i] = (int)v[i] + 1;
		}
	}
}


//======================================================================


/*
===============
LogAccuracyHit
===============
*/
qboolean LogAccuracyHit( gentity_t *target, gentity_t *attacker ) {
	if( !target->takedamage ) {
		return qfalse;
	}

	if ( target == attacker ) {
		return qfalse;
	}

	if( !target->client ) {
		return qfalse;
	}

	if (!attacker)
	{
		return qfalse;
	}

	if( !attacker->client ) {
		return qfalse;
	}

	if( target->client->ps.stats[STAT_HEALTH] <= 0 ) {
		return qfalse;
	}

	if ( OnSameTeam( target, attacker ) ) {
		return qfalse;
	}

	return qtrue;
}


/*
===============
CalcMuzzlePoint

set muzzle location relative to pivoting eye
rwwFIXMEFIXME: Since ghoul2 models are on server and properly updated now,
it may be reasonable to base muzzle point off actual weapon bolt point.
The down side would be that it does not necessarily look alright from a
first person perspective.
===============
*/
void CalcMuzzlePoint ( gentity_t *ent, vec3_t forward, vec3_t right, vec3_t up, vec3_t muzzlePoint ) 
{
	int weapontype;
	vec3_t muzzleOffPoint;

	weapontype = ent->s.weapon;
	VectorCopy( ent->s.pos.trBase, muzzlePoint );

	VectorCopy(WP_MuzzlePoint[weapontype], muzzleOffPoint);

	if (weapontype > WP_NONE && weapontype < WP_NUM_WEAPONS)
	{	// Use the table to generate the muzzlepoint;
		{	// Crouching.  Use the add-to-Z method to adjust vertically.
			VectorMA(muzzlePoint, muzzleOffPoint[0], forward, muzzlePoint);
			VectorMA(muzzlePoint, muzzleOffPoint[1], right, muzzlePoint);
			muzzlePoint[2] += ent->client->ps.viewheight + muzzleOffPoint[2];
		}
	}

	// snap to integer coordinates for more efficient network bandwidth usage
	SnapVector( muzzlePoint );
}
#ifdef AKIMBO
void CalcMuzzlePoint2( gentity_t *ent, vec3_t forward, vec3_t right, vec3_t up, vec3_t muzzlePoint ) 
{
	int weapontype;
	vec3_t muzzleOffPoint;

	weapontype = ent->s.weapon2;
	VectorCopy( ent->s.pos.trBase, muzzlePoint );

	VectorCopy(WP_MuzzlePoint2[weapontype], muzzleOffPoint);

	if (weapontype > WP_NONE && weapontype < WP_NUM_WEAPONS)
	{	// Use the table to generate the muzzlepoint;
		{	// Crouching.  Use the add-to-Z method to adjust vertically.
			VectorMA(muzzlePoint, muzzleOffPoint[0], forward, muzzlePoint);
			VectorMA(muzzlePoint, muzzleOffPoint[1], right, muzzlePoint);
			muzzlePoint[2] += ent->client->ps.viewheight + muzzleOffPoint[2];
		}
	}

	// snap to integer coordinates for more efficient network bandwidth usage
	SnapVector( muzzlePoint );
}
#endif

/*
===============
CalcMuzzlePointOrigin

set muzzle location relative to pivoting eye
===============
*/
void CalcMuzzlePointOrigin ( gentity_t *ent, vec3_t origin, vec3_t forward, vec3_t right, vec3_t up, vec3_t muzzlePoint ) {
	VectorCopy( ent->s.pos.trBase, muzzlePoint );
	muzzlePoint[2] += ent->client->ps.viewheight;
	VectorMA( muzzlePoint, 14, forward, muzzlePoint );
	// snap to integer coordinates for more efficient network bandwidth usage
	SnapVector( muzzlePoint );
}

extern void G_MissileImpact( gentity_t *ent, trace_t *trace );
void WP_TouchVehMissile( gentity_t *ent, gentity_t *other, trace_t *trace )
{
	trace_t	myTrace;
	memcpy( (void *)&myTrace, (void *)trace, sizeof(myTrace) );
	if ( other )
	{
		myTrace.entityNum = other->s.number;
	}
	G_MissileImpact( ent, &myTrace );
}

void WP_CalcVehMuzzle(gentity_t *ent, int muzzleNum)
{
	Vehicle_t *pVeh = ent->m_pVehicle;
	mdxaBone_t boltMatrix;
	vec3_t	vehAngles;

	assert(pVeh);

	if (pVeh->m_iMuzzleTime[muzzleNum] == level.time)
	{ //already done for this frame, don't need to do it again
		return;
	}
	//Uh... how about we set this, hunh...?  :)
	pVeh->m_iMuzzleTime[muzzleNum] = level.time;
	
	VectorCopy( ent->client->ps.viewangles, vehAngles );
	if ( pVeh->m_pVehicleInfo
		&& (pVeh->m_pVehicleInfo->type == VH_ANIMAL
			 ||pVeh->m_pVehicleInfo->type == VH_WALKER
			 ||pVeh->m_pVehicleInfo->type == VH_SPEEDER) )
	{
		vehAngles[PITCH] = vehAngles[ROLL] = 0;
	}

	trap_G2API_GetBoltMatrix_NoRecNoRot(ent->ghoul2, 0, pVeh->m_iMuzzleTag[muzzleNum], &boltMatrix, vehAngles,
		ent->client->ps.origin, level.time, NULL, ent->modelScale);
	BG_GiveMeVectorFromMatrix(&boltMatrix, ORIGIN, pVeh->m_vMuzzlePos[muzzleNum]);
	BG_GiveMeVectorFromMatrix(&boltMatrix, NEGATIVE_Y, pVeh->m_vMuzzleDir[muzzleNum]);
}

void WP_VehWeapSetSolidToOwner( gentity_t *self )
{
	self->r.svFlags |= SVF_OWNERNOTSHARED;
	if ( self->genericValue1 )
	{//expire after a time
		if ( self->genericValue2 )
		{//blow up when your lifetime is up
			self->think = G_ExplodeMissile;//FIXME: custom func?
		}
		else
		{//just remove yourself
			self->think = G_FreeEntity;//FIXME: custom func?
		}
		self->nextthink = level.time + self->genericValue1;
	}
}

#define VEH_HOMING_MISSILE_THINK_TIME		100
gentity_t *WP_FireVehicleWeapon( gentity_t *ent, vec3_t start, vec3_t dir, vehWeaponInfo_t *vehWeapon, qboolean alt_fire, qboolean isTurretWeap )
{
	gentity_t	*missile = NULL;

	//FIXME: add some randomness...?  Inherent inaccuracy stat of weapon?  Pilot skill?
	if ( !vehWeapon )
	{//invalid vehicle weapon
		return NULL;
	}
	else if ( vehWeapon->bIsProjectile )
	{//projectile entity
		vec3_t		mins, maxs;

		VectorSet( maxs, vehWeapon->fWidth/2.0f,vehWeapon->fWidth/2.0f,vehWeapon->fHeight/2.0f );
		VectorScale( maxs, -1, mins );

		//make sure our start point isn't on the other side of a wall
		W_TraceSetStart( ent, start, mins, maxs );
		
		//FIXME: CUSTOM MODEL?
		//QUERY: alt_fire true or not?  Does it matter?
		missile = CreateMissile( start, dir, vehWeapon->fSpeed, 10000, ent, qfalse );

		missile->classname = "vehicle_proj";
		
		missile->s.genericenemyindex = ent->s.number+MAX_GENTITIES;
		missile->damage = vehWeapon->iDamage;
		missile->splashDamage = vehWeapon->iSplashDamage;
		missile->splashRadius = vehWeapon->fSplashRadius;

		//FIXME: externalize some of these properties?
		missile->dflags = DAMAGE_DEATH_KNOCKBACK;
		missile->clipmask = MASK_SHOT;
		//Maybe by checking flags...?
		if ( vehWeapon->bSaberBlockable )
		{
			missile->clipmask |= CONTENTS_LIGHTSABER;
		}
		/*
		if ( (vehWeapon->iFlags&VWF_KNOCKBACK) )
		{
			missile->dflags &= ~DAMAGE_DEATH_KNOCKBACK;
		}
		if ( (vehWeapon->iFlags&VWF_RADAR) )
		{
			missile->s.eFlags |= EF_RADAROBJECT;
		}
		*/
		// Make it easier to hit things
		VectorCopy( mins, missile->r.mins );
		VectorCopy( maxs, missile->r.maxs );
		//some slightly different stuff for things with bboxes
		if ( vehWeapon->fWidth || vehWeapon->fHeight )
		{//we assume it's a rocket-like thing
			missile->s.weapon = WP_ROCKET_LAUNCHER;//does this really matter?
			missile->methodOfDeath = MOD_VEHICLE;//MOD_ROCKET;
			missile->splashMethodOfDeath = MOD_VEHICLE;//MOD_ROCKET;// ?SPLASH;

			// we don't want it to ever bounce
			missile->bounceCount = 0;

			missile->mass = 10;
		}
		else
		{//a blaster-laser-like thing
			missile->s.weapon = WP_BLASTER;//does this really matter?
			missile->methodOfDeath = MOD_VEHICLE; //count as a heavy weap
			missile->splashMethodOfDeath = MOD_VEHICLE;// ?SPLASH;
			// we don't want it to bounce forever
			missile->bounceCount = 8;
		}
		
		if ( vehWeapon->bHasGravity )
		{//TESTME: is this all we need to do?
			missile->s.weapon = WP_THERMAL;//does this really matter?
			missile->s.pos.trType = TR_GRAVITY;
		}
		
		if ( vehWeapon->bIonWeapon )
		{//so it disables ship shields and sends them out of control
			missile->s.weapon = WP_DEMP2;
		}

		if ( vehWeapon->iHealth )
		{//the missile can take damage
			//[Asteroids]
			/*
			//don't do this - ships hit them first and have no trace.plane.normal to bounce off it at and end up in the middle of the asteroid...
			missile->health = vehWeapon->iHealth;
			missile->takedamage = qtrue;
			missile->r.contents = MASK_SHOT;
			missile->die = RocketDie;
			*/
			//[/Asteroids]
		}

		//pilot should own this projectile on server if we have a pilot
		if (ent->m_pVehicle && ent->m_pVehicle->m_pPilot)
		{//owned by vehicle pilot
			missile->r.ownerNum = ent->m_pVehicle->m_pPilot->s.number;
		}
		else
		{//owned by vehicle?
			missile->r.ownerNum = ent->s.number;
		}

		//set veh as cgame side owner for purpose of fx overrides
		missile->s.owner = ent->s.number;
		if ( alt_fire )
		{//use the second weapon's iShotFX
			missile->s.eFlags |= EF_ALT_FIRING;
		}
		if ( isTurretWeap )
		{//look for the turret weapon info on cgame side, not vehicle weapon info
			missile->s.weapon = WP_TURRET;
		}
		if ( vehWeapon->iLifeTime )
		{//expire after a time
			if ( vehWeapon->bExplodeOnExpire )
			{//blow up when your lifetime is up
				missile->think = G_ExplodeMissile;//FIXME: custom func?
			}
			else
			{//just remove yourself
				missile->think = G_FreeEntity;//FIXME: custom func?
			}
			missile->nextthink = level.time + vehWeapon->iLifeTime;
		}
		missile->s.otherEntityNum2 = (vehWeapon-&g_vehWeaponInfo[0]);
		missile->s.eFlags |= EF_JETPACK_ACTIVE;
		//homing
		if ( vehWeapon->fHoming )
		{//homing missile
			if ( ent->client && ent->client->ps.rocketLockIndex != ENTITYNUM_NONE )
			{
				int dif = 0;
				float rTime;
				rTime = ent->client->ps.rocketLockTime;

				if (rTime == -1)
				{
					rTime = ent->client->ps.rocketLastValidTime;
				}

				if ( !vehWeapon->iLockOnTime )
				{//no minimum lock-on time
					dif = 10;//guaranteed lock-on
				}
				else
				{
					float lockTimeInterval = vehWeapon->iLockOnTime/16.0f;
					dif = ( level.time - rTime ) / lockTimeInterval;
				}

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
						missile->spawnflags |= 1;//just to let it know it should be faster...
						missile->speed = vehWeapon->fSpeed;
						missile->angle = vehWeapon->fHoming;
						missile->radius = vehWeapon->fHomingFOV;
						//crap, if we have a lifetime, need to store that somewhere else on ent and have rocketThink func check it every frame...
						if ( vehWeapon->iLifeTime )
						{//expire after a time
							missile->genericValue1 = level.time + vehWeapon->iLifeTime;
							missile->genericValue2 = (int)(vehWeapon->bExplodeOnExpire);
						}
						//now go ahead and use the rocketThink func
						missile->think = rocketThink;//FIXME: custom func?
						missile->nextthink = level.time + VEH_HOMING_MISSILE_THINK_TIME;
						missile->s.eFlags |= EF_RADAROBJECT;//FIXME: externalize
						if ( missile->enemy->s.NPC_class == CLASS_VEHICLE )
						{//let vehicle know we've locked on to them
							missile->s.otherEntityNum = missile->enemy->s.number;
						}
					}
				}

				VectorCopy( dir, missile->movedir );
				missile->random = 1.0f;//FIXME: externalize?
			}
		}
		if ( !vehWeapon->fSpeed )
		{//a mine or something?
			//[Asteroids]
			if ( vehWeapon->iHealth )
			{//the missile can take damage
				missile->health = vehWeapon->iHealth;
				missile->takedamage = qtrue;
				missile->r.contents = MASK_SHOT;
				missile->die = RocketDie;
			}
			//[/Asteroids]
			//only do damage when someone touches us
			missile->s.weapon = WP_THERMAL;//does this really matter?
			G_SetOrigin( missile, start );
			missile->touch = WP_TouchVehMissile;
			missile->s.eFlags |= EF_RADAROBJECT;//FIXME: externalize
			//crap, if we have a lifetime, need to store that somewhere else on ent and have rocketThink func check it every frame...
			if ( vehWeapon->iLifeTime )
			{//expire after a time
				missile->genericValue1 = vehWeapon->iLifeTime;
				missile->genericValue2 = (int)(vehWeapon->bExplodeOnExpire);
			}
			//now go ahead and use the setsolidtoowner func
			missile->think = WP_VehWeapSetSolidToOwner;
			missile->nextthink = level.time + 3000;
		}
	}
	else
	{//traceline
		//FIXME: implement
	}

	return missile;
}

//custom routine to not waste tempents horribly -rww
void G_VehMuzzleFireFX( gentity_t *ent, gentity_t *broadcaster, int muzzlesFired )
{
	Vehicle_t *pVeh = ent->m_pVehicle;
	gentity_t *b;

	if (!pVeh)
	{
		return;
	}

	if (!broadcaster)
	{ //oh well. We will WASTE A TEMPENT.
		b = G_TempEntity( ent->client->ps.origin, EV_VEH_FIRE );
	}
	else
	{ //joy
		b = broadcaster;
	}

	//this guy owns it
	b->s.owner = ent->s.number;

	//this is the bitfield of all muzzles fired this time
	//NOTE: just need MAX_VEHICLE_MUZZLES bits for this... should be cool since it's currently 12 and we're sending it in 16 bits
	b->s.trickedentindex = muzzlesFired;

	if ( broadcaster )
	{ //add the event
		G_AddEvent( b, EV_VEH_FIRE, 0 );
	}
}

void G_EstimateCamPos( vec3_t viewAngles, vec3_t cameraFocusLoc, float viewheight, float thirdPersonRange, 
					  float thirdPersonHorzOffset, float vertOffset, float pitchOffset, 
					  int ignoreEntNum, vec3_t camPos )
{
	int		MASK_CAMERACLIP = (MASK_SOLID|CONTENTS_PLAYERCLIP);
	float	CAMERA_SIZE = 4;
	vec3_t	cameramins;
	vec3_t	cameramaxs;
	vec3_t	cameraFocusAngles, camerafwd, cameraup;
	vec3_t	cameraIdealTarget, cameraCurTarget;
	vec3_t	cameraIdealLoc, cameraCurLoc;
	vec3_t	diff;
	vec3_t	camAngles;
	vec3_t	viewaxis[3];
	trace_t	trace;

	VectorSet( cameramins, -CAMERA_SIZE, -CAMERA_SIZE, -CAMERA_SIZE );
	VectorSet( cameramaxs, CAMERA_SIZE, CAMERA_SIZE, CAMERA_SIZE );

	VectorCopy( viewAngles, cameraFocusAngles );
	cameraFocusAngles[PITCH] += pitchOffset;
	if ( !bg_fighterAltControl.integer )
	{//clamp view pitch
		cameraFocusAngles[PITCH] = AngleNormalize180( cameraFocusAngles[PITCH] );
		if (cameraFocusAngles[PITCH] > 80.0)
		{
			cameraFocusAngles[PITCH] = 80.0;
		}
		else if (cameraFocusAngles[PITCH] < -80.0)
		{
			cameraFocusAngles[PITCH] = -80.0;
		}
	}
	AngleVectors(cameraFocusAngles, camerafwd, NULL, cameraup);

	cameraFocusLoc[2] += viewheight;

	VectorCopy( cameraFocusLoc, cameraIdealTarget );
	cameraIdealTarget[2] += vertOffset;

	//NOTE: on cgame, this uses the thirdpersontargetdamp value, we ignore that here
	VectorCopy( cameraIdealTarget, cameraCurTarget );
	trap_Trace( &trace, cameraFocusLoc, cameramins, cameramaxs, cameraCurTarget, ignoreEntNum, MASK_CAMERACLIP );
	if (trace.fraction < 1.0)
	{
		VectorCopy(trace.endpos, cameraCurTarget);
	}

	VectorMA(cameraIdealTarget, -(thirdPersonRange), camerafwd, cameraIdealLoc);
	//NOTE: on cgame, this uses the thirdpersoncameradamp value, we ignore that here
	VectorCopy( cameraIdealLoc, cameraCurLoc );
	trap_Trace(&trace, cameraCurTarget, cameramins, cameramaxs, cameraCurLoc, ignoreEntNum, MASK_CAMERACLIP);
	if (trace.fraction < 1.0)
	{
		VectorCopy( trace.endpos, cameraCurLoc );
	}

	VectorSubtract(cameraCurTarget, cameraCurLoc, diff);
	{
		float dist = VectorNormalize(diff);
		//under normal circumstances, should never be 0.00000 and so on.
		if ( !dist || (diff[0] == 0 || diff[1] == 0) )
		{//must be hitting something, need some value to calc angles, so use cam forward
			VectorCopy( camerafwd, diff );
		}
	}

	vectoangles(diff, camAngles);

	if ( thirdPersonHorzOffset != 0.0f )
	{
		AnglesToAxis( camAngles, viewaxis );
		VectorMA( cameraCurLoc, thirdPersonHorzOffset, viewaxis[1], cameraCurLoc );
	}

	VectorCopy(cameraCurLoc, camPos);
}

void WP_GetVehicleCamPos( gentity_t *ent, gentity_t *pilot, vec3_t camPos )
{
	float thirdPersonHorzOffset = ent->m_pVehicle->m_pVehicleInfo->cameraHorzOffset;
	float thirdPersonRange = ent->m_pVehicle->m_pVehicleInfo->cameraRange;
	float pitchOffset = ent->m_pVehicle->m_pVehicleInfo->cameraPitchOffset;
	float vertOffset = ent->m_pVehicle->m_pVehicleInfo->cameraVertOffset;

	if ( ent->client->ps.hackingTime )
	{
		thirdPersonHorzOffset += (((float)ent->client->ps.hackingTime)/MAX_STRAFE_TIME) * -80.0f;
		thirdPersonRange += fabs(((float)ent->client->ps.hackingTime)/MAX_STRAFE_TIME) * 100.0f;
	}

	if ( ent->m_pVehicle->m_pVehicleInfo->cameraPitchDependantVertOffset )
	{
		if ( pilot->client->ps.viewangles[PITCH] > 0 )
		{
			vertOffset = 130+pilot->client->ps.viewangles[PITCH]*-10;
			if ( vertOffset < -170 )
			{
				vertOffset = -170;
			}
		}
		else if ( pilot->client->ps.viewangles[PITCH] < 0 )
		{
			vertOffset = 130+pilot->client->ps.viewangles[PITCH]*-5;
			if ( vertOffset > 130 )
			{
				vertOffset = 130;
			}
		}
		else
		{
			vertOffset = 30;
		}
		if ( pilot->client->ps.viewangles[PITCH] > 0 )
		{
			pitchOffset = pilot->client->ps.viewangles[PITCH]*-0.75;
		}
		else if ( pilot->client->ps.viewangles[PITCH] < 0 )
		{
			pitchOffset = pilot->client->ps.viewangles[PITCH]*-0.75;
		}
		else
		{
			pitchOffset = 0;
		}
	}

	//Control Scheme 3 Method:
	G_EstimateCamPos( ent->client->ps.viewangles, pilot->client->ps.origin, pilot->client->ps.viewheight, thirdPersonRange, 
		thirdPersonHorzOffset, vertOffset, pitchOffset, 
		pilot->s.number, camPos );
	/*
	//Control Scheme 2 Method:
	G_EstimateCamPos( ent->m_pVehicle->m_vOrientation, ent->r.currentOrigin, pilot->client->ps.viewheight, thirdPersonRange, 
		thirdPersonHorzOffset, vertOffset, pitchOffset, 
		pilot->s.number, camPos );
	*/
}

void WP_VehLeadCrosshairVeh( gentity_t *camTraceEnt, vec3_t newEnd, const vec3_t dir, const vec3_t shotStart, vec3_t shotDir )
{
	//[Asteroids]
	if ( g_vehAutoAimLead.integer )
	{
		if ( camTraceEnt 
			&& camTraceEnt->client
			&& camTraceEnt->client->NPC_class == CLASS_VEHICLE )
		{//if the crosshair is on a vehicle, lead it
			float dot, distAdjust = DotProduct( camTraceEnt->client->ps.velocity, dir );
			vec3_t	predPos, predShotDir;
			if ( distAdjust > 500 || DistanceSquared( camTraceEnt->client->ps.origin, shotStart ) > 7000000 )
			{//moving away from me at a decent speed and/or more than @2600 units away from me
				VectorMA( newEnd, distAdjust, dir, predPos );
				VectorSubtract( predPos, shotStart, predShotDir );
				VectorNormalize( predShotDir );
				dot = DotProduct( predShotDir, shotDir );
				if ( dot >= 0.75f )
				{//if the new aim vector is no more than 23 degrees off the original one, go ahead and adjust the aim
					VectorCopy( predPos, newEnd );
				}
			}
		}
	//[/Asteroids]
	}
	VectorSubtract( newEnd, shotStart, shotDir );
	VectorNormalize( shotDir );
}

#define MAX_XHAIR_DIST_ACCURACY	20000.0f
extern float g_cullDistance;
extern int BG_VehTraceFromCamPos( trace_t *camTrace, bgEntity_t *bgEnt, const vec3_t entOrg, const vec3_t shotStart, const vec3_t end, vec3_t newEnd, vec3_t shotDir, float bestDist );
qboolean WP_VehCheckTraceFromCamPos( gentity_t *ent, const vec3_t shotStart, vec3_t shotDir )
{
	//FIXME: only if dynamicCrosshair and dynamicCrosshairPrecision is on!
	if ( !ent 
		|| !ent->m_pVehicle 
		|| !ent->m_pVehicle->m_pVehicleInfo 
		|| !ent->m_pVehicle->m_pPilot//not being driven
		|| !((gentity_t*)ent->m_pVehicle->m_pPilot)->client//not being driven by a client...?!!!
		|| (ent->m_pVehicle->m_pPilot->s.number >= MAX_CLIENTS) )//being driven, but not by a real client, no need to worry about crosshair
	{
		return qfalse;
	}
	if ( (ent->m_pVehicle->m_pVehicleInfo->type == VH_FIGHTER && g_cullDistance > MAX_XHAIR_DIST_ACCURACY )
		|| ent->m_pVehicle->m_pVehicleInfo->type == VH_WALKER)
	{
		//FIRST: simulate the normal crosshair trace from the center of the veh straight forward
		trace_t trace;
		vec3_t	dir, start, end;
		if ( ent->m_pVehicle->m_pVehicleInfo->type == VH_WALKER )
		{//for some reason, the walker always draws the crosshair out from from the first muzzle point
			AngleVectors( ent->client->ps.viewangles, dir, NULL, NULL );
			VectorCopy( ent->r.currentOrigin, start );
			start[2] += ent->m_pVehicle->m_pVehicleInfo->height-DEFAULT_MINS_2-48;
		}
		else
		{
			vec3_t ang;
			if (ent->m_pVehicle->m_pVehicleInfo->type == VH_SPEEDER)
			{
				VectorSet(ang, 0.0f, ent->m_pVehicle->m_vOrientation[1], 0.0f);
			}
			else
			{
				VectorCopy(ent->m_pVehicle->m_vOrientation, ang);
			}
			AngleVectors( ang, dir, NULL, NULL );
			VectorCopy( ent->r.currentOrigin, start );
		}
		VectorMA( start, g_cullDistance, dir, end );
		trap_Trace( &trace, start, vec3_origin, vec3_origin, end, 
			ent->s.number, CONTENTS_SOLID|CONTENTS_BODY );

		if ( ent->m_pVehicle->m_pVehicleInfo->type == VH_WALKER )
		{//just use the result of that one trace since walkers don't do the extra trace
			VectorSubtract( trace.endpos, shotStart, shotDir );
			VectorNormalize( shotDir );
			return qtrue;
		}
		else
		{//NOW do the trace from the camPos and compare with above trace
			trace_t	extraTrace;
			vec3_t	newEnd;
			int camTraceEntNum = BG_VehTraceFromCamPos( &extraTrace, (bgEntity_t *)ent, ent->r.currentOrigin, shotStart, end, newEnd, shotDir, (trace.fraction*g_cullDistance) );
			if ( camTraceEntNum )
			{
				WP_VehLeadCrosshairVeh( &g_entities[camTraceEntNum-1], newEnd, dir, shotStart, shotDir );
				return qtrue;
			}
		}
	}
	return qfalse;
}

//---------------------------------------------------------
void FireVehicleWeapon( gentity_t *ent, qboolean alt_fire ) 
//---------------------------------------------------------
{
	Vehicle_t *pVeh = ent->m_pVehicle;
	int muzzlesFired = 0;
	gentity_t *missile = NULL;
	vehWeaponInfo_t *vehWeapon = NULL;
	qboolean	clearRocketLockEntity = qfalse;
	
	//ROP VEHICLE_IMP START
	int nStopPrimary;
	int	nStopAlt;

	if ( !pVeh )
	{
		return;
	}

	if(pVeh->bTransAnimFlag)
	{
		//Can never fire during transition anims
		return;
	}

	//Check whether we can fire
	nStopPrimary = pVeh->m_pVehicleInfo->MBFstopprimaryfiring;
	nStopAlt = pVeh->m_pVehicleInfo->MBFstopaltfiring;

	if((pVeh->eCurrentMode & pVeh->m_pVehicleInfo->MBFstopprimaryfiring) && !alt_fire)
	{
		//We can't primary fire in current mode
		return;
	}

	if((pVeh->eCurrentMode & pVeh->m_pVehicleInfo->MBFstopaltfiring) && alt_fire)
	{
		//We can't alt fire in current mode
		return;
	}
	//ROP VEHICLE_IMP END

	if (pVeh->m_iRemovedSurfaces)
	{ //can't fire when the thing is breaking apart
		return;
	}

	if (pVeh->m_pVehicleInfo->type == VH_WALKER &&
		ent->client->ps.electrifyTime > level.time)
	{ //don't fire while being electrocuted
		return;
	}

	// TODO?: If possible (probably not enough time), it would be nice if secondary fire was actually a mode switch/toggle
	// so that, for instance, an x-wing can have 4-gun fire, or individual muzzle fire. If you wanted a different weapon, you
	// would actually have to press the 2 key or something like that (I doubt I'd get a graphic for it anyways though). -AReis

	// If this is not the alternate fire, fire a normal blaster shot...
	if ( pVeh->m_pVehicleInfo &&
		(pVeh->m_pVehicleInfo->type != VH_FIGHTER || (pVeh->m_ulFlags&VEH_WINGSOPEN)) ) // NOTE: Wings open also denotes that it has already launched.
	{//fighters can only fire when wings are open
		int	weaponNum = 0, vehWeaponIndex = VEH_WEAPON_NONE;
		int	delay = 1000;
		qboolean aimCorrect = qfalse;
		qboolean linkedFiring = qfalse;

		if ( !alt_fire )
		{
			weaponNum = 0;
		}
		else
		{
			weaponNum = 1;
		}

		vehWeaponIndex = pVeh->m_pVehicleInfo->weapon[weaponNum].ID;

		if ( pVeh->weaponStatus[weaponNum].ammo <= 0 )
		{//no ammo for this weapon
			if ( pVeh->m_pPilot && pVeh->m_pPilot->s.number < MAX_CLIENTS )
			{// let the client know he's out of ammo
				int i;
				//but only if one of the vehicle muzzles is actually ready to fire this weapon
				for ( i = 0; i < MAX_VEHICLE_MUZZLES; i++ )
				{
					if ( pVeh->m_pVehicleInfo->weapMuzzle[i] != vehWeaponIndex )
					{//this muzzle doesn't match the weapon we're trying to use
						continue;
					}
					if ( pVeh->m_iMuzzleTag[i] != -1 
						&& pVeh->m_iMuzzleWait[i] < level.time )
					{//this one would have fired, send the no ammo message
						G_AddEvent( (gentity_t*)pVeh->m_pPilot, EV_NOAMMO, weaponNum );
						break;
					}
				}
			}
			return;
		}

		delay = pVeh->m_pVehicleInfo->weapon[weaponNum].delay;
		aimCorrect = pVeh->m_pVehicleInfo->weapon[weaponNum].aimCorrect;
		if ( pVeh->m_pVehicleInfo->weapon[weaponNum].linkable == 2//always linked
			|| ( pVeh->m_pVehicleInfo->weapon[weaponNum].linkable == 1//optionally linkable
				 && pVeh->weaponStatus[weaponNum].linked ) )//linked
		{//we're linking the primary or alternate weapons, so we'll do *all* the muzzles
			linkedFiring = qtrue;
		}

		if ( vehWeaponIndex <= VEH_WEAPON_BASE || vehWeaponIndex >= MAX_VEH_WEAPONS )
		{//invalid vehicle weapon
			return;
		}
		else
		{
			int i, numMuzzles = 0, numMuzzlesReady = 0, cumulativeDelay = 0, cumulativeAmmo = 0;
			qboolean sentAmmoWarning = qfalse;

			vehWeapon = &g_vehWeaponInfo[vehWeaponIndex];

			if ( pVeh->m_pVehicleInfo->weapon[weaponNum].linkable == 2 )
			{//always linked weapons don't accumulate delay, just use specified delay
				cumulativeDelay = delay;
			}
			//find out how many we've got for this weapon
			for ( i = 0; i < MAX_VEHICLE_MUZZLES; i++ )
			{
				if ( pVeh->m_pVehicleInfo->weapMuzzle[i] != vehWeaponIndex )
				{//this muzzle doesn't match the weapon we're trying to use
					continue;
				}
				if ( pVeh->m_iMuzzleTag[i] != -1 && pVeh->m_iMuzzleWait[i] < level.time )
				{
					numMuzzlesReady++;
				}
				if ( pVeh->m_pVehicleInfo->weapMuzzle[pVeh->weaponStatus[weaponNum].nextMuzzle] != vehWeaponIndex )
				{//Our designated next muzzle for this weapon isn't valid for this weapon (happens when ships fire for the first time)
					//set the next to this one
					pVeh->weaponStatus[weaponNum].nextMuzzle = i;
				}
				if ( linkedFiring )
				{
					cumulativeAmmo += vehWeapon->iAmmoPerShot;
					if ( pVeh->m_pVehicleInfo->weapon[weaponNum].linkable != 2 )
					{//always linked weapons don't accumulate delay, just use specified delay
						cumulativeDelay += delay;
					}
				}
				numMuzzles++;
			}

			if ( linkedFiring )
			{//firing all muzzles at once
				if ( numMuzzlesReady != numMuzzles )
				{//can't fire all linked muzzles yet
					return;
				}
				else 
				{//can fire all linked muzzles, check ammo
					if ( pVeh->weaponStatus[weaponNum].ammo < cumulativeAmmo )
					{//can't fire, not enough ammo
						if ( pVeh->m_pPilot && pVeh->m_pPilot->s.number < MAX_CLIENTS )
						{// let the client know he's out of ammo
							G_AddEvent( (gentity_t*)pVeh->m_pPilot, EV_NOAMMO, weaponNum );
						}
						return;
					}
				}
			}

			for ( i = 0; i < MAX_VEHICLE_MUZZLES; i++ )
			{
				if ( pVeh->m_pVehicleInfo->weapMuzzle[i] != vehWeaponIndex )
				{//this muzzle doesn't match the weapon we're trying to use
					continue;
				}
				if ( !linkedFiring
					&& i != pVeh->weaponStatus[weaponNum].nextMuzzle )
				{//we're only firing one muzzle and this isn't it
					continue;
				}

				// Fire this muzzle.
				if ( pVeh->m_iMuzzleTag[i] != -1 && pVeh->m_iMuzzleWait[i] < level.time )
				{
					vec3_t	start, dir;
					
					if ( pVeh->weaponStatus[weaponNum].ammo < vehWeapon->iAmmoPerShot )
					{//out of ammo!
						if ( !sentAmmoWarning )
						{
							sentAmmoWarning = qtrue;
							if ( pVeh->m_pPilot && pVeh->m_pPilot->s.number < MAX_CLIENTS )
							{// let the client know he's out of ammo
								G_AddEvent( (gentity_t*)pVeh->m_pPilot, EV_NOAMMO, weaponNum );
							}
						}
					}
					else
					{//have enough ammo to shoot
						//do the firing
						WP_CalcVehMuzzle(ent, i);
						VectorCopy( pVeh->m_vMuzzlePos[i], start );
						VectorCopy( pVeh->m_vMuzzleDir[i], dir );
						if ( WP_VehCheckTraceFromCamPos( ent, start, dir ) )
						{//auto-aim at whatever crosshair would be over from camera's point of view (if closer)
						}
						else if ( aimCorrect )
						{//auto-aim the missile at the crosshair if there's anything there
							trace_t trace;
							vec3_t	end;
							vec3_t	ang;
							vec3_t	fixedDir;

							if (pVeh->m_pVehicleInfo->type == VH_SPEEDER)
							{
								VectorSet(ang, 0.0f, pVeh->m_vOrientation[1], 0.0f);
							}
							else
							{
								VectorCopy(pVeh->m_vOrientation, ang);
							}
							AngleVectors( ang, fixedDir, NULL, NULL );
							VectorMA( ent->r.currentOrigin, 32768, fixedDir, end );
							//VectorMA( ent->r.currentOrigin, 8192, dir, end );
							trap_Trace( &trace, ent->r.currentOrigin, vec3_origin, vec3_origin, end, ent->s.number, MASK_SHOT );
							if ( trace.fraction < 1.0f && !trace.allsolid && !trace.startsolid )
							{
								vec3_t newEnd;
								VectorCopy( trace.endpos, newEnd );
								WP_VehLeadCrosshairVeh( &g_entities[trace.entityNum], newEnd, fixedDir, start, dir );
							}
						}

						//play the weapon's muzzle effect if we have one
						//NOTE: just need MAX_VEHICLE_MUZZLES bits for this... should be cool since it's currently 12 and we're sending it in 16 bits
						muzzlesFired |= (1<<i);
												
						missile = WP_FireVehicleWeapon( ent, start, dir, vehWeapon, alt_fire, qfalse );
						if ( vehWeapon->fHoming )
						{//clear the rocket lock entity *after* all muzzles have fired
							clearRocketLockEntity = qtrue;
						}
					}

					if ( linkedFiring )
					{//we're linking the weapon, so continue on and fire all appropriate muzzles
						continue;
					}
					//else just firing one
					//take the ammo, set the next muzzle and set the delay on it
					if ( numMuzzles > 1 )
					{//more than one, look for it
						int nextMuzzle = pVeh->weaponStatus[weaponNum].nextMuzzle;
						while ( 1 )
						{
							nextMuzzle++;
							if ( nextMuzzle >= MAX_VEHICLE_MUZZLES )
							{
								nextMuzzle = 0;
							}
							if ( nextMuzzle == pVeh->weaponStatus[weaponNum].nextMuzzle )
							{//WTF?  Wrapped without finding another valid one!
								break;
							}
							if ( pVeh->m_pVehicleInfo->weapMuzzle[nextMuzzle] == vehWeaponIndex )
							{//this is the next muzzle for this weapon
								pVeh->weaponStatus[weaponNum].nextMuzzle = nextMuzzle;
								break;
							}
						}
					}//else, just stay on the one we just fired
					//set the delay on the next muzzle
					pVeh->m_iMuzzleWait[pVeh->weaponStatus[weaponNum].nextMuzzle] = level.time + delay;
					//take away the ammo
					pVeh->weaponStatus[weaponNum].ammo -= vehWeapon->iAmmoPerShot;
					//NOTE: in order to send the vehicle's ammo info to the client, we copy the ammo into the first 2 ammo slots on the vehicle NPC's client->ps.ammo array
					if ( pVeh->m_pParentEntity && ((gentity_t*)(pVeh->m_pParentEntity))->client )
					{
						((gentity_t*)(pVeh->m_pParentEntity))->client->ps.ammo[weaponNum] = pVeh->weaponStatus[weaponNum].ammo;
					}
					//done!
					//we'll get in here again next frame and try the next muzzle...
					//return;
					goto tryFire;
				}
			}
			//we went through all the muzzles, so apply the cumulative delay and ammo cost
			if ( cumulativeAmmo )
			{//taking ammo one shot at a time
				//take the ammo
				pVeh->weaponStatus[weaponNum].ammo -= cumulativeAmmo;
				//NOTE: in order to send the vehicle's ammo info to the client, we copy the ammo into the first 2 ammo slots on the vehicle NPC's client->ps.ammo array
				if ( pVeh->m_pParentEntity && ((gentity_t*)(pVeh->m_pParentEntity))->client )
				{
					((gentity_t*)(pVeh->m_pParentEntity))->client->ps.ammo[weaponNum] = pVeh->weaponStatus[weaponNum].ammo;
				}
			}
			if ( cumulativeDelay )
			{//we linked muzzles so we need to apply the cumulative delay now, to each of the linked muzzles
				for ( i = 0; i < MAX_VEHICLE_MUZZLES; i++ )
				{
					if ( pVeh->m_pVehicleInfo->weapMuzzle[i] != vehWeaponIndex )
					{//this muzzle doesn't match the weapon we're trying to use
						continue;
					}
					//apply the cumulative delay
					pVeh->m_iMuzzleWait[i] = level.time + cumulativeDelay;
				}
			}
		}
	}

tryFire:
	if ( clearRocketLockEntity )
	{//hmm, should probably clear that anytime any weapon fires?
		ent->client->ps.rocketLockIndex = ENTITYNUM_NONE;
		ent->client->ps.rocketLockTime = 0;
		ent->client->ps.rocketTargetTime = 0;
	}

	if ( vehWeapon && muzzlesFired > 0 )
	{
		G_VehMuzzleFireFX(ent, missile, muzzlesFired );
	}
}


//[WEAPONSDAT]
qboolean WP_CallFireFunction( int weaponNumber, qboolean altFire, gentity_t* ent )
{//calls the set weapon fire function.  Uses the default fire functions if
	//these aren't set.
	int index = 0;
	if(altFire)
		index = weaponData[weaponNumber].altFireFunctionIndex;
	else
		index = weaponData[weaponNumber].fireFunctionIndex;
	if(index == 0)
		return qfalse;
	(fireFunctions[index].func)(ent);
	return qtrue;
}
//[/WEAPONSDAT]

/*
===============
FireWeapon
===============
*/
#include "../namespace_begin.h"
int BG_EmplacedView(vec3_t baseAngles, vec3_t angles, float *newYaw, float constraint);
#include "../namespace_end.h"
#ifdef AKIMBO
void FireWeapon( gentity_t *ent, qboolean altFire, qboolean leftWeapon ) 
#else
void FireWeapon( gentity_t *ent, qboolean altFire ) 
#endif
{
	//[CoOp]
	float alert = 256;  //alert level for weapon alter events
	//[/CoOp]
#ifdef AKIMBO
	int weapon2 = ent->client->ps.weapon2;
#endif

	if (ent->client->ps.powerups[PW_QUAD] ) {
		s_quadFactor = g_quadfactor.value;
	} else {
		s_quadFactor = 1;
	}

	// track shots taken for accuracy tracking.  Grapple is not a weapon and gauntet is just not tracked
	if( ent->s.weapon != WP_SABER && ent->s.weapon != WP_STUN_BATON && ent->s.weapon != WP_MELEE ) 
	{
		if( ent->s.weapon == WP_FLECHETTE ) {
			ent->client->accuracy_shots += FLECHETTE_SHOTS;
		} else {
			ent->client->accuracy_shots++;
		}
	}

	if ( ent && ent->client && ent->client->NPC_class == CLASS_VEHICLE )
	{
		FireVehicleWeapon( ent, altFire );
		return;
	}
	else
	{
		// set aiming directions
		if (ent->s.weapon == WP_EMPLACED_GUN &&
			ent->client->ps.emplacedIndex)
		{ //if using emplaced then base muzzle point off of gun position/angles
			gentity_t *emp = &g_entities[ent->client->ps.emplacedIndex];

			if (emp->inuse)
			{
				float yaw;
				vec3_t viewAngCap;
				int override;

				VectorCopy(ent->client->ps.viewangles, viewAngCap);
				if (viewAngCap[PITCH] > 40)
				{
					viewAngCap[PITCH] = 40;
				}

				override = BG_EmplacedView(ent->client->ps.viewangles, emp->s.angles, &yaw,
					emp->s.origin2[0]);
				
				if (override)
				{
					viewAngCap[YAW] = yaw;
				}

				AngleVectors( viewAngCap, wp_forward, wp_vright, wp_up );
			}
			else
			{
				AngleVectors( ent->client->ps.viewangles, wp_forward, wp_vright, wp_up );
			}
		}
		else if (ent->s.number < MAX_CLIENTS &&
			ent->client->ps.m_iVehicleNum && ent->s.weapon == WP_BLASTER)
		{ //riding a vehicle...with blaster selected
			vec3_t vehTurnAngles;
			gentity_t *vehEnt = &g_entities[ent->client->ps.m_iVehicleNum];

			if (vehEnt->inuse && vehEnt->client && vehEnt->m_pVehicle)
			{
				VectorCopy(vehEnt->m_pVehicle->m_vOrientation, vehTurnAngles);
				vehTurnAngles[PITCH] = ent->client->ps.viewangles[PITCH];
			}
			else
			{
				VectorCopy(ent->client->ps.viewangles, vehTurnAngles);
			}
			if (ent->client->pers.cmd.rightmove > 0)
			{ //shooting to right
				vehTurnAngles[YAW] -= 90.0f;
			}
			else if (ent->client->pers.cmd.rightmove < 0)
			{ //shooting to left
				vehTurnAngles[YAW] += 90.0f;
			}

			AngleVectors( vehTurnAngles, wp_forward, wp_vright, wp_up );
		}
		else
		{
			AngleVectors( ent->client->ps.viewangles, wp_forward, wp_vright, wp_up );
		}

		CalcMuzzlePoint ( ent, wp_forward, wp_vright, wp_up, wp_muzzle );
#ifdef AKIMBO
		if(weapon2 != WP_NONE)
		{
			CalcMuzzlePoint2( ent, wp_forward, wp_vright, wp_up, wp_muzzle2 );
		}
#endif


		// fire the specific weapon
		if( ent->s.weapon == WP_EMPLACED_GUN || ent->s.weapon == WP_SABER || ent->s.weapon == WP_MELEE  || ent->s.weapon == WP_NONE || ent->s.weapon == WP_TURRET ||
			ent->s.weapon == WP_THERMAL || ent->s.weapon == WP_TRIP_MINE || ent->s.weapon == WP_DET_PACK || ent->s.weapon == WP_ROCKET_LAUNCHER || ent->s.weapon == WP_AUXILIARY6 ||
			ent->s.weapon == WP_AUXILIARY7 || ent->s.weapon == WP_AUXILIARY8)
		{
//[WEAPONSDAT]
		if(!WP_CallFireFunction(ent->s.weapon, altFire, ent))
		{
//[/WEAPONSDAT]
		switch( ent->s.weapon ) {
		case WP_STUN_BATON:
			WP_FireStunBaton( ent, altFire );
			break;

		case WP_MELEE:
			//[CoOp]
			alert = 0;
			//[/CoOp]
			WP_FireMelee(ent, altFire);
			break;

		case WP_SABER:
			break;

		case WP_BRYAR_PISTOL:
			//if ( g_gametype.integer == GT_SIEGE )
			/*if (1)
			{//allow alt-fire
				WP_FireBowcaster( ent, altFire );
			}
			else
			{
				WP_FireBowcaster( ent, qfalse );
			}*/
#ifdef AKIMBO
			if(!leftWeapon)
#endif
			WP_FireBowcaster(ent, qfalse);
			break;

		case WP_CONCUSSION:
			WP_FireConcussion( ent, altFire );
			break;

		case WP_BRYAR_OLD:
			WP_FireBowcaster( ent, altFire );
			break;

		case WP_BLASTER:
			WP_FireBowcaster( ent, altFire );
			break;

		case WP_DISRUPTOR:
			//[CoOp]
			alert = 50;
			//[/CoOp]
			WP_FireDisruptor( ent, altFire );
			break;

		case WP_BOWCASTER:
			WP_FireBowcaster( ent, altFire );
			break;

		case WP_REPEATER:
			WP_FireRepeater( ent, altFire );
			break;

		case WP_DEMP2:
			WP_FireDEMP2( ent, altFire );
			break;

		case WP_FLECHETTE:
			WP_FireFlechette( ent, altFire );
			break;

		case WP_ROCKET_LAUNCHER:
			if(weaponData[ent->s.weapon].hcIdx != HC_RPG)
			{
				switch(weaponData[ent->s.weapon].hcIdx)
				{
					case HC_STUN_GRENADE:
					case HC_SMOKE_GRENADE:
					case HC_GAS_GRENADE:
						break;
				}
			}
			else
				WP_FireRocket( ent, altFire );
			break;

		case WP_THERMAL:
			/*if(weaponData[ent->s.weapon].hcIdx != HC_GRENADE)
			{
				switch(weaponData[ent->s.weapon].hcIdx)
				{
					case HC_STUN_GRENADE:
						WP_FireStunGrenade( ent );
						break;
					case HC_SMOKE_GRENADE:
						WP_FireSmokeGrenade( ent );
						break;
					case HC_GAS_GRENADE:
						WP_FireGasGrenade( ent );
						break;
				}
			}
			else*/
				WP_FireThermalDetonator( ent, altFire );
			break;

		case WP_TRIP_MINE:
			//[CoOp]
			alert = 0;
			//[/CoOp]
			/*if(weaponData[ent->s.weapon].hcIdx != HC_FLASHBANG)
			{
				switch(weaponData[ent->s.weapon].hcIdx)
				{
					case HC_SMOKE_GRENADE:
						WP_FireSmokeGrenade( ent );
						break;
					case HC_GAS_GRENADE:
						WP_FireGasGrenade( ent );
						break;
					case HC_STUN_GRENADE:
						WP_FireStunGrenade( ent );
						break;
				}
			}
			else*/
				WP_FireFlashbang( ent, altFire );
			break;

		case WP_DET_PACK:
			//[CoOp]
			alert = 0;
			//[/CoOp]
			WP_DropDetPack( ent, altFire );
			break;

		case WP_EMPLACED_GUN:
			if (ent->client && ent->client->ewebIndex)
			{ //specially handled by the e-web itself
				break;
			}
			WP_FireEmplaced( ent, altFire );
			break;
		case WP_AUXILIARY6:
			alert = 0;
			WP_FireStunGrenade( ent );
			break;
		case WP_AUXILIARY7:
			WP_FireSmokeGrenade( ent );
			break;
		case WP_AUXILIARY8:
			WP_FireGasGrenade(ent);
			break;
		default:
//			assert(!"unknown weapon fire");
			break;
		}
//[WEAPONSDAT]
		}
//[/WEAPONSDAT]
		}
		else
		{
#ifdef AKIMBO
			if(ent->s.weapon2 != WP_NONE)
			{ //Akimbo!
				if(!leftWeapon)
				{ //Right hand weapons use alt fire
					if(weaponData[ent->s.weapon].altFireFuncInvert)
						weaponData[ent->s.weapon].fireFunc(ent, qtrue, qtrue);
					else
						weaponData[ent->s.weapon].fireFunc(ent, qfalse, qtrue);
				}
				else
				{ //Left hand weapons use primary fire
					if(weaponData[ent->s.weapon2].fireFuncInvert)
						weaponData[ent->s.weapon2].fireFunc(ent, qtrue, qfalse);
					else
						weaponData[ent->s.weapon2].fireFunc(ent, qfalse, qfalse);
				}
			}
			else
			{ //Not using Akimbo
				if(altFire)
				{
					if(weaponData[ent->s.weapon].altFireFuncInvert)
						weaponData[ent->s.weapon].altFireFunc(ent, qfalse, qtrue);
					else
						weaponData[ent->s.weapon].altFireFunc(ent, qtrue, qtrue);
				}
				else
				{
					if(weaponData[ent->s.weapon].fireFuncInvert)
						weaponData[ent->s.weapon].fireFunc(ent, qtrue, qtrue);
					else
						weaponData[ent->s.weapon].fireFunc(ent, qfalse, qtrue);
				}
			}
#else
			if(altFire)
			{
				if(weaponData[ent->s.weapon].altFireFuncInvert)
					weaponData[ent->s.weapon].altFireFunc(ent, qfalse);
				else
					weaponData[ent->s.weapon].altFireFunc(ent, qtrue);
			}
			else
			{
				if(weaponData[ent->s.weapon].fireFuncInvert)
					weaponData[ent->s.weapon].fireFunc(ent, qtrue);
				else
					weaponData[ent->s.weapon].fireFunc(ent, qfalse);
			}
#endif
		}
	}

	//[CoOp] SP code
	//alert events for NPCs
	// We should probably just use this as a default behavior, in special cases, just set alert to false.
	if ( alert > 0 )
	{
		if ( ent->client->ps.groundEntityNum == ENTITYNUM_WORLD//FIXME: check for sand contents type?
			&& ent->s.weapon != WP_STUN_BATON
			&& ent->s.weapon != WP_MELEE
			//&& ent->s.weapon != WP_TUSKEN_STAFF //RAFIXME - Impliment?
			&& ent->s.weapon != WP_THERMAL
			&& ent->s.weapon != WP_TRIP_MINE
			&& ent->s.weapon != WP_DET_PACK )
		{//the vibration of the shot carries through your feet into the ground
			AddSoundEvent( ent, wp_muzzle, alert, AEL_DISCOVERED, qfalse, qtrue );
		}
		else
		{//an in-air alert
			AddSoundEvent( ent, wp_muzzle, alert, AEL_DISCOVERED, qfalse, qfalse );
		}
		AddSightEvent( ent, wp_muzzle, alert*2, AEL_DISCOVERED, 20 );
	}
	//[/CoOp]

	G_LogWeaponFire(ent->s.number, ent->s.weapon);
}

//---------------------------------------------------------
static void WP_FireEmplaced( gentity_t *ent, qboolean altFire )
//---------------------------------------------------------
{
	vec3_t	dir, angs, gunpoint;
	vec3_t	right;
	gentity_t *gun;
	int side;

	if (!ent->client)
	{
		return;
	}

	if (!ent->client->ps.emplacedIndex)
	{ //shouldn't be using WP_EMPLACED_GUN if we aren't on an emplaced weapon
		return;
	}

	gun = &g_entities[ent->client->ps.emplacedIndex];

	if (!gun->inuse || gun->health <= 0)
	{ //gun was removed or killed, although we should never hit this check because we should have been forced off it already
		return;
	}

	VectorCopy(gun->s.origin, gunpoint);
	gunpoint[2] += 46;

	AngleVectors(ent->client->ps.viewangles, NULL, right, NULL);

	if (gun->genericValue10)
	{ //fire out of the right cannon side
		VectorMA(gunpoint, 10.0f, right, gunpoint);
		side = 0;
	}
	else
	{ //the left
		VectorMA(gunpoint, -10.0f, right, gunpoint);
		side = 1;
	}

	gun->genericValue10 = side;
	G_AddEvent(gun, EV_FIRE_WEAPON, side);

	vectoangles( wp_forward, angs );

	AngleVectors( angs, dir, NULL, NULL );

	WP_FireEmplacedMissile( gun, gunpoint, dir, altFire, ent );
}


#define EMPLACED_CANRESPAWN 1

//----------------------------------------------------------

/*QUAKED emplaced_gun (0 0 1) (-30 -20 8) (30 20 60) CANRESPAWN

 count - if CANRESPAWN spawnflag, decides how long it is before gun respawns (in ms)
 constraint - number of degrees gun is constrained from base angles on each side (default 60.0)

 showhealth - set to 1 to show health bar on this entity when crosshair is over it
  
  teamowner - crosshair shows green for this team, red for opposite team
	0 - none
	1 - red
	2 - blue

  alliedTeam - team that can use this
	0 - any
	1 - red
	2 - blue

  teamnodmg - team that turret does not take damage from or do damage to
	0 - none
	1 - red
	2 - blue
*/
 
//----------------------------------------------------------
extern qboolean TryHeal(gentity_t *ent, gentity_t *target); //g_utils.c
void emplaced_gun_use( gentity_t *self, gentity_t *other, trace_t *trace )
{
	vec3_t fwd1, fwd2;
	float dot;
	int oldWeapon;
	gentity_t *activator = other;
	float zoffset = 50;
	vec3_t anglesToOwner;
	vec3_t vLen;
	float ownLen;

	if ( self->health <= 0 )
	{ //gun is destroyed
		return;
	}

	if (self->activator)
	{ //someone is already using me
		return;
	}

	if (!activator->client)
	{
		return;
	}

	if (activator->client->ps.emplacedTime > level.time)
	{ //last use attempt still too recent
		return;
	}

	if (activator->client->ps.forceHandExtend != HANDEXTEND_NONE)
	{ //don't use if busy doing something else
		return;
	}

	if (activator->client->ps.origin[2] > self->s.origin[2]+zoffset-8)
	{ //can't use it from the top
		return;
	}

	if (activator->client->ps.pm_flags & PMF_DUCKED)
	{ //must be standing
		return;
	}

	if (activator->client->ps.isJediMaster)
	{ //jm can't use weapons
		return;
	}

	VectorSubtract(self->s.origin, activator->client->ps.origin, vLen);
	ownLen = VectorLength(vLen);

	if (ownLen > 64.0f)
	{ //must be within 64 units of the gun to use at all
		return;
	}

	// Let's get some direction vectors for the user
	AngleVectors( activator->client->ps.viewangles, fwd1, NULL, NULL );

	// Get the guns direction vector
	AngleVectors( self->pos1, fwd2, NULL, NULL );

	dot = DotProduct( fwd1, fwd2 );

	// Must be reasonably facing the way the gun points ( 110 degrees or so ), otherwise we don't allow to use it.
	if ( dot < -0.2f )
	{
		goto tryHeal;
	}

	VectorSubtract(self->s.origin, activator->client->ps.origin, fwd1);
	VectorNormalize(fwd1);

	dot = DotProduct( fwd1, fwd2 );

	//check the positioning in relation to the gun as well
	if ( dot < 0.6f )
	{
		goto tryHeal;
	}

	self->genericValue1 = 1;

	oldWeapon = activator->s.weapon;

	// swap the users weapon with the emplaced gun
	activator->client->ps.weapon = self->s.weapon;
	activator->client->ps.weaponstate = WEAPON_READY;
	activator->client->ps.stats[STAT_WEAPONS] |= ( 1 << WP_EMPLACED_GUN );

	activator->client->ps.emplacedIndex = self->s.number;

	self->s.emplacedOwner = activator->s.number;
	self->s.activeForcePass = NUM_FORCE_POWERS+1;

	// the gun will track which weapon we used to have
	self->s.weapon = oldWeapon;

	//user's new owner becomes the gun ent
	activator->r.ownerNum = self->s.number;
	self->activator = activator;

	VectorSubtract(self->r.currentOrigin, activator->client->ps.origin, anglesToOwner);
	vectoangles(anglesToOwner, anglesToOwner);
	return;

tryHeal: //well, not in the right dir, try healing it instead...
	TryHeal(activator, self);
}

void emplaced_gun_realuse( gentity_t *self, gentity_t *other, gentity_t *activator )
{
	emplaced_gun_use(self, other, NULL);
}

//----------------------------------------------------------
void emplaced_gun_pain( gentity_t *self, gentity_t *attacker, int damage )
{
	self->s.health = self->health;

	if ( self->health <= 0 )
	{
		//death effect.. for now taken care of on cgame
	}
	else
	{
		//if we have a pain behavior set then use it I guess
		G_ActivateBehavior( self, BSET_PAIN );
	}
}

#define EMPLACED_GUN_HEALTH 800

//----------------------------------------------------------
void emplaced_gun_update(gentity_t *self)
{
	vec3_t	smokeOrg, puffAngle;
	int oldWeap;
	float ownLen = 0;

	if (self->health < 1 && !self->genericValue5)
	{ //we are dead, set our respawn delay if we have one
		if (self->spawnflags & EMPLACED_CANRESPAWN)
		{
			self->genericValue5 = level.time + 4000 + self->count;
		}
	}
	else if (self->health < 1 && self->genericValue5 < level.time)
	{ //we are dead, see if it's time to respawn
		self->s.time = 0;
		self->genericValue4 = 0;
		self->genericValue3 = 0;
		self->health = EMPLACED_GUN_HEALTH*0.4;
		self->s.health = self->health;
	}

	if (self->genericValue4 && self->genericValue4 < 2 && self->s.time < level.time)
	{ //we have finished our warning (red flashing) effect, it's time to finish dying
		vec3_t explOrg;

		VectorSet( puffAngle, 0, 0, 1 );

		VectorCopy(self->r.currentOrigin, explOrg);
		explOrg[2] += 16;

		//just use the detpack explosion effect
		G_PlayEffect(EFFECT_EXPLOSION_DETPACK, explOrg, puffAngle);

		self->genericValue3 = level.time + Q_irand(2500, 3500);

		G_RadiusDamage(self->r.currentOrigin, self, self->splashDamage, self->splashRadius, self, NULL, MOD_UNKNOWN);

		self->s.time = -1;

		self->genericValue4 = 2;
	}

	if (self->genericValue3 > level.time)
	{ //see if we are freshly dead and should be smoking
		if (self->genericValue2 < level.time)
		{ //is it time yet to spawn another smoke puff?
			VectorSet( puffAngle, 0, 0, 1 );
			VectorCopy(self->r.currentOrigin, smokeOrg);

			smokeOrg[2] += 60;

			G_PlayEffect(EFFECT_SMOKE, smokeOrg, puffAngle);
			self->genericValue2 = level.time + Q_irand(250, 400);
		}
	}

	if (self->activator && self->activator->client && self->activator->inuse)
	{ //handle updating current user
		vec3_t vLen;
		VectorSubtract(self->s.origin, self->activator->client->ps.origin, vLen);
		ownLen = VectorLength(vLen);

		if (!(self->activator->client->pers.cmd.buttons & BUTTON_USE) && self->genericValue1)
		{//RACC - finished attaching the player to the turret.
			self->genericValue1 = 0;
		}

		if ((self->activator->client->pers.cmd.buttons & BUTTON_USE) && !self->genericValue1)
		{//RACC - trigger start to get off the emplaced turret
			self->activator->client->ps.emplacedIndex = 0;
			self->activator->client->ps.saberHolstered = 0;
			self->nextthink = level.time + 50;
			return;
		}
	}

	if ((self->activator && self->activator->client) &&
		(!self->activator->inuse || self->activator->client->ps.emplacedIndex != self->s.number || self->genericValue4 || ownLen > 64))
	{ //get the user off of me then
		self->activator->client->ps.stats[STAT_WEAPONS] &= ~(1<<WP_EMPLACED_GUN);
		//[CoOp]
		//SP Turrets have ammo, remove the ammo from the player
		self->activator->client->ps.ammo[weaponData[WP_EMPLACED_GUN].ammoIndex] = 0;
		//[/CoOp]

		oldWeap = self->activator->client->ps.weapon;
		self->activator->client->ps.weapon = self->s.weapon;
		self->s.weapon = oldWeap;
		self->activator->r.ownerNum = ENTITYNUM_NONE;
		self->activator->client->ps.emplacedTime = level.time + 1000;
		self->activator->client->ps.emplacedIndex = 0;
		self->activator->client->ps.saberHolstered = 0;
		self->activator = NULL;

		self->s.activeForcePass = 0;
	}
	else if (self->activator && self->activator->client)
	{ //make sure the user is using the emplaced gun weapon
		self->activator->client->ps.weapon = WP_EMPLACED_GUN;
		self->activator->client->ps.weaponstate = WEAPON_READY;
		//[CoOp]
		//SP Turrets have ammo.  increase the amount of ammo the player has for this turret.
		if(self->activator->client->ps.ammo[weaponData[WP_EMPLACED_GUN].ammoIndex] < ammoData[weaponData[WP_EMPLACED_GUN].ammoIndex].max)
		{
			self->activator->client->ps.ammo[weaponData[WP_EMPLACED_GUN].ammoIndex]++;
		}
		//[/CoOp]
	}
	self->nextthink = level.time + 50;
}

//----------------------------------------------------------
void emplaced_gun_die( gentity_t *self, gentity_t *inflictor, gentity_t *attacker, int damage, int mod )
{ //set us up to flash and then explode
	if (self->genericValue4)
	{
		return;
	}

	self->genericValue4 = 1;

	self->s.time = level.time + 3000;

	self->genericValue5 = 0;
}

void SP_emplaced_gun( gentity_t *ent )
{
	const char *name = "models/map_objects/mp/turret_chair.glm";
	vec3_t down;
	trace_t tr;

	//make sure our assets are precached
	RegisterItem( BG_FindItemForWeapon(WP_EMPLACED_GUN) );

	ent->r.contents = CONTENTS_SOLID;
	ent->s.solid = SOLID_BBOX;

	ent->genericValue5 = 0;

	VectorSet( ent->r.mins, -30, -20, 8 );
	VectorSet( ent->r.maxs, 30, 20, 60 );

	VectorCopy(ent->s.origin, down);

	down[2] -= 1024;

	trap_Trace(&tr, ent->s.origin, ent->r.mins, ent->r.maxs, down, ent->s.number, MASK_SOLID);

	if (tr.fraction != 1 && !tr.allsolid && !tr.startsolid)
	{
		VectorCopy(tr.endpos, ent->s.origin);
	}

	ent->spawnflags |= 4; // deadsolid

	ent->health = EMPLACED_GUN_HEALTH;

	if (ent->spawnflags & EMPLACED_CANRESPAWN)
	{ //make it somewhat easier to kill if it can respawn
		ent->health *= 0.4;
	}

	ent->maxHealth = ent->health;
	G_ScaleNetHealth(ent);

	ent->genericValue4 = 0;

	ent->takedamage = qtrue;
	ent->pain = emplaced_gun_pain;
	ent->die = emplaced_gun_die;

	// being caught in this thing when it blows would be really bad.
	ent->splashDamage = 80;
	ent->splashRadius = 128;

	// amount of ammo that this little poochie has
	G_SpawnInt( "count", "600", &ent->count );

	G_SpawnFloat( "constraint", "60", &ent->s.origin2[0] );

	ent->s.modelindex = G_ModelIndex( (char *)name );
	ent->s.modelGhoul2 = 1;
	ent->s.g2radius = 110;

	//so the cgame knows for sure that we're an emplaced weapon
	ent->s.weapon = WP_EMPLACED_GUN;

	G_SetOrigin( ent, ent->s.origin );
	
	// store base angles for later
	VectorCopy( ent->s.angles, ent->pos1 );
	VectorCopy( ent->s.angles, ent->r.currentAngles );
	VectorCopy( ent->s.angles, ent->s.apos.trBase );

	ent->think = emplaced_gun_update;
	ent->nextthink = level.time + 50;

	ent->use = emplaced_gun_realuse;

	ent->r.svFlags |= SVF_PLAYER_USABLE;

	ent->s.pos.trType = TR_STATIONARY;

	ent->s.owner = MAX_CLIENTS+1;
	ent->s.shouldtarget = qtrue;
	//ent->s.teamowner = 0;

	trap_LinkEntity(ent);
}

//[WEAPONSDAT]
void WP_PrimaryFireStunBaton( gentity_t* ent )
{
	WP_FireStunBaton(ent, qfalse);
}
void WP_PrimaryFireMelee( gentity_t* ent )
{
	WP_FireMelee(ent, qfalse);
}
void WP_PrimaryFireBryarPistol( gentity_t* ent )
{
	WP_FireBryarPistol(ent, qfalse);
}
void WP_PrimaryFireConcussion( gentity_t* ent )
{
	WP_FireConcussion(ent, qfalse);
}
void WP_PrimaryFireBryarOld( gentity_t* ent )
{
	WP_FireBowcaster(ent, qfalse);
}
void WP_PrimaryFireBlaster( gentity_t* ent )
{
	WP_FireBowcaster(ent, qfalse);
}
void WP_PrimaryFireDisruptor( gentity_t* ent )
{
	WP_FireDisruptor(ent, qfalse);
}
void WP_PrimaryFireBowcaster( gentity_t* ent )
{
	WP_FireBowcaster(ent, qfalse);
}
void WP_PrimaryFireRepeater( gentity_t* ent )
{
	WP_FireBowcaster(ent, qfalse);
}
void WP_PrimaryFireDEMP2( gentity_t* ent )
{
	WP_FireDEMP2(ent, qfalse);
}
void WP_PrimaryFireFlechette( gentity_t* ent )
{
	WP_FireFlechette(ent, qfalse);
}
void WP_PrimaryFireRocket( gentity_t* ent )
{
	WP_FireRocket(ent, qfalse);
}
void WP_PrimaryFireThermalDetonator( gentity_t* ent )
{
	WP_FireThermalDetonator(ent, qfalse);
}
void WP_PrimaryPlaceLaserTrap( gentity_t* ent )
{
	WP_FireFlashbang(ent, qfalse);
}
void WP_PrimaryDropDetPack( gentity_t* ent )
{
	WP_DropDetPack(ent, qfalse);
}
void WP_PrimaryFireEmplaced( gentity_t* ent )
{
	WP_FireEmplaced(ent, qfalse);
}

void WP_AltFireStunBaton( gentity_t* ent )
{
	WP_FireStunBaton(ent, qtrue);
}
void WP_AltFireMelee( gentity_t* ent )
{
	WP_FireMelee(ent, qtrue);
}
void WP_AltFireBryarPistol( gentity_t* ent )
{
	WP_FireBryarPistol(ent, qtrue);
}
void WP_AltFireConcussion( gentity_t* ent )
{
	WP_FireConcussion(ent, qfalse);
}
void WP_AltFireBryarOld( gentity_t* ent )
{
	WP_FireBowcaster(ent, qtrue);
}
void WP_AltFireBlaster( gentity_t* ent )
{
	WP_FireBowcaster(ent, qtrue);
}
void WP_AltFireDisruptor( gentity_t* ent )
{
	WP_FireDisruptor(ent, qtrue);
}
void WP_AltFireBowcaster( gentity_t* ent )
{
	WP_FireBowcaster(ent, qtrue);
}
void WP_AltFireRepeater( gentity_t* ent )
{
	WP_FireRepeater(ent, qtrue);
}
void WP_AltFireDEMP2( gentity_t* ent )
{
	WP_FireDEMP2(ent, qtrue);
}
void WP_AltFireFlechette( gentity_t* ent )
{
	WP_FireFlechette(ent, qtrue);
}
void WP_AltFireRocket( gentity_t* ent )
{
	WP_FireRocket(ent, qtrue);
}
void WP_AltFireThermalDetonator( gentity_t* ent )
{
	//WP_FireThermalDetonator(ent, qtrue);
}
void WP_AltPlaceLaserTrap( gentity_t* ent )
{
	WP_FireFlashbang(ent, qtrue);
}
void WP_AltDropDetPack( gentity_t* ent )
{
	WP_DropDetPack(ent, qtrue);
}
void WP_AltFireEmplaced( gentity_t* ent )
{
	WP_FireEmplaced(ent, qtrue);
}
//[/WEAPONSDAT]


//[CoOp]
/*QUAKED emplaced_eweb (0 0 1) (-12 -12 -24) (12 12 24) INACTIVE FACING INVULNERABLE PLAYERUSE

 INACTIVE cannot be used until used by a target_activate
 FACING - player must be facing relatively in the same direction as the gun in order to use it
 VULNERABLE - allow the gun to take damage
 PLAYERUSE - only the player makes it run its usescript

 count - how much ammo to give this gun ( default 999 )
 health - how much damage the gun can take before it blows ( default 250 )
 delay - ONLY AFFECTS NPCs - time between shots ( default 200 on hardest setting )
 wait - ONLY AFFECTS NPCs - time between bursts ( default 800 on hardest setting )
 splashdamage - how much damage a blowing up gun deals ( default 80 )
 splashradius - radius for exploding damage ( default 128 )

 scripts:
	will run usescript, painscript and deathscript
*/
void SP_emplaced_eweb( gentity_t *ent )
{
	//temp replacing the eweb with an emplaced gun so we can at least get the AI/scripting
	//running for it first.
	SP_emplaced_gun(ent);
}
//[/CoOp]


