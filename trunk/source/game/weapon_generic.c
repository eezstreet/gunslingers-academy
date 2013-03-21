#include "g_local.h"
#include "g_weapon.h"

extern vmCvar_t g_friendlyFire;
extern vec3_t	wp_forward, wp_vright, wp_up;
extern vec3_t	wp_muzzle;
void WP_FireTraceLine(gentity_t *ent, int weapon, qboolean altFire){
	NEWVEC3(start);
	NEWVEC3(end);
	NEWVEC3(offset);
	NEWTR(tr);
	NEWENT(traceEnt);
	NEWENT(tent);

	float shotRange			= 8192.0f;
	int ignore				= ent->s.number;
	qboolean renderImpact	= qtrue;
	int accuracy;
	weaponData_t *wp		= &weaponData[weapon];
	//eezstreet start
	if(ent->NPC)
		accuracy = ent->NPC->currentAim;
	else
		accuracy = ent->client->ps.stats[STAT_ACCURACY];
	//eezstreet end

	VectorCopy( wp_muzzle, start );

	VectorMA( start, shotRange, wp_forward, end );

	offset[0]	+= crandom() * ( accuracy * wp->accuracyThreshold ) * 200.0f;
	offset[1]	+= crandom() * ( accuracy * wp->accuracyThreshold ) * 200.0f;
	offset[2]	+= crandom() * ( accuracy * wp->accuracyThreshold ) * 200.0f;
	VectorAdd( offset, end, end );

	if(!g_friendlyFire.integer){
		do {
			trap_G2Trace( &tr, start, NULL, NULL, end, ignore, MASK_SHOT, G2TRFLAG_DOGHOULTRACE|G2TRFLAG_GETSURFINDEX|G2TRFLAG_THICK/*|G2TRFLAG_HITCORPSES*/, g_g2TraceLod.integer );
			VectorMA( tr.endpos, 16.0f, wp_forward, start );
		}
		while ( tr.entityNum && tr.entityNum < MAX_CLIENTS );
	}
	else
		trap_G2Trace( &tr, start, NULL, NULL, end, ignore, MASK_SHOT, G2TRFLAG_DOGHOULTRACE|G2TRFLAG_GETSURFINDEX|G2TRFLAG_THICK, g_g2TraceLod.integer );

	traceEnt = &g_entities[tr.entityNum];

	if ( d_projectileGhoul2Collision.integer && traceEnt->inuse && traceEnt->client )
	{ //g2 collision checks -rww
		if ( traceEnt->inuse && traceEnt->client && traceEnt->ghoul2 )
		{//since we used G2TRFLAG_GETSURFINDEX, tr.surfaceFlags will actually contain the index of the surface on the ghoul2 model we collided with.
			traceEnt->client->g2LastSurfaceHit = tr.surfaceFlags;
			traceEnt->client->g2LastSurfaceTime = level.time;
		}

		if ( traceEnt->ghoul2 )
			tr.surfaceFlags = 0; //clear the surface flags after, since we actually care about them in here.
	}

	if ( traceEnt->flags & FL_SHIELDED )
		return;

	if ( tr.surfaceFlags & SURF_NOIMPACT ) 
		renderImpact = qfalse;

	//STCFIXME: Send in the one event? We really should...
	//	Trail
	tent = G_TempEntity( tr.endpos, EV_DISRUPTOR_MAIN_SHOT );
	VectorCopy( wp_muzzle, tent->s.origin2 );
	tent->s.eventParm = ent->s.number;

	//	Impact
	tent = G_TempEntity( tr.endpos, EV_DISRUPTOR_HIT );
	tent->s.generic1 = DirToByte( tr.plane.normal );
	tent->s.weapon = 1;

	if ( tr.entityNum < ENTITYNUM_WORLD && traceEnt->takedamage ){
		if(altFire)
			G_Damage( traceEnt, ent, ent, wp_forward, tr.endpos, wp->altDamage, DAMAGE_NORMAL, wp->altMethodOfDeath);
		else
			G_Damage( traceEnt, ent, ent, wp_forward, tr.endpos, wp->damage, DAMAGE_NORMAL, wp->methodOfDeath );
	}
}