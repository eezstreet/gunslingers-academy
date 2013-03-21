#include "g_local.h"
#include "g_weapon.h"

extern vec3_t	wp_forward, wp_vright, wp_up;
extern vec3_t	wp_muzzle;
extern void W_TraceSetStart( gentity_t *ent, vec3_t start, vec3_t mins, vec3_t maxs );
extern int G_GetHitLocation(gentity_t *target, vec3_t ppoint);

#pragma warning(disable : 4701) //local variable may be used without having been initialized
void WP_FireConcussionAlt( gentity_t *ent )
{//a rail-gun-like beam
	weaponData_t	*wp				= &weaponData[ent->s.weapon];
	int			damage = wp->altDamage, skip, traces = DISRUPTOR_ALT_TRACES;
	qboolean	render_impact = qtrue;
	vec3_t		start, end;
	vec3_t		muzzle2, dir;
	trace_t		tr;
	gentity_t	*traceEnt, *tent;
	float		shotRange = 8192.0f;
	qboolean	hitDodged = qfalse;
	vec3_t shot_mins, shot_maxs;
	int			i;

	//Shove us backwards for half a second
	/*VectorMA( ent->client->ps.velocity, -200, wp_forward, ent->client->ps.velocity );
	ent->client->ps.groundEntityNum = ENTITYNUM_NONE;
	if ( (ent->client->ps.pm_flags&PMF_DUCKED) )
	{//hunkered down
		ent->client->ps.pm_time = 100;
	}
	else
	{
		ent->client->ps.pm_time = 250;
	}*/
//	ent->client->ps.pm_flags |= PMF_TIME_KNOCKBACK|PMF_TIME_NOFRICTION;
	//FIXME: only if on ground?  So no "rocket jump"?  Or: (see next FIXME)
	//FIXME: instead, set a forced ucmd backmove instead of this sliding

	VectorCopy( wp_muzzle, muzzle2 ); // making a backwp_up copy

	VectorCopy( wp_muzzle, start );
	W_TraceSetStart( ent, start, vec3_origin, vec3_origin );

	skip = ent->s.number;

//	if ( ent->client && ent->client->ps.powerwp_ups[PW_WEAPON_OVERCHARGE] > 0 && ent->client->ps.powerwp_ups[PW_WEAPON_OVERCHARGE] > cg.time )
//	{
//		// in overcharge mode, so doing double damage
//		damage *= 2;
//	}
	
	//Make it a little easier to hit guys at long range
	VectorSet( shot_mins, -1, -1, -1 );
	VectorSet( shot_maxs, 1, 1, 1 );

	for ( i = 0; i < traces; i++ )
	{
		VectorMA( start, shotRange, wp_forward, end );

		//NOTE: if you want to be able to hit guys in emplaced guns, use "G2_COLLIDE, 10" instead of "G2_RETURNONHIT, 0"
		//alternately, if you end wp_up hitting an emplaced_gun that has a sitter, just redo this one trace with the "G2_COLLIDE, 10" to see if we it the sitter
		//gi.trace( &tr, start, NULL, NULL, end, skip, MASK_SHOT, G2_COLLIDE, 10 );//G2_RETURNONHIT, 0 );
		if (d_projectileGhoul2Collision.integer)
		{
			trap_G2Trace( &tr, start, shot_mins, shot_maxs, end, skip, MASK_SHOT, G2TRFLAG_DOGHOULTRACE|G2TRFLAG_GETSURFINDEX|G2TRFLAG_HITCORPSES, g_g2TraceLod.integer );
		}
		else
		{
			trap_Trace( &tr, start, shot_mins, shot_maxs, end, skip, MASK_SHOT );
		}

		traceEnt = &g_entities[tr.entityNum];

		if (d_projectileGhoul2Collision.integer && traceEnt->inuse && traceEnt->client)
		{ //g2 collision checks -rww
			if (traceEnt->inuse && traceEnt->client && traceEnt->ghoul2)
			{ //since we used G2TRFLAG_GETSURFINDEX, tr.surfaceFlags will actually contain the index of the surface on the ghoul2 model we collided with.
				traceEnt->client->g2LastSurfaceHit = tr.surfaceFlags;
				traceEnt->client->g2LastSurfaceTime = level.time;
				//[BugFix12]
				//BUGFIX12RAFIXME - ugh, can't seem to get the model index on the 
				//trap_G2Traces.  These probably need to be replaced with the more
				//indepth G2traces.  For now, just assume that the player model was hit.
				traceEnt->client->g2LastSurfaceModel = G2MODEL_PLAYER;
				//[/BugFix12]
			}

			if (traceEnt->ghoul2)
			{
				tr.surfaceFlags = 0; //clear the surface flags after, since we actually care about them in here.
			}
		}
		if ( tr.surfaceFlags & SURF_NOIMPACT ) 
		{
			render_impact = qfalse;
		}

		if ( tr.entityNum == ent->s.number )
		{
			// should never happen, but basically we don't want to consider a hit to ourselves?
			// Get ready for an attempt to trace through another person
			VectorCopy( tr.endpos, muzzle2 );
			VectorCopy( tr.endpos, start );
			skip = tr.entityNum;
#ifdef _DEBUG
			Com_Printf( "BAD! Concussion gun shot somehow traced back and hit the owner!\n" );			
#endif
			continue;
		}

		// always render a shot beam, doing this the old way because I don't much feel like overriding the effect.
		//NOTE: let's just draw one beam at the end
		//tent = G_TempEntity( tr.endpos, EV_CONC_ALT_SHOT );
		//tent->svFlags |= SVF_BROADCAST;

		//VectorCopy( muzzle2, tent->s.origin2 );

		if ( tr.fraction >= 1.0f )
		{
			// draw the beam but don't do anything else
			break;
		}

		if ( traceEnt->s.weapon == WP_SABER )//&& traceEnt->NPC 
		{//FIXME: need a more reliable way to know we hit a jedi?
			hitDodged = Jedi_DodgeEvasion( traceEnt, ent, &tr, HL_NONE );
			//acts like we didn't even hit him
		}
		if ( !hitDodged )
		{
			if ( render_impact )
			{
				if (( tr.entityNum < ENTITYNUM_WORLD && traceEnt->takedamage ) 
					|| !Q_stricmp( traceEnt->classname, "misc_model_breakable" ) 
					|| traceEnt->s.eType == ET_MOVER )
				{
					qboolean noKnockBack;

					// Create a simple impact type mark that doesn't last long in the world
					//G_PlayEffectID( G_EffectIndex( "concussion/alt_hit" ), tr.endpos, tr.plane.normal );
					//no no no

					if ( traceEnt->client && LogAccuracyHit( traceEnt, ent )) 
					{//NOTE: hitting multiple ents can still get you over 100% accuracy
						ent->client->accuracy_hits++;
					} 

					noKnockBack = (traceEnt->flags&FL_NO_KNOCKBACK);//will be set if they die, I want to know if it was on *before* they died
					if ( traceEnt && traceEnt->client && traceEnt->client->NPC_class == CLASS_GALAKMECH )
					{//hehe
						G_Damage( traceEnt, ent, ent, wp_forward, tr.endpos, 10, DAMAGE_NO_KNOCKBACK|DAMAGE_NO_HIT_LOC, MOD_CONC_ALT );
						break;
					}
					G_Damage( traceEnt, ent, ent, wp_forward, tr.endpos, damage, DAMAGE_NO_KNOCKBACK|DAMAGE_NO_HIT_LOC, MOD_CONC_ALT );

					//do knockback and knockdown manually
					if ( traceEnt->client )
					{//only if we hit a client
						vec3_t pushDir;
						VectorCopy( wp_forward, pushDir );
						if ( pushDir[2] < 0.2f )
						{
							pushDir[2] = 0.2f;
						}//hmm, re-normalize?  nah...
						/*
						if ( !noKnockBack )
						{//knock-backable
							G_Throw( traceEnt, pushDir, 200 );
						}
						*/
						if ( traceEnt->health > 0 )
						{//alive
							//if ( G_HasKnockdownAnims( traceEnt ) )
							if (!noKnockBack && !traceEnt->localAnimIndex && traceEnt->client->ps.forceHandExtend != HANDEXTEND_KNOCKDOWN &&
								BG_KnockDownable(&traceEnt->client->ps)) //just check for humanoids..
							{//knock-downable
								//G_Knockdown( traceEnt, ent, pushDir, 400, qtrue );
								vec3_t plPDif;
								float pStr;

								//cap it and stuff, base the strength and whether or not we can knockdown on the distance
								//from the shooter to the target
								VectorSubtract(traceEnt->client->ps.origin, ent->client->ps.origin, plPDif);
								pStr = 500.0f-VectorLength(plPDif);
								if (pStr < 150.0f)
								{
									pStr = 150.0f;
								}
								if (pStr > 200.0f)
								{
									traceEnt->client->ps.forceHandExtend = HANDEXTEND_KNOCKDOWN;
									traceEnt->client->ps.forceHandExtendTime = level.time + 1100;
									traceEnt->client->ps.forceDodgeAnim = 0; //this toggles between 1 and 0, when it's 1 we should play the get wp_up anim
								}
								traceEnt->client->ps.otherKiller = ent->s.number;
								traceEnt->client->ps.otherKillerTime = level.time + 5000;
								traceEnt->client->ps.otherKillerDebounceTime = level.time + 100;
								//[Asteroids]
								traceEnt->client->otherKillerMOD = MOD_UNKNOWN;
								traceEnt->client->otherKillerVehWeapon = 0;
								traceEnt->client->otherKillerWeaponType = WP_NONE;
								//[/Asteroids]

								traceEnt->client->ps.velocity[0] += pushDir[0]*pStr;
								traceEnt->client->ps.velocity[1] += pushDir[1]*pStr;
								traceEnt->client->ps.velocity[2] = pStr;
							}
						}
					}

					if ( traceEnt->s.eType == ET_MOVER )
					{//stop the traces on any mover
						break;
					}
				}
				else 
				{
					 // we only make this mark on things that can't break or move
				//	tent = G_TempEntity(tr.endpos, EV_MISSILE_MISS);
				//	tent->s.eventParm = DirToByte(tr.plane.normal);
				//	tent->s.eFlags |= EF_ALT_FIRING;

					//tent->svFlags |= SVF_BROADCAST;
					//eh? why broadcast?
				//	VectorCopy( tr.plane.normal, tent->pos1 );

					//mmm..no..don't do this more than once for no reason whatsoever.
					break; // hit solid, but doesn't take damage, so stop the shot...we _could_ allow it to shoot through walls, might be cool?
				}
			}
			else // not rendering impact, must be a skybox or other similar thing?
			{
				break; // don't try anymore traces
			}
		}
		// Get ready for an attempt to trace through another person
		VectorCopy( tr.endpos, muzzle2 );
		VectorCopy( tr.endpos, start );
		skip = tr.entityNum;
		hitDodged = qfalse;
	}
	//just draw one beam all the way to the end
//	tent = G_TempEntity( tr.endpos, EV_CONC_ALT_SHOT );
//	tent->svFlags |= SVF_BROADCAST;
	//again, why broadcast?

//	tent = G_TempEntity(tr.endpos, EV_MISSILE_MISS);
//	tent->s.eventParm = DirToByte(tr.plane.normal);
//	tent->s.eFlags |= EF_ALT_FIRING;
//	VectorCopy( muzzle, tent->s.origin2 );

	// now go along the trail and make sight events
	VectorSubtract( tr.endpos, wp_muzzle, dir );

//	shotDist = VectorNormalize( dir );

	//let's pack all this junk into a single tempent, and send it off.
	tent = G_TempEntity(tr.endpos, EV_CONC_ALT_IMPACT);
	tent->s.eventParm = DirToByte(tr.plane.normal);
	tent->s.owner = ent->s.number;
	VectorCopy(dir, tent->s.angles);
	VectorCopy(wp_muzzle, tent->s.origin2);
	VectorCopy(wp_forward, tent->s.angles2);

#if 0 //yuck
	//FIXME: if shoot *really* close to someone, the alert could be way out of their FOV
	for ( dist = 0; dist < shotDist; dist += 64 )
	{
		//FIXME: on a really long shot, this could make a LOT of alerts in one frame...
		VectorMA( muzzle, dist, dir, spot );
		AddSightEvent( ent, spot, 256, AEL_DISCOVERED, 50 );
		//FIXME: creates *way* too many effects, make it one effect somehow?
		G_PlayEffectID( G_EffectIndex( "concussion/alt_ring" ), spot, actualAngles );
	}
	//FIXME: spawn a temp ent that continuously spawns sight alerts here?  And 1 sound alert to draw their attention?
	VectorMA( start, shotDist-4, wp_forward, spot );
	AddSightEvent( ent, spot, 256, AEL_DISCOVERED, 50 );

	G_PlayEffectID( G_EffectIndex( "concussion/altmuzzle_flash" ), muzzle, wp_forward );
#endif
}
#pragma warning(default : 4701) //local variable may be used without having been initialized

void WP_FireConcussionPrimary( gentity_t *ent, vec3_t dir )
{//a fast rocket-like projectile
	weaponData_t	*wp				= &weaponData[ent->s.weapon];
	int	damage	= wp->damage;
	gentity_t *missile; 
	vec3_t start;

	VectorCopy(wp_muzzle, start);

	if(ent->client)
	{
		if(ent->client->ps.zoomMode == 1 || ent->client->pers.cmd.buttons & BUTTON_SIGHTS /*&& weaponNum < WP_AUXILIARY1*/)
		{
			VectorCopy( ent->client->ps.origin, start );
			start[2] += ent->client->ps.viewheight;//By eyes
		}
	}

	missile = CreateMissile( start, dir, REPEATER_VELOCITY, 10000, ent, qfalse );
	if(g_instagib.integer == 1)
		damage = 3000;

	missile->classname = "repeater_proj";
	missile->s.weapon = WP_REPEATER;

	missile->damage = damage;
	missile->dflags = DAMAGE_DEATH_KNOCKBACK;
	missile->methodOfDeath = wp->altMethodOfDeath;
	missile->clipmask = MASK_SHOT | CONTENTS_LIGHTSABER;

	// we don't want it to bounce forever
	missile->bounceCount = 8;
}

void WP_FireConcussion( gentity_t *ent, qboolean altFire )
{
	vec3_t	dir, angs;
	if(altFire)
	{
		WP_FireConcussionAlt( ent );
		return;
	}

	vectoangles( wp_forward, angs );

	angs[PITCH] += crandom() * ( ent->client->ps.stats[STAT_ACCURACY] * weaponData[ent->client->ps.weapon].accuracyThreshold );
	angs[YAW] += crandom() * ( ent->client->ps.stats[STAT_ACCURACY] * weaponData[ent->client->ps.weapon].accuracyThreshold );

	AngleVectors( angs, dir, NULL, NULL );
	WP_FireConcussionPrimary(ent, dir);
}