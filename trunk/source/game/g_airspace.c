#include "g_local.h"

qboolean PointWithinBounds2D(vec3_t point, vec3_t absmin, vec3_t absmax)
{ //does not compare Z axis
	if(point[0] >= absmin[0] && point[1] >= absmin[1] &&
		point[0] <= absmax[0] && point[1] <= absmax[1])
		return qtrue;
	return qfalse;
}

gentity_t *AirspaceForPoint(vec3_t point)
{ //Typecast to qboolean to determine PointWithinAirspace
	int i;
	for(i = 0; i < numAirspaces; i++)
	{
		if(PointWithinBounds2D(point, airspace[i]->r.absmin, airspace[i]->r.absmax))
			return airspace[i];
	}
	return NULL;
}

//qboolean PointWithinAirspace(vec3_t point)
//{
//}