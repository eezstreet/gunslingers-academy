#include "tr_ext_public.h"
#include "cg_local.h"
#include "GLee.h"

#pragma warning(disable:4204)

float projectionMatrix[16];
float *viewMatrix;

extern vmCvar_t	r_ext_hdrImageKey;
extern vmCvar_t	r_ext_hdrAdjustTime;
extern vmCvar_t	r_ext_postProcessing;
extern vmCvar_t	r_ext_tonemapping;
extern vmCvar_t	r_ext_hdrAlpha;
extern vmCvar_t	r_ext_debugHDR;
extern vmCvar_t	r_ext_desaturation;
extern vmCvar_t r_ext_radialBlur;
extern vmCvar_t r_ext_filmGrain;
extern vmCvar_t r_ext_vignette;
extern vmCvar_t	r_ext_filmGrainThickness;
extern vmCvar_t	r_ext_filmGrainIntensity;
extern vmCvar_t	r_ext_bloom;
extern vmCvar_t	r_ext_brightPassThres;
extern vmCvar_t	r_ext_radialBlurSampleDistance;
extern vmCvar_t	r_ext_radialBlurDistanceMultiplier;
extern vmCvar_t r_ext_bloomFactor;
extern vmCvar_t	r_ext_volumetricLightScattering;
extern vmCvar_t	r_ext_vlsWeight;
extern vmCvar_t	r_ext_vlsNumSamples;
extern vmCvar_t	r_ext_vlsDensity;
extern vmCvar_t	r_ext_vlsDecay;
extern vmCvar_t	r_ext_vlsExposure;
extern vmCvar_t	r_ext_vlsThreshold;

extern qboolean CG_WorldCoordToScreenCoordFloat(vec3_t worldCoord, float *x, float *y);
qboolean r_ext_loaded = qfalse;

#ifdef _WIN32
	#define GL_BIND_ADDRESS				0x489FA0
	#define GL_SELECTTEXTURE_ADDRESS	0x48A010
	static void (*R_SyncRenderThread)	( void )	= ( void (*)( void ) )0x4915B0;
	static void (*RB_SetGL2D)			( void )	= ( void (*)( void ) )0x48B070;
	static void (__cdecl *RB_EndSurface)( void )	= ( void (*)( void ) )0x4AF530;
#elif MAC_PORT
	#define GL_BIND_ADDRESS				0x0448BC
	#define GL_SELECTTEXTURE_ADDRESS	0x044938
	static void (*R_SyncRenderThread)	( void )	= ( void (*)( void ) )0x04CA06;
	static void (*RB_SetGL2D)			( void )	= ( void (*)( void ) )0x045AD0;
	static void (__cdecl *RB_EndSurface)( void )	= ( void (*)( void ) )0x064D26;
#endif

#define TABLESIZE( x, y ) static const int y = sizeof( x ) / sizeof( x[0] );

#define FBODEF( name ) static framebuffer_t *fbo##name
#define CREATEFBO( name, width, height, bits ) \
{ \
	fbo##name = R_EXT_CreateFramebuffer(); \
	R_EXT_BindFramebuffer( fbo##name ); \
	R_EXT_AttachColorTextureToFramebuffer( fbo##name, R_EXT_CreateBlankTexture( width, height, bits ), 0 ); \
	R_EXT_CheckFramebuffer( fbo##name ); \
}

#define SHADERDEF( name ) static glslProgram_t *shader##name
#define LOADSHADER( _program, _shader ) \
{ \
	shader##_program = R_EXT_GLSL_CreateProgram(); \
	R_EXT_GLSL_AttachShader( shader##_program, R_EXT_GLSL_CreateFragmentShader( _shader ) ); \
	R_EXT_GLSL_LinkProgram( shader##_program ); \
	R_EXT_GLSL_UseProgram( shader##_program ); \
}

//================
//	Framebuffers
//================

	FBODEF( Scene );		//	Original scene (Never modified, only use to sample)
	FBODEF( Luminance[9] );
		TABLESIZE( fboLuminance, numLuminanceFbos );
	FBODEF( ToneMapping );	//	Tone mapping
	//FBODEF( Desaturate );

	FBODEF( FilmGrain );
	FBODEF( Vignette );
	FBODEF( RadialBlur );

	FBODEF( BrightPass );
	FBODEF( Downscale[3] );
	FBODEF( Blur[2] );
	FBODEF( Bloom );

	FBODEF( GodRays[2] );

//================
//	Pixel shaders
//================

	SHADERDEF( DownscaleLum );	//	Max luminance
	SHADERDEF( Luminance );		//	Average luminance
	SHADERDEF( ToneMapping );	//	Tone mapping

//================
//	Desaturation
//================

	//SHADERDEF( Desaturate );

//================
//	Others
//================

	SHADERDEF( FilmGrain );
	SHADERDEF( Vignette );
	SHADERDEF( RadialBlur );

	SHADERDEF( BrightPass );
	SHADERDEF( Blur[2] );
	SHADERDEF( Bloom );

	SHADERDEF( GodRays[2] );


typedef struct image_s
{
	char 	name[64];
	short 	width;
	short 	height;
	int 	texnum;
	int		frameUsed;
	int		imageFormat;
	int		glWrapClampMode;
	char 	mipmap;
	char	allowPicmip;
	short 	levused;
} image_t;

static void GL_Bind( unsigned int texID )
{//ASM bridge to call GL_Bind
	static image_t image;
	image.texnum = texID;
	image.frameUsed = 0;

	__asm
	{
		mov esi, offset image
		mov eax, GL_BIND_ADDRESS
		call eax
	}
}

static int GL_SelectTexture( int texUnit )
{//ASM bridge to call GL_SelectTexture
	__asm
	{
		mov esi, texUnit
		mov eax, GL_SELECTTEXTURE_ADDRESS
		call eax
	}
}

static __inline void LoadShaders( void )
{
	//	Luminance
	LOADSHADER( Luminance, "shaders/glsl/luminance.frag" );
	R_EXT_GLSL_SetUniform1i( shaderLuminance, "tex", 0 );
	R_EXT_GLSL_SetUniform1f( shaderLuminance, "pixelWidth", 1.0f / 256.0f );
	R_EXT_GLSL_SetUniform1f( shaderLuminance, "pixelHeight", 1.0f / 256.0f );

	//	Downscale luminance
	LOADSHADER( DownscaleLum, "shaders/glsl/downscale_luminance.frag" );
	R_EXT_GLSL_SetUniform1i( shaderDownscaleLum, "tex", 0 );

	//	Tone mapping
	LOADSHADER( ToneMapping, "shaders/glsl/tonemapping.frag" );
	R_EXT_GLSL_SetUniform1i( shaderToneMapping, "u_ScreenMap", 0 );

	//	Desaturation
	//LOADSHADER( Desaturate, "shaders/glsl/desaturate.frag" );
	//R_EXT_GLSL_SetUniform1f( shaderDesaturate, "strength", 1.0f);
	//R_EXT_GLSL_SetUniform1i( shaderDesaturate, "tex0", 0 );

	LOADSHADER( FilmGrain, "shaders/glsl/filmgrain.frag" );
	R_EXT_GLSL_SetUniform1i( shaderFilmGrain, "sceneTex", 0 );
	R_EXT_GLSL_SetUniform1i( shaderFilmGrain, "depthTex", 1 );

	LOADSHADER( Vignette, "shaders/glsl/vignette.frag" );
	R_EXT_GLSL_SetUniform1i( shaderVignette, "u_ScreenMap", 0 );

	LOADSHADER( RadialBlur, "shaders/glsl/radialblur.frag" );
	R_EXT_GLSL_SetUniform1i( shaderRadialBlur, "SceneTex", 0 );
	R_EXT_GLSL_SetUniform1f( shaderRadialBlur, "Desaturation", 0.4f );

	//	Additive bloom
	LOADSHADER( Bloom, "shaders/glsl/additive_bloom.frag" );

	//	Bright pass
	LOADSHADER( BrightPass, "shaders/glsl/bright_pass.frag" );
	R_EXT_GLSL_SetUniform1i( shaderBrightPass, "SceneTex", 0 );

	//	Gaussian blur X
	LOADSHADER( Blur[0], "shaders/glsl/horizontal_blur.frag" );
	R_EXT_GLSL_SetUniform1i( shaderBlur[0], "SceneTex", 0 );

	//	Gaussian blur Y
	LOADSHADER( Blur[1], "shaders/glsl/vertical_blur.frag" );
	R_EXT_GLSL_SetUniform1i( shaderBlur[1], "SceneTex", 0 );

	//	God Rays
	LOADSHADER( GodRays[0], "shaders/glsl/vol_light_scattering.frag" );
	R_EXT_GLSL_SetUniform1i( shaderGodRays[0], "tex", 0 );

	LOADSHADER( GodRays[1], "shaders/glsl/vol_light_scattering.frag" );
	R_EXT_GLSL_SetUniform1i( shaderGodRays[1], "tex", 0 );

	//LOADSHADER( GodRays[2], "shaders/glsl/vol_light_scattering.frag" );
	//R_EXT_GLSL_SetUniform1i( shaderGodRays[2], "tex", 0 );

	//LOADSHADER( GodRays[3], "shaders/glsl/vol_light_scattering.frag" );
	//R_EXT_GLSL_SetUniform1i( shaderGodRays[3], "tex", 0 );

	return;
}

#define SIZE1 2
#define SIZE2 4
#define SIZE3 8
#define SIZE4 8


void R_EXT_Init( void )
{
	int				sizes[]		= { cgs.glconfig.vidWidth, cgs.glconfig.vidHeight };
	//int				blurSizes[]	= { sizes[0], sizes[1] };
	int				lumSize		= 256;
	int				i;
	int				w = sizes[0];
	int				h = sizes[1];

	r_ext_loaded = qfalse;

	CG_Printf( "----------------------------\n" );
	CG_Printf( "Loading visual extensions...\n" );

	if ( !R_EXT_GLSL_Init() || !R_EXT_FramebufferInit() )
		return;

	CG_Printf( "----------------------------\n" );

	CREATEFBO( Scene,				sizes[0],		sizes[1],		IF_RGBA8 );
	for ( i=0; i<numLuminanceFbos; i++, lumSize /= 2 )
		CREATEFBO( Luminance[i],	lumSize,		lumSize,		IF_RGBA8 );
	CREATEFBO( ToneMapping,			sizes[0],		sizes[1],		IF_RGBA8 );
	//CREATEFBO( Desaturate,			sizes[0],		sizes[1],		IF_RGBA8 );
	CREATEFBO( FilmGrain,			sizes[0],		sizes[1],		IF_RGBA8 );
	CREATEFBO( RadialBlur,			sizes[0],		sizes[1],		IF_RGBA8 );
	CREATEFBO( Vignette,			sizes[0],		sizes[1],		IF_RGBA8 );

	CREATEFBO( BrightPass, w, h, IF_RGBA8 );
	CREATEFBO( Downscale[0], w/2, h/2, IF_RGBA8 );
	CREATEFBO( Downscale[1], w/4, h/4, IF_RGBA8 );
	CREATEFBO( Downscale[2], w/8, h/8, IF_RGBA8 );
	CREATEFBO( Blur[0], w/8, h/8, IF_RGBA8 );
	CREATEFBO( Blur[1], w/8, h/8, IF_RGBA8 );
	CREATEFBO( Bloom, w, h, IF_RGBA8 );

	CREATEFBO( GodRays[0], w, h, IF_RGBA8 );
	CREATEFBO( GodRays[1], w, h, IF_RGBA8 );
	//CREATEFBO( GodRays[2], w, h, IF_RGBA8 );
	//CREATEFBO( GodRays[3], w, h, IF_RGBA8 );

	R_EXT_BindDefaultFramebuffer();

	LoadShaders();

	R_EXT_GLSL_UseProgram( NULL );
	R_EXT_BindDefaultFramebuffer();

	r_ext_loaded = qtrue;
}

void R_EXT_Cleanup( void )
{
	R_EXT_FramebufferCleanup();
	R_EXT_GLSL_Cleanup();
}

static __inline void DrawQuad( float x, float y, float width, float height )
{
	glBegin( GL_QUADS );
		glTexCoord2f (0.0f, 0.0f);
		glVertex2f (x, y + height);

		glTexCoord2f (0.0f, 1.0f);
		glVertex2f (x, y);

		glTexCoord2f (1.0f, 1.0f);
		glVertex2f (x + width, y);

		glTexCoord2f (1.0f, 0.0f);
		glVertex2f (x + width, y + height);
	glEnd();
}

#define DrawFullscreenQuad() DrawQuad( 0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT )
//#define GL_Bind( x ) glBindTexture( GL_TEXTURE_2D, x );
//#define GL_SelectTexture( x ) glActiveTextureARB( GL_TEXTURE##x##_ARB ); glClientActiveTextureARB( GL_TEXTURE##x##_ARB );

static float adaptedFrameLuminance	= 0.0f;
static float adaptedMaxLuminance	= 1.0f;
static float lastFrameTime			= 0.0f;
static void CalculateLightAdaptation ( void )
{
	float	time = trap_Milliseconds() / 1000.0f;
	float	dt;
	vec4_t	currentFrameLuminance;

	R_EXT_BindFramebuffer( fboLuminance[numLuminanceFbos - 1] );
	glReadPixels( 0, 0, 1, 1, GL_RGBA, GL_FLOAT, currentFrameLuminance );

	dt = max( time-lastFrameTime, 0.0f );

	if(adaptedFrameLuminance < 0.08){
		adaptedFrameLuminance	+= (currentFrameLuminance[0] - adaptedFrameLuminance)	* (1.0f - expf(-dt * r_ext_hdrAdjustTime.value));
	} if(adaptedFrameLuminance > 0.5){
		adaptedFrameLuminance	-= (currentFrameLuminance[0] - adaptedFrameLuminance)	* (1.0f - expf(-dt * r_ext_hdrAdjustTime.value));
	}
	//if(adaptedMaxLuminance < 1.0){
		adaptedMaxLuminance		+= (currentFrameLuminance[1] - adaptedMaxLuminance)		* (1.0f - expf(-dt * r_ext_hdrAdjustTime.value));
	//}

	//CAP(adaptedFrameLuminance, 0.1);
	lastFrameTime = time;
}

void R_EXT_PostProcess ( void )
{
	int i;
	int lastFramebufferTexture/* = fboScene->colorTextures[0]->id*/; //FIXED: Older intel-based renderers crashed here.
	int w = cgs.glconfig.vidWidth;
	int h = cgs.glconfig.vidHeight;

	if ( !r_ext_postProcessing.integer || !r_ext_loaded )
		return;

	lastFramebufferTexture = fboScene->colorTextures[0]->id;

	glColor4fv( colorTable[CT_WHITE] ); //HACK: Sometimes scopes change the colour.
    
	// Render scene
	// The renderer batch renders surfaces. This means that it won't surfaces until the loaded surface is different from the previously
	// loaded surface. By drawing an invisible quad, we ensure that the render pipeline is flushed (i.e. all surfaces have been drawn).
	trap_R_DrawStretchPic( 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, cgs.media.whiteShader );
	R_SyncRenderThread();

	// Anything that can be rendered to, be it the screen, or a texture, is known as a framebuffer. The 'default' framebuffer is the
	// backbuffer (which will be displayed on the screen when the driver tell it to). We can copy the data from the default framebuffer
	// to any framebuffer we choose. In this case, I'm copying it to 'sceneFbo'.
	R_EXT_BlitFramebufferColor( NULL, fboScene, w, h, w, h );

	//================================
	//	High Dynamic Range
	//================================

	if ( r_ext_tonemapping.integer )
	{
		int size = fboLuminance[0]->colorTextures[0]->width;
		glPushAttrib( GL_VIEWPORT_BIT|GL_SCISSOR_BIT );
		glViewport( 0, 0, size, size );
		glScissor( 0, 0, size, size );

		// Downscale and convert to luminance.    
		R_EXT_GLSL_UseProgram( shaderLuminance );
		R_EXT_BindFramebuffer( fboLuminance[0] );
		GL_Bind( lastFramebufferTexture );
		DrawFullscreenQuad();

		// Downscale further to 1x1 texture.
		R_EXT_GLSL_UseProgram( shaderDownscaleLum );
		for ( i=1; i<numLuminanceFbos; i++ )
		{
			size = fboLuminance[i]->colorTextures[0]->width;

			R_EXT_GLSL_SetUniform1f( shaderDownscaleLum, "pixelWidth", 1.0f / (float)size );
			R_EXT_GLSL_SetUniform1f( shaderDownscaleLum, "pixelHeight", 1.0f / (float)size );

			R_EXT_BindFramebuffer( fboLuminance[i] );
			GL_Bind( fboLuminance[i-1]->colorTextures[0]->id );
			glViewport( 0, 0, size, size );
			glScissor( 0, 0, size, size );

			DrawFullscreenQuad();
		}

		glPopAttrib();

		// Tonemapping.
		CalculateLightAdaptation();

		R_EXT_BindFramebuffer( fboToneMapping );
		R_EXT_GLSL_UseProgram( shaderToneMapping );
		R_EXT_GLSL_SetUniform1f( shaderToneMapping, "imageKey", r_ext_hdrImageKey.value ? r_ext_hdrImageKey.value : ( 1.03f - ( 2.0f / ( 2.0f + log10f( adaptedFrameLuminance + 1.0 ) ) ) ) );
		R_EXT_GLSL_SetUniform1f( shaderToneMapping, "luminance", adaptedFrameLuminance );
		R_EXT_GLSL_SetUniform1f( shaderToneMapping, "maxWhite", adaptedMaxLuminance*adaptedMaxLuminance );
		R_EXT_GLSL_SetUniform1f( shaderToneMapping, "alpha", r_ext_hdrAlpha.value );
		GL_Bind( lastFramebufferTexture );
		DrawFullscreenQuad();

		lastFramebufferTexture = fboToneMapping->colorTextures[0]->id;
	}
	else
	{
		R_EXT_BindFramebuffer( fboToneMapping );
		R_EXT_GLSL_UseProgram( NULL );
		GL_Bind( lastFramebufferTexture );
		DrawFullscreenQuad();

		lastFramebufferTexture = fboToneMapping->colorTextures[0]->id;
	}

	//====
	//God Rays
	//====
#define ResizeTarget(w, h) glViewport( 0, 0, w, h ); glScissor( 0, 0, w, h );
	if(r_ext_volumetricLightScattering.integer)
	{
		//OK...First, evaluate what's available
		godRay_t godRays[2];
		int i;
		int godRayEnts[2] = { 0, 0 };
		numGodRays = 0;
		{
			for(i = 0; i < 1024; i++)
			{
				if(numGodRays > 1)
					break;
				if(cg.lightScattering[i])
				{
					godRayEnts[numGodRays++] = i;
				}
			}
		}
		//Good. Got our ents. Now get the screen coords.
		if(numGodRays > 0)
		{
			for(i = 0; i < numGodRays; i++)
			{
				float weight, xDiff, yDiff;
				float density;
				CG_WorldCoordToScreenCoordFloat(cg_entities[godRayEnts[i]].currentState.origin, &godRays[i].screenX, &godRays[i].screenY);
				
				//Render the sucker.
				R_EXT_BindFramebuffer(fboGodRays[i]);
				R_EXT_GLSL_UseProgram(shaderGodRays[i]);

				//Check for out of bounds
				if(abs(godRays[i].screenX) > SCREEN_WIDTH || abs(godRays[i].screenY) > SCREEN_HEIGHT)
				{
					weight = 0;
					density = 0;
					goto startMe;
				}

				//Find the weight addition
				xDiff = abs(godRays[i].screenX - (SCREEN_WIDTH/2));
				yDiff = abs(godRays[i].screenY - (SCREEN_HEIGHT/2));
				weight = (float)sqrt(((xDiff*xDiff)+(yDiff*yDiff)));
				weight /= (float)sqrt((SCREEN_WIDTH*SCREEN_WIDTH)+(SCREEN_HEIGHT*SCREEN_HEIGHT));
				weight = 0.65 - weight;
				BUMP(weight, 0);

				density = weight * 0.2;
startMe:
				//Set the uniforms
				R_EXT_GLSL_SetUniform2f(shaderGodRays[i], "lightOrigin", -1.0f+(godRays[i].screenX/(SCREEN_WIDTH/2)), 1.0f-(godRays[i].screenY/(SCREEN_HEIGHT/2)));
				R_EXT_GLSL_SetUniform1f(shaderGodRays[i], "Weight", r_ext_vlsWeight.value + weight);
				R_EXT_GLSL_SetUniform1i(shaderGodRays[i], "NumSamples", r_ext_vlsNumSamples.integer);
				R_EXT_GLSL_SetUniform1f(shaderGodRays[i], "Density", r_ext_vlsDensity.value + density);
				R_EXT_GLSL_SetUniform1f(shaderGodRays[i], "Decay", r_ext_vlsDecay.value);
				R_EXT_GLSL_SetUniform1f(shaderGodRays[i], "Exposure", r_ext_vlsExposure.value);
				R_EXT_GLSL_SetUniform1f(shaderGodRays[i], "Threshold", r_ext_vlsThreshold.value);

				//Bind the texture
				GL_Bind( lastFramebufferTexture );
				ResizeTarget( w, h );
				DrawFullscreenQuad();

				lastFramebufferTexture = fboGodRays[i]->colorTextures[0]->id;
			}
		}
	}

	//====
	//Desat
	//====
	/*if(r_ext_desaturation.integer){
		R_EXT_BindFramebuffer( fboDesaturate );
		R_EXT_GLSL_UseProgram( shaderDesaturate );
		GL_Bind( lastFramebufferTexture );
		DrawFullscreenQuad();

		lastFramebufferTexture = fboDesaturate->colorTextures[0]->id;
	}*/
	//====
	//Film Grain
	//====
	if(r_ext_filmGrain.integer){
		if(!projectionMatrix[0])
		{
			glGetFloatv(GL_PROJECTION_MATRIX, projectionMatrix);
		}
		if(!viewMatrix)
		{
			viewMatrix = (float *)(0xFE381C + 0x3C);
		}
		//0xFE381C + 0x3C = view matrix

		R_EXT_BindFramebuffer( fboFilmGrain );
		R_EXT_GLSL_UseProgram( shaderFilmGrain );

		//R_EXT_GLSL_SetUniform1f( shaderFilmGrain, "time", cg.time );
		//R_EXT_GLSL_SetUniform1f( shaderFilmGrain, "grainMax", r_ext_filmGrainThickness.value ); //TODO: create a cvar for this
		//R_EXT_GLSL_SetUniform1f( shaderFilmGrain, "intensity", r_ext_filmGrainIntensity.value ); //TODO: create a cvar for this

		R_EXT_GLSL_SetUniform1i( shaderFilmGrain, "numberSamples", 16 );
		R_EXT_GLSL_SetUniformMatrix( shaderFilmGrain, "previousPM", projectionMatrix, 16 );
		R_EXT_GLSL_SetUniformMatrix( shaderFilmGrain, "previousVM", viewMatrix, 16 );
		R_EXT_GLSL_SetUniformMatrix( shaderFilmGrain, "thisVM", (float *)(0xFE381C + 0x3C), 16 );

		GL_Bind( lastFramebufferTexture );
		DrawFullscreenQuad();

		glGetFloatv(GL_PROJECTION_MATRIX, projectionMatrix);
		viewMatrix = (float *)(0xFE381C + 0x3C);
		lastFramebufferTexture = fboFilmGrain->colorTextures[0]->id;
	}

	//====
	//Radial Blur
	//====
	if(r_ext_radialBlur.integer){
		float sumSpeed = abs(cg.predictedPlayerState.velocity[0]) + abs(cg.predictedPlayerState.velocity[1]) + abs(cg.predictedPlayerState.velocity[2]);
		R_EXT_BindFramebuffer( fboRadialBlur );
		R_EXT_GLSL_UseProgram( shaderRadialBlur );

		//eezstreet add
		if(!(cg.predictedPlayerState.movementDir == 0 || cg.predictedPlayerState.movementDir == 4) || sumSpeed < 10)
		{
			/*R_EXT_GLSL_SetUniform1f( shaderRadialBlur, "SampleDistance", r_ext_radialBlurSampleDistance.value );
			R_EXT_GLSL_SetUniform1f( shaderRadialBlur, "DistanceMultiplier", r_ext_radialBlurDistanceMultiplier.value );*/
		}
		else
		{
			R_EXT_GLSL_SetUniform1f( shaderRadialBlur, "SampleDistance", r_ext_radialBlurSampleDistance.value );
			R_EXT_GLSL_SetUniform1f( shaderRadialBlur, "DistanceMultiplier", sumSpeed / 35.0f );
		}
		//end eezstreet add
		GL_Bind( lastFramebufferTexture );
		DrawFullscreenQuad();

		lastFramebufferTexture = fboRadialBlur->colorTextures[0]->id;
	}

	//====
	//Bloom
	//====
	if ( r_ext_bloom.integer )
	{
		//Bright pass
		R_EXT_BindFramebuffer( fboBrightPass );
		R_EXT_GLSL_UseProgram( shaderBrightPass );
		R_EXT_GLSL_SetUniform1f( shaderBrightPass, "threshold", r_ext_brightPassThres.value );
		GL_Bind( fboToneMapping->colorTextures[0]->id );
		DrawFullscreenQuad();

		R_EXT_GLSL_UseProgram( NULL );
		glPushAttrib( GL_VIEWPORT_BIT|GL_SCISSOR_BIT );

		//Downscale /2
		R_EXT_BindFramebuffer( fboDownscale[0] );
		GL_Bind( fboBrightPass->colorTextures[0]->id );
		ResizeTarget( w/SIZE1, h/SIZE1 );
		DrawFullscreenQuad();

		//Downscale /4
		R_EXT_BindFramebuffer( fboDownscale[1] );
		GL_Bind( fboDownscale[0]->colorTextures[0]->id );
		ResizeTarget( w/SIZE2, h/SIZE2 );
		DrawFullscreenQuad();

		//Downscale /8
		R_EXT_BindFramebuffer( fboDownscale[2] );
		GL_Bind( fboDownscale[1]->colorTextures[0]->id );
		ResizeTarget( w/SIZE3, h/SIZE3 );
		DrawFullscreenQuad();

		//Blur X
		R_EXT_BindFramebuffer( fboBlur[0] );
		R_EXT_GLSL_UseProgram( shaderBlur[0] );		R_EXT_GLSL_SetUniform1f( shaderBlur[0], "PixelWidth", 1.0f/(w/SIZE4) );
		GL_Bind( fboDownscale[2]->colorTextures[0]->id );
		ResizeTarget( w/SIZE4, h/SIZE4 );
		DrawFullscreenQuad();

		//Blur Y
		R_EXT_BindFramebuffer( fboBlur[1] );
		R_EXT_GLSL_UseProgram( shaderBlur[1] );		R_EXT_GLSL_SetUniform1f( shaderBlur[1], "PixelHeight", 1.0f/(h/SIZE4) );
		GL_Bind( fboBlur[0]->colorTextures[0]->id );
		ResizeTarget( w/SIZE4, h/SIZE4 );
		DrawFullscreenQuad();

		//Upscale /8
		R_EXT_BindFramebuffer( fboDownscale[2] );
		R_EXT_GLSL_UseProgram( NULL );
		GL_Bind( fboBlur[1]->colorTextures[0]->id );
		ResizeTarget( w/SIZE3, h/SIZE3 );
		DrawFullscreenQuad();

		//Upscale /4
		R_EXT_BindFramebuffer( fboDownscale[1] );
		R_EXT_GLSL_UseProgram( NULL );
		GL_Bind( fboDownscale[2]->colorTextures[0]->id );
		ResizeTarget( w/SIZE2, h/SIZE2 );
		DrawFullscreenQuad();

		//Upscale /2
		R_EXT_BindFramebuffer( fboDownscale[0] );
		GL_Bind( fboDownscale[1]->colorTextures[0]->id );
		ResizeTarget( w/SIZE1, h/SIZE1 );
		DrawFullscreenQuad();

		//Upscale /1
		R_EXT_BindFramebuffer( fboBrightPass );
		GL_Bind( fboDownscale[0]->colorTextures[0]->id );
		ResizeTarget( w, h );
		DrawFullscreenQuad();

		glPopAttrib();

		//Blend
		R_EXT_BindFramebuffer( fboBloom );
		R_EXT_GLSL_UseProgram( shaderBloom );
		GL_SelectTexture( 0 );	
		GL_Bind( fboToneMapping->colorTextures[0]->id );
		R_EXT_GLSL_SetUniform1i( shaderBloom, "SceneTex", 0 );
		GL_SelectTexture( 1 );	
		GL_Bind( fboBrightPass->colorTextures[0]->id );	
		R_EXT_GLSL_SetUniform1i( shaderBloom, "BloomTex", 1 );
		R_EXT_GLSL_SetUniform1f( shaderBloom, "bloomFactor", r_ext_bloomFactor.value );
		DrawFullscreenQuad();
		lastFramebufferTexture = fboBrightPass->colorTextures[0]->id;
		GL_SelectTexture( 0 );
	}
	//====
	//Vignette
	//====
	if(r_ext_vignette.integer){
		R_EXT_BindFramebuffer( fboVignette );
		R_EXT_GLSL_UseProgram( shaderVignette );
		GL_Bind( lastFramebufferTexture );
		DrawFullscreenQuad();

		lastFramebufferTexture = fboVignette->colorTextures[0]->id;
	}


	//After all post-processing effets are done, we render to the back-buffer
	R_EXT_GLSL_UseProgram( NULL );
	R_EXT_BindDefaultFramebuffer();
	GL_Bind( lastFramebufferTexture );
	if(r_ext_tonemapping.integer)
		GL_Bind( fboToneMapping->colorTextures[0]->id );
	if(r_ext_volumetricLightScattering.integer)
	{
		int i;
		for(i = 0; i < numGodRays; i++)
		{
			GL_Bind( fboGodRays[i]->colorTextures[0]->id );
		}
	}
	//if(r_ext_desaturation.integer)
	//	GL_Bind( fboDesaturate->colorTextures[0]->id );
	if(r_ext_filmGrain.integer)
		GL_Bind( fboFilmGrain->colorTextures[0]->id );
	if(r_ext_vignette.integer)
		GL_Bind( fboVignette->colorTextures[0]->id );
	if(r_ext_radialBlur.integer)
		GL_Bind( fboRadialBlur->colorTextures[0]->id );
	if(r_ext_bloom.integer)
		GL_Bind( fboBloom->colorTextures[0]->id );
	DrawFullscreenQuad();


#define DEBUGVIEW( FBO ) { GL_Bind( (fbo##FBO)->colorTextures[0]->id ); DrawQuad( weights[dViews][0], weights[dViews][1], w, h ); dViews++; }
	if ( r_ext_debugHDR.integer )
	{
		static const float weights[18][2] = {
			{ 108.0f*0, 80.0f*0 },	{ 108.0f*0, 80.0f*1 },	{ 108.0f*0,	80.0f*2 },	{ 108.0f*0,	80.0f*3 },	{ 108.0f*0,	80.0f*4 },	{ 108.0f*0,	80.0f*5 },
			{ 108.0f*1, 80.0f*0 },	{ 108.0f*1,	80.0f*1 },	{ 108.0f*1,	80.0f*2 },	{ 108.0f*1,	80.0f*3 },	{ 108.0f*1,	80.0f*4 },	{ 108.0f*1,	80.0f*5 },
			{ 108.0f*2, 80.0f*0 },	{ 108.0f*2,	80.0f*1 },	{ 108.0f*2,	80.0f*2 },	{ 108.0f*2,	80.0f*3 },	{ 108.0f*2,	80.0f*4 },	{ 108.0f*2,	80.0f*5 },
		};
		int		dViews = 0;
		float	w = 108.0f;
		float	h = 80;

		//6*3 MAX
		DEBUGVIEW( Scene );
		//DEBUGVIEW( Desaturate );
		DEBUGVIEW( Luminance[4] );
		DEBUGVIEW( Luminance[6] );
		DEBUGVIEW( Luminance[8] );
		DEBUGVIEW( ToneMapping );
	}
#undef DEBUGVIEW

	return;
}