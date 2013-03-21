#include "g_local.h"
#include "g_weapon.h"

extern vec3_t	wp_forward, wp_vright, wp_up;
extern vec3_t	wp_muzzle;
extern qboolean G_HeavyMelee( gentity_t *attacker );
//---------------------------------------------------------
// FireMelee
//---------------------------------------------------------
void WP_FireMelee( gentity_t *ent, qboolean alt_fire )
{
	weaponData_t	*wp				= &weaponData[ent->s.weapon];
	gentity_t	*tr_ent;
	trace_t		tr;
	vec3_t		mins, maxs, end;
	vec3_t		muzzlePunch;

	if (ent->client && ent->client->ps.torsoAnim == BOTH_MELEE2)
	{ //right
		if (ent->client->ps.brokenLimbs & (1 << BROKENLIMB_RARM))
		{
			return;
		}
	}
	else
	{ //left
		if (ent->client->ps.brokenLimbs & (1 << BROKENLIMB_LARM))
		{
			return;
		}
	}

	if (!ent->client)
	{
		VectorCopy(ent->r.currentOrigin, muzzlePunch);
		muzzlePunch[2] += 8;
	}
	else
	{
		VectorCopy(ent->client->ps.origin, muzzlePunch);
		muzzlePunch[2] += ent->client->ps.viewheight-6;
	}

	VectorMA(muzzlePunch, 20.0f, wp_forward, muzzlePunch);
	VectorMA(muzzlePunch, 4.0f, wp_vright, muzzlePunch);

	VectorMA( muzzlePunch, MELEE_RANGE, wp_forward, end );

	VectorSet( maxs, 6, 6, 6 );
	VectorScale( maxs, -1, mins );

	trap_Trace ( &tr, muzzlePunch, mins, maxs, end, ent->s.number, MASK_SHOT );

	if (tr.entityNum != ENTITYNUM_NONE)
	{ //hit something
		tr_ent = &g_entities[tr.entityNum];

		G_Sound( ent, CHAN_AUTO, G_SoundIndex( va("sound/weapons/melee/punch%d", Q_irand(1, 4)) ) );

		if (tr_ent->takedamage && tr_ent->client)
		{ //special duel checks
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

		if ( tr_ent->takedamage )
		{ //damage them, do more damage if we're in the second right hook
			int dmg = wp->damage;

			if (ent->client && ent->client->ps.torsoAnim == BOTH_MELEE2)
			{ //do a tad bit more damage on the second swing
				dmg = (wp->damage + 6);
			}

			if ( G_HeavyMelee( ent ) )
			{ //2x damage for heavy melee class
				dmg *= 2;
			}

			G_Damage( tr_ent, ent, ent, wp_forward, tr.endpos, dmg, DAMAGE_NO_ARMOR, wp->methodOfDeath );
		}
	}
}