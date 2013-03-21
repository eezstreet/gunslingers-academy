#include "GSAI_NPC.h"

void GSNPC_Begin(gentity_t *ent)
{
	usercmd_t	ucmd;
	gclient_t	*client;
	vec3_t	spawn_origin, spawn_angles;

	VectorCopy( ent->client->ps.origin, spawn_origin);
	VectorCopy( ent->s.angles, spawn_angles);

	spawn_angles[YAW] = ent->NPC->desiredYaw;

	client = ent->client;
	// increment the spawncount so the client will detect the respawn
	client->ps.persistant[PERS_SPAWN_COUNT]++;

	client->airOutTime = level.time + 12000;
	client->ps.clientNum = ent->s.number;
	if ( ent->health )	// Was health supplied in map
	{
		client->pers.maxHealth = client->ps.stats[STAT_MAX_HEALTH] = ent->health;
	}
	else
	{
		client->pers.maxHealth = client->ps.stats[STAT_MAX_HEALTH] = 100;
	}

	ent->r.contents = CONTENTS_BODY;
	ent->clipmask = MASK_NPCSOLID;

	ent->die = player_die;
	ent->waterlevel = 0;
	ent->watertype = 0;
	ent->client->ps.rocketLockIndex = ENTITYNUM_NONE;
	ent->client->ps.rocketLockTime = 0;

	ent->flags &= ~FL_NOTARGET;
	ent->s.eFlags &= ~EF_NODRAW;

	ent->client->ps.gravity = g_gravity.value;

	//TODO: Weapon sets
	ent->NPC->currentAmmo = 100;

	ent->client->ps.weapon = WP_NONE;
	ent->client->ps.weaponstate = WEAPON_IDLE;
	
	client->ps.pm_flags |= PMF_RESPAWNED;
	ent->s.eType = ET_NPC;

	VectorCopy (spawn_origin, ent->s.origin);
	SetClientViewAngle( ent, spawn_angles );
	client->renderInfo.lookTarget = ENTITYNUM_NONE;

	client->respawnTime = level.time;
	client->inactivityTime = level.time + g_inactivity.value * 1000;
	client->latched_buttons = 0;

	ent->s.owner = ENTITYNUM_NONE;

	NPC_SetAnim( ent, SETANIM_BOTH, BOTH_STAND1, SETANIM_FLAG_NORMAL );
	trap_ICARUS_InitEnt( ent );

	memset( &ucmd, 0, sizeof( usercmd_t ) );

	ent->enemy = NULL;

}