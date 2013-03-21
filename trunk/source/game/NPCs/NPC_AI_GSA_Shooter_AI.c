#include "../b_local.h"
#include "../g_nav.h"
#include "../anims.h"
#include "../w_saber.h"


extern void Jedi_Patrol( void );
void NPC_BSGSA_Default( void )
{
	if(!NPC->enemy)
	{ //Look for enemies
		Jedi_Patrol();
	}
	else
	{ //We have an enemy

	}
}