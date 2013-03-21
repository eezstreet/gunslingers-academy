#include "cg_local.h"
static void CG_BDistortionOrb( centity_t *cent )
{
	refEntity_t ent;
	vec3_t ang;
	float scale = 0.5f;
	float vLen;

	if (!cg_renderToTextureFX.integer)
	{
		return;
	}
	memset( &ent, 0, sizeof( ent ) );

	VectorCopy( cent->lerpOrigin, ent.origin );

	VectorSubtract(ent.origin, cg.refdef.vieworg, ent.axis[0]);
	vLen = VectorLength(ent.axis[0]);
	if (VectorNormalize(ent.axis[0]) <= 0.1f)
	{	// Entity is right on vieworg.  quit.
		return;
	}

//	VectorCopy(cg.refdef.viewaxis[2], ent.axis[2]);
//	CrossProduct(ent.axis[0], ent.axis[2], ent.axis[1]);
	vectoangles(ent.axis[0], ang);
	ang[ROLL] = cent->trickAlpha;
	cent->trickAlpha += 16; //spin the half-sphere to give a "screwdriver" effect
	AnglesToAxis(ang, ent.axis);

	//radius must be a power of 2, and is the actual captured texture size
	if (vLen < 128)
	{
		ent.radius = 256;
	}
	else if (vLen < 256)
	{
		ent.radius = 128;
	}
	else if (vLen < 512)
	{
		ent.radius = 64;
	}
	else
	{
		ent.radius = 32;
	}

	VectorScale(ent.axis[0], scale, ent.axis[0]);
	VectorScale(ent.axis[1], scale, ent.axis[1]);
	VectorScale(ent.axis[2], -scale, ent.axis[2]);

	ent.hModel = cgs.media.halfShieldModel;
	ent.customShader = 0;//cgs.media.halfShieldShader;	

#if 1
	ent.renderfx = (RF_DISTORTION|RF_RGB_TINT);

	//tint the whole thing a shade of blue
	ent.shaderRGBA[0] = 200.0f;
	ent.shaderRGBA[1] = 200.0f;
	ent.shaderRGBA[2] = 255.0f;
#else //no tint
	ent.renderfx = RF_DISTORTION;
#endif

	trap_R_AddRefEntityToScene( &ent );
}


void FX_BatonAltProjectileThink( centity_t *cent, const struct weaponInfo_s *weapon )
{
	vec3_t forward;

	if ( VectorNormalize2( cent->currentState.pos.trDelta, forward ) == 0.0f )
	{
		forward[2] = 1.0f;
	}

	if(cent->currentState.saberInFlight)
	{
		trap_FX_PlayEffectID( cgs.effects.batonCartridgeEffect, cent->lerpOrigin, forward, -1, -1 );
	}
	else
	{
		if(Q_irand(1,5) == 1)
		{
			trap_FX_PlayEffectID( cgs.effects.batonAltProjectileArcEffect, cent->lerpOrigin, forward, -1, -1 );
		}
		else
		{
			trap_FX_PlayEffectID( cgs.effects.batonAltProjectileEffect, cent->lerpOrigin, forward, -1, -1 );
		}
	}
}

/*
------------------------
FX_RepeaterAltHitWall
------------------------
*/

void FX_BatonAltHitWall( vec3_t origin, vec3_t normal )
{
	trap_FX_PlayEffectID( cgs.effects.batonAltWallImpactEffect, origin, normal, -1, -1 );
}

/*
------------------------
FX_RepeaterAltHitPlayer
------------------------
*/

void FX_BatonAltHitPlayer( vec3_t origin, vec3_t normal, qboolean humanoid )
{
	trap_FX_PlayEffectID( cgs.effects.batonAltPlayerImpactEffect, origin, normal, -1, -1 );
}