#include "../g_local.h"
#include "../g_weapon.h"

//LASER SIGHT
//This weapon function creates a visible laser beam. While the beam is active, accuracy drain rates decrease sharply.
void WF_LaserSight( gentity_t *ent )
{
	weaponData_t	*wp				= &weaponData[ent->s.weapon];
	if(ent->s.eFlags2 & EF2_LASER_SIGHT)
	{
		ent->s.eFlags2 &= ~EF2_LASER_SIGHT;
	}
	else
	{
		ent->s.eFlags2 |= EF2_LASER_SIGHT;
	}
}