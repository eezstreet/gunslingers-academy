#include "g_weapon.h"
#include "g_local.h"

extern void WP_FireTraceLine(gentity_t *ent, int weapon, qboolean altFire);
extern vec3_t	wp_forward, wp_vright, wp_up;
extern vec3_t	wp_muzzle;
/*
======================================================================

DEMP2

======================================================================
*/

//---------------------------------------------------------
void WP_FireDEMP2( gentity_t *ent, qboolean altFire )
//---------------------------------------------------------
{
	qboolean	render_impact = qtrue;
	vec3_t		start, end, offset;
	trace_t		tr;
	gentity_t	*traceEnt, *tent;
	float		shotRange = 8192;
	int			ignore, traces;
	int				accuracy		= ent->client->ps.stats[STAT_ACCURACY];
	weaponData_t	*wp				= &weaponData[WP_DEMP2];
	int damage;
	int methodOfDeath;
	if(altFire){
		damage = wp->altDamage;
		methodOfDeath = wp->altMethodOfDeath;
	}else{
		damage = wp->damage;
		methodOfDeath = wp->methodOfDeath;
	}

	memset(&tr, 0, sizeof(tr)); //to shut the compiler wp_up

	VectorCopy( ent->client->ps.origin, start );
	start[2] += ent->client->ps.viewheight;//By eyes

	offset[0]	= crandom() * ( accuracy * wp->accuracyThreshold ) * 200.0f;
	offset[1]	= crandom() * ( accuracy * wp->accuracyThreshold ) * 200.0f;
	offset[2]	= crandom() * ( accuracy * wp->accuracyThreshold ) * 200.0f;

	VectorMA( start, shotRange, wp_forward, end );

	VectorAdd( offset, end, end );

	ignore = ent->s.number;
	traces = 0;
	while ( traces < 10 )
	{//need to loop this in case we hit a Jedi who dodges the shot
		if (d_projectileGhoul2Collision.integer)
		{
			trap_G2Trace( &tr, start, NULL, NULL, end, ignore, MASK_SHOT, G2TRFLAG_DOGHOULTRACE|G2TRFLAG_GETSURFINDEX|G2TRFLAG_THICK|G2TRFLAG_HITCORPSES, g_g2TraceLod.integer );
		}
		else
		{
			trap_Trace( &tr, start, NULL, NULL, end, ignore, MASK_SHOT );
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

		if (traceEnt && traceEnt->client && traceEnt->client->ps.duelInProgress &&
			traceEnt->client->ps.duelIndex != ent->s.number)
		{
			VectorCopy( tr.endpos, start );
			ignore = tr.entityNum;
			traces++;
			continue;
		}
		break;
	}

	if ( tr.surfaceFlags & SURF_NOIMPACT ) 
	{
		render_impact = qfalse;
	}

	tent = G_TempEntity( tr.endpos, EV_DISRUPTOR_MAIN_SHOT );
	VectorCopy( wp_muzzle, tent->s.origin2 );
	tent->s.eventParm = ent->s.number;

	traceEnt = &g_entities[tr.entityNum];

	if ( render_impact )
	{
		if ( tr.entityNum < ENTITYNUM_WORLD && traceEnt->takedamage )
		{
			if ( traceEnt->client && LogAccuracyHit( traceEnt, ent )) 
			{
				ent->client->accuracy_hits++;
			} 

			G_Damage( traceEnt, ent, ent, wp_forward, tr.endpos, damage, DAMAGE_NORMAL, methodOfDeath );
			
			tent = G_TempEntity( tr.endpos, EV_DISRUPTOR_HIT );
			tent->s.eventParm = DirToByte( tr.plane.normal );
			if (traceEnt->client)
			{
				tent->s.weapon = 1;
			}
		}
		else 
		{
			 // Hmmm, maybe don't make any marks on things that could break
			tent = G_TempEntity( tr.endpos, EV_DISRUPTOR_SNIPER_MISS );
			tent->s.eventParm = DirToByte( tr.plane.normal );
			tent->s.weapon = 1;
		}
	}
}