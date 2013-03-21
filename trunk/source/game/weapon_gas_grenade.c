#include "g_local.h"
#include "g_weapon.h"

vec3_t	forward, vright, up;
vec3_t	muzzle;
vec3_t	forward, vright, up;
vec3_t	muzzle;

#define GASNADE_TIME	2000
#define GAS_TIME		10000

#define GAS_BLIND_AREA	256

extern void touch_NULL( gentity_t *ent, gentity_t *other, trace_t *trace );
extern void WP_TraceSetStart( gentity_t *ent, vec3_t start, vec3_t mins, vec3_t maxs );

qboolean G_PointInBounds( vec3_t point, vec3_t mins, vec3_t maxs );
qboolean GasGuyInOurContents(gentity_t *ent, int num);
//Rundown of what this function does:
//Updates players origins that are supposedly within this trigger then runs a check to see if they're still in this trigger
//If not, then remove their flags.
void trigger_nosee_gas_thinkstandard( gentity_t *ent )
{
	qboolean statusQuo = qtrue; //Status Quota looks the same
	int i;
	int j;
	vec3_t absmin;
	vec3_t absmax;
	VectorAdd(ent->s.origin, ent->r.mins, absmin);
	VectorAdd(ent->s.origin, ent->r.maxs, absmax);

	if(ent->genericValue5 < level.time)
	{
		//Unflag innocent victims...erm...I mean...civilians
		for(i = 0; i < ent->nosee_num; i++)
		{
			g_entities[ent->nosee_contents[i]].s.shouldtarget = qfalse;
		}
		trap_LinkEntity(ent);
		return;
	}
	//Okay...update origins
	for(i = 0; i < ent->nosee_num; i++)
	{
		VectorCopy(g_entities[ent->nosee_contents[i]].r.currentOrigin, ent->nosee_origins[i]);
		if(!G_PointInBounds( ent->nosee_origins[i], absmin, absmax ) )
		{
			//WAIT! You're not in this trigger!
			g_entities[ent->nosee_contents[i]].s.shouldtarget = qfalse;
			ent->nosee_contents[i] = ENTITYNUM_WORLD;
			for(j = i; j < ent->nosee_num-1; j++)
			{
				ent->nosee_contents[j] = ent->nosee_contents[j+1];
			}
			ent->nosee_num--;
			statusQuo = qfalse;
		}
		else
		{ //hurt the bastard
			G_Damage(&g_entities[ent->nosee_contents[i]], ent->parent, ent->parent, NULL, NULL, 5, DAMAGE_NO_PROTECTION, MOD_GAS);
		}
	}

	//Check for new guys in our bounds
	for (i = 0; i < MAX_CLIENTS; i++)
	{
		if(!GasGuyInOurContents(ent, i))
		{ //Potential candidate...
			if(G_PointInBounds(g_entities[i].r.currentOrigin, absmin, absmax))
			{ //OH SNAP
				if(!g_entities[i].client)
				{
					ent->nextthink = level.time;
					return;
				}
				g_entities[i].s.shouldtarget = qtrue;
				ent->nosee_num++;
				ent->nosee_contents[ent->nosee_num-1] = g_entities[i].s.number;
				VectorCopy(g_entities[i].s.origin, ent->nosee_origins[ent->nosee_num-1]);
			}
		}
	}
	ent->nextthink = level.time;
}

//Flags this client initially
void trigger_nosee_gas_touch( gentity_t *self, gentity_t *other, trace_t *trace )
{
	if(!other->client)
		return;

	other->s.shouldtarget = qfalse;
	self->nosee_num++;
	self->nosee_contents[self->nosee_num-1] = other->s.number;
	VectorCopy(other->s.origin, self->nosee_origins[self->nosee_num-1]);
}
//This trigger entity "blinds" the target area, preventing crosshair identification
void SP_trigger_gas_nosee( gentity_t *ent )
{
	int i;
	ent->think = trigger_nosee_gas_thinkstandard;

	for(i = 0; i < MAX_CLIENTS; i++)
	{
		ent->nosee_contents[i] = ENTITYNUM_WORLD;
		VectorCopy(vec3_origin, ent->nosee_origins[i]);
	}
	ent->nosee_num = 0;

	ent->nextthink = level.time + 300;
}

qboolean GasGuyInOurContents(gentity_t *ent, int num)
{
	int i;
	for(i = 0; i < ent->nosee_num; i++)
	{
		if(ent->nosee_contents[i] == num)
			return qtrue;
	}
	return qfalse;
}

void gasThinkStandard(gentity_t *ent);

void gasGrenadeExplode( gentity_t *ent )
{
	if ( !ent->count )
	{
		//G_Sound( ent, CHAN_WEAPON, G_SoundIndex( "sound/weapons/thermal/warning.wav" ) );
		ent->count = 1;
		ent->genericValue5 = level.time + 500;
		ent->think = gasThinkStandard;
		ent->nextthink = level.time;
		ent->r.svFlags |= SVF_BROADCAST;//so everyone hears/sees the explosion?
	}
	else
	{
		vec3_t	origin;
		vec3_t	dir={0,0,1};
		gentity_t *triggerEnt;

		triggerEnt = G_Spawn();

		BG_EvaluateTrajectory( &ent->s.pos, level.time, origin );
		origin[2] += 8;
		SnapVector( origin );
		G_SetOrigin( ent, origin );

		ent->s.eType = ET_GENERAL;
		G_AddEvent( ent, EV_MISSILE_MISS, DirToByte( dir ) );
		ent->freeAfterEvent = qtrue;

		if (G_RadiusDamage( ent->r.currentOrigin, ent->parent,  ent->splashDamage, ent->splashRadius, 
				ent, ent, ent->splashMethodOfDeath))
		{
			g_entities[ent->r.ownerNum].client->accuracy_hits++;
		}

		VectorCopy(ent->r.currentOrigin, triggerEnt->s.origin);
		{
			vec3_t mins = { -(GAS_BLIND_AREA/2), -(GAS_BLIND_AREA/2), -(GAS_BLIND_AREA/2) };
			vec3_t maxs = { (GAS_BLIND_AREA/2), (GAS_BLIND_AREA/2), (GAS_BLIND_AREA/2) };
			VectorCopy(mins, triggerEnt->r.mins);
			VectorCopy(maxs, triggerEnt->r.maxs);
		}
		triggerEnt->genericValue5 = level.time + GAS_TIME;

		triggerEnt->parent = ent->parent;
		SP_trigger_gas_nosee(triggerEnt);

		trap_LinkEntity( ent );
	}
}

void gasThinkStandard(gentity_t *ent)
{
	if (ent->genericValue5 < level.time)
	{
		ent->think = gasGrenadeExplode;
		ent->nextthink = level.time;
		return;
	}

	G_RunObject(ent);
	ent->nextthink = level.time;
}

#define SPECIAL_NADE_SPEED 900
gentity_t *WP_FireGasGrenade( gentity_t *ent )
{
	weaponData_t	*wp				= &weaponData[WP_AUXILIARY8];
	gentity_t	*bolt;
	vec3_t		dir, start;

	AngleVectors( ent->client->ps.viewangles, forward, vright, up );
	CalcMuzzlePoint( ent, forward, vright, up, muzzle );

	VectorCopy( forward, dir );
	VectorCopy( muzzle, start );

	bolt = G_Spawn();
	
	bolt->physicsObject = qtrue;

	bolt->classname = "thermal_detonator";
	bolt->think = gasThinkStandard;
	bolt->nextthink = level.time;
	bolt->touch = touch_NULL;

	// How 'bout we give this thing a size...
	VectorSet( bolt->r.mins, -3.0f, -3.0f, -3.0f );
	VectorSet( bolt->r.maxs, 3.0f, 3.0f, 3.0f );
	bolt->clipmask = MASK_SHOT;

	WP_TraceSetStart( ent, start, bolt->r.mins, bolt->r.maxs );//make sure our start point isn't on the other side of a wall

	bolt->genericValue5 = level.time + GASNADE_TIME; // How long 'til she blows
	bolt->s.pos.trType = TR_GRAVITY;
	bolt->parent = ent;
	bolt->r.ownerNum = ent->s.number;
	VectorScale( dir, wp->bulletSpeed, bolt->s.pos.trDelta );

	if ( ent->health >= 0 )
	{
		bolt->s.pos.trDelta[2] += 120;
	}

	bolt->flags |= FL_BOUNCE_HALF;
	bolt->damage = wp->damage;
	bolt->dflags = 0;
	bolt->splashDamage = wp->damage;
	bolt->splashRadius = LT_SPLASH_RAD;

	bolt->s.eType = ET_MISSILE;
	bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
	bolt->s.weapon = WP_AUXILIARY8;

	bolt->methodOfDeath = wp->methodOfDeath;
	bolt->splashMethodOfDeath = wp->methodOfDeath;

	bolt->s.pos.trTime = level.time;		// move a bit on the very first frame
	VectorCopy( start, bolt->s.pos.trBase );
	
	SnapVector( bolt->s.pos.trDelta );			// save net bandwidth
	VectorCopy (start, bolt->r.currentOrigin);

	VectorCopy( start, bolt->pos2 );

	bolt->bounceCount = -5;

	return bolt;
}