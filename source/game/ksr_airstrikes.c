#include "g_local.h"
#include "g_weapon.h"

vec3_t	forward, vright, up;
vec3_t	muzzle;
extern vec3_t	wp_forward, wp_vright, wp_up;
extern vec3_t	wp_muzzle;

extern void touch_NULL( gentity_t *ent, gentity_t *other, trace_t *trace );
extern void WP_TraceSetStart( gentity_t *ent, vec3_t start, vec3_t mins, vec3_t maxs );

//MISSILE STRIKES
//These are not very spectacular; a missile just comes from the sky and flies around drunkenly towards its target
extern void rpgThink( gentity_t *ent );
extern void RocketDie(gentity_t *self, gentity_t *inflictor, gentity_t *attacker, int damage, int mod);
void KS_MissileBombThink(gentity_t *ent);
void KS_MissileBombActivate(gentity_t *ent)
{
	vec3_t	dir={0,0,1};
	int i;

	if(!(qboolean)AirspaceForPoint(ent->r.currentOrigin))
	{
		trap_SendServerCommand(ent->s.owner, "print \"^3WARNING: (Debug) Airstrike not within valid airspace, refunding airstrike...\n");
		g_entities[ent->s.owner].client->ps.stats[STAT_HOLDABLE_ITEMS] |= (1 << HI_AIRSTRIKE);
		trap_LinkEntity( ent );
		return;
	}
	else
	{
		//Shoot it!
		for(i = 0; i < 4; i++) //shoot eight missiles, at varying start origins but flying towards the same general point.
			//Uses an RPG think function
		{
			int damage = 500;
			int	vel = ROCKET_VELOCITY*2;
			gentity_t *missile;
			vec3_t origin, c, angles;
			vec3_t hitspot;

			VectorCopy(ent->r.currentOrigin, hitspot);
			hitspot[0] += (float)Q_irand(-300,300);
			hitspot[1] += (float)Q_irand(-300,300);
			//CHOOSE YOUR ORIGINS
			origin[0] = (hitspot[0] + (float)Q_irand(-512,512));
			origin[1] = (hitspot[1] + (float)Q_irand(-512,512));
			origin[2] = (AirspaceForPoint(ent->r.currentOrigin)->r.absmax[2] - 8); //give us a little negative Z overhead to avoid unintended explosions

			//Next, get some angles
			VectorSubtract(hitspot, origin, c);
			VectorNormalize(c);
			//vectoangles(c, angles);
			angles[2] = 0;
			//Create this sucker.
			missile = CreateMissile( origin, c, vel, 10000, &g_entities[ent->s.owner], qfalse );

			missile->classname = "rocket_proj";
			missile->s.weapon = WP_ROCKET_LAUNCHER;

			VectorSet( missile->r.maxs, ROCKET_SIZE, ROCKET_SIZE, ROCKET_SIZE );
			VectorScale( missile->r.maxs, -1, missile->r.mins );

			missile->r.ownerNum = ent->r.ownerNum;
			missile->s.owner = ent->r.ownerNum;

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
			missile->think = rpgThink;
			missile->nextthink = level.time + 2;
			missile->die = RocketDie;

			missile->clipmask = MASK_SHOT;
			missile->splashDamage = ROCKET_SPLASH_DAMAGE;
			missile->splashRadius = ROCKET_SPLASH_RADIUS*2;
			missile->bounceCount = 0;
		}
	}
	ent->genericValue2++;
	if(ent->genericValue2 > 10)
	{
		G_FreeEntity( ent );
		trap_LinkEntity( ent );
	}
	else
	{
		ent->think = KS_MissileBombThink;
		ent->nextthink = level.time + 700;
	}
}

void KS_MissileBombThink(gentity_t *ent)
{
	if(ent->genericValue2 > 0)
	{
		ent->think = KS_MissileBombActivate;
		ent->nextthink = level.time;
	}
	if (ent->genericValue5 < level.time)
	{
		ent->think = KS_MissileBombActivate;
		ent->nextthink = level.time;
		return;
	}

	G_RunObject(ent);
	ent->nextthink = level.time;
}
gentity_t *KS_FireMissileBomb( gentity_t *ent )
{
	gentity_t *bolt;
	vec3_t		dir, start;

	AngleVectors( ent->client->ps.viewangles, forward, vright, up );
	CalcMuzzlePoint( ent, forward, vright, up, muzzle );

	VectorCopy( forward, dir );
	VectorCopy( muzzle, start );

	bolt = G_Spawn();
	
	bolt->physicsObject = qtrue;

	bolt->classname = "thermal_detonator";
	bolt->think = KS_MissileBombThink;
	bolt->nextthink = level.time;
	bolt->touch = touch_NULL;

	// How 'bout we give this thing a size...
	VectorSet( bolt->r.mins, -3.0f, -3.0f, -3.0f );
	VectorSet( bolt->r.maxs, 3.0f, 3.0f, 3.0f );
	bolt->clipmask = MASK_SHOT;

	WP_TraceSetStart( ent, start, bolt->r.mins, bolt->r.maxs );//make sure our start point isn't on the other side of a wall

	bolt->genericValue5 = level.time + 2000; // How long 'til she blows
	bolt->s.pos.trType = TR_GRAVITY;
	bolt->parent = ent;
	bolt->r.ownerNum = ent->s.number;
	VectorScale( dir, 900, bolt->s.pos.trDelta );

	if ( ent->health >= 0 )
	{
		bolt->s.pos.trDelta[2] += 120;
	}

	bolt->flags |= FL_BOUNCE_HALF;
	bolt->damage = 0;
	bolt->dflags = 0;
	bolt->splashDamage = 0;
	bolt->splashRadius = 0;

	bolt->s.eType = ET_MISSILE;
	bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
	bolt->s.weapon = WP_TRIP_MINE;

	bolt->s.pos.trTime = level.time;		// move a bit on the very first frame
	bolt->s.activeForcePass = 1;			// Hack -> Use flame trail
	VectorCopy( start, bolt->s.pos.trBase );
	
	SnapVector( bolt->s.pos.trDelta );			// save net bandwidth
	VectorCopy (start, bolt->r.currentOrigin);

	VectorCopy( start, bolt->pos2 );

	bolt->s.owner = ent->s.number;

	bolt->bounceCount = -5;
	bolt->genericValue2;

	return bolt;
}