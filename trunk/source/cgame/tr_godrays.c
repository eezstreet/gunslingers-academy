#include "tr_ext_public.h"
#include "cg_local.h"
#include "GLee.h"

extern qboolean CG_WorldCoordToScreenCoordFloat(vec3_t worldCoord, float *x, float *y);
void EvaluateGodRays(godRay_t **grs)
{
	int i, j;
	numGodRays = 0;
	for(i = 0; i < 1024; i++)
	{
		if(cg.lightScattering[i])
		{
			godRay_t *gr = &(*grs[numGodRays]);
			CG_WorldCoordToScreenCoordFloat(cg_entities[i].currentState.origin, &gr->screenX, &gr->screenY);
			numGodRays++;
		}
	}
}