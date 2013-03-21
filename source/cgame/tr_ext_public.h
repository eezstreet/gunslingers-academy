#ifndef TR_EXT_PUBLIC_H
#define TR_EXT_PUBLIC_H

#include "tr_ext_glsl_program.h"
#include "tr_ext_framebuffer.h"

void R_EXT_Init ( void );
void R_EXT_Cleanup ( void );
void R_EXT_PostProcess ( void );

typedef struct
{
	float screenX;
	float screenY;
} godRay_t;
int numGodRays;

#endif
