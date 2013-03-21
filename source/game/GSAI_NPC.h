#include "g_local.h"

typedef struct
{
	int		intelligence;
	int		aggression;
	int		loyalty;
	int		marksmanship;
} keyTraits_t;

extern void NPC_SetAnim(gentity_t *ent, int setAnimParts, int anim, int setAnimFlags);