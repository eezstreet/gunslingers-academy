#include "g_local.h"

void EvaluateMiniRewards(gentity_t *self, gentity_t *attacker, int mod)
{
	char buf[512];
	if(g_barebones.integer)
		return;

	buf[0] = '\0';

	if((level.time - attacker->last_kill_time) > 1000)
	{
		attacker->cumulativeKills = 0;
	}
	else if((level.time - attacker->last_kill_time) < 2500 && attacker->client)
	{
		attacker->cumulativeKills++;
		if(attacker->cumulativeDebounce <= level.time && attacker->cumulativeKills > 0)
		{
			switch(attacker->cumulativeKills)
			{
				case 1:
					//trap_SendServerCommand(attacker->s.number, "scp \"^2Double Kill!\"");
					strcat(buf, "^2Double Kill! ");
					break;
				case 2:
					strcat(buf, "^2Triple Kill! ");
					//trap_SendServerCommand(attacker->s.number, "scp \"^2Triple Kill!\"");
					break;
				default:
					strcat(buf, "^2Multi Kill! ");
					//trap_SendServerCommand(attacker->s.number, "scp \"^2Multi Kill!\"");
					trap_SendServerCommand(-1, va("multikill %i", attacker->s.number));
					break;
			}
		}
	}

	if(attacker->lastKilledBy == self->s.number && attacker->s.number != self->s.number && self->client)
	{
		strcat(buf, "^3Revenge! ");
	}
	
	if(self->client && attacker->client)
	{
		IFGAME(GT_HERO)
		{
			if(self->s.number == level.blueHero || self->s.number == level.redHero && self->client->sess.sessionTeam != attacker->client->sess.sessionTeam)
			{
				strcat(buf, "^2Killed Hero! ");
			}
		}
		if(modData[mod].headShot == 1)
		{
			strcat(buf, "^7Head Shot! ");
		}
		else if(mod == MOD_MELEE)
		{
			strcat(buf, "^6Fists of Fury! ");
		}
		else if(mod == MOD_STUN_BATON)
		{
			strcat(buf, "^5Stun-Gunned! ");
		}

		if(self->client->ps.powerups[PW_BLUEFLAG] ||
			self->client->ps.powerups[PW_REDFLAG])
		{
			strcat(buf, "^3Killed Flag Carrier! ");
		}
		if(g_gametype.integer >= GT_TEAM && attacker->client)
		{
			if(self->client->sess.sessionTeam == attacker->client->sess.sessionTeam)
			{
				strcat(buf, "^1Team Kill! ");
			}
		}

		if(self->client->ps.isJediMaster)
		{
			strcat(buf, "^3Killed Ninja! ");
		}
	}

	trap_SendServerCommand(attacker->s.number, va("scp \"%s\"", buf));
}