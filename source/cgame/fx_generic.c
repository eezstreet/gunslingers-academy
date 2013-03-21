//fx_generic.c: Deals with surface-specific effects ~eezstreet
#include "cg_local.h"
#include "fx_local.h"

//todo: heavy/light weapon distinction, and explosives
void FX_PlayMaterialSpecificEffect( vec3_t origin, vec3_t normal, int material, int weapon )
{
	int bmNumber = BM_NONE;
	switch( material )
	{
		case MATERIAL_SOLIDWOOD:
		case MATERIAL_HOLLOWWOOD:
			bmNumber = BM_WOOD;
			break;
		case MATERIAL_SOLIDMETAL:
		case MATERIAL_HOLLOWMETAL:
			bmNumber = BM_METAL;
			break;
		case MATERIAL_SHORTGRASS:
		case MATERIAL_LONGGRASS:
			bmNumber = BM_GRASS;
			break;
		case MATERIAL_DIRT:
			bmNumber = BM_DIRT;
			break;
		case MATERIAL_SAND:
			bmNumber = BM_SAND;
			break;
		case MATERIAL_GRAVEL:
			bmNumber = BM_GRAVEL;
			break;
		case MATERIAL_GLASS:
		case MATERIAL_BPGLASS:
		case MATERIAL_SHATTERGLASS:
			bmNumber = BM_GLASS;
			break;
		case MATERIAL_CONCRETE:
		case MATERIAL_ARMOR:
			bmNumber = BM_CONCRETE;
			break;
		case MATERIAL_MARBLE:
			bmNumber = BM_MARBLE;
			break;
		case MATERIAL_WATER:
			bmNumber = BM_WATER;
			break;
		case MATERIAL_SNOW:
			bmNumber = BM_SNOW;
			break;
		case MATERIAL_ICE:
			bmNumber = BM_ICE;
			break;
		case MATERIAL_FLESH:
			bmNumber = BM_FLESH;
			break;
		case MATERIAL_MUD:
			bmNumber = BM_MUD;
			break;
		case MATERIAL_DRYLEAVES:
		case MATERIAL_GREENLEAVES:
			bmNumber = BM_LEAVES;
			break;
		case MATERIAL_FABRIC:
		case MATERIAL_CANVAS:
			bmNumber = BM_FABRIC;
			break;
		case MATERIAL_ROCK:
			bmNumber = BM_ROCK;
			break;
		case MATERIAL_RUBBER:
			bmNumber = BM_RUBBER;
			break;
		case MATERIAL_PLASTIC:
		case MATERIAL_TILES:
			bmNumber = BM_PLASTIC;
			break;
		case MATERIAL_CARPET:
			bmNumber = BM_CARPET;
			break;
		case MATERIAL_PLASTER:
			bmNumber = BM_PLASTER;
			break;
		case MATERIAL_COMPUTER:
			bmNumber = BM_COMPUTER;
			break;
	}

	trap_FX_PlayEffectID( cgs.effects.materialImpactEffect[bmNumber], origin, normal, -1, -1 );
}