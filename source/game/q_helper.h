#pragma once

//Credit to Raz0r for finding this

typedef unsigned char	byte;

//RGBA helpers
#define _RGB( v, r, g, b)		{v[0] = r; v[1] = g; v[2] = b; }
#define _RGBA( v, r, g, b, a)	{v[0] = r; v[1] = g; v[2] = b; v[3] = a;}

//enum versions of RGB
typedef enum RGBA_e		{ RED = 0,	GREEN,	BLUE,	ALPHA	} RGBA_t;
typedef enum _RGBA_e	{ R = 0,	G,		B,		A		} _RGBA_t;

//BUMP/CLAMP/CAP helpers. BUMP enforces a bottom limit, CAP forces an upper limit and CLAMP does both.
#define BUMP(x,y)	if(x < y) x = y
#define  CAP(x,y)	if(x > y) x = y
#define CLAMP( var, min, max )			BUMP( var,		min );	CAP( var,		max)
#define CLAMPVEC( vec, idx, min, max )	BUMP( vec[idx], min );	CAP( vec[idx],	max)

//RNG/ERNG modifiers
#define RNG(x,y,z)		(x < z && x > y)
#define ERNG(x,y,z)		(x <= z && x >= y)

//IFGAME/IFNGAME/IFGTGAME/IFGTEGAME/IFLTGAME/IFLTEGAME
#define IFGAME(x)		if(g_gametype.integer == x)
#define IFNGAME(x)		if(g_gametype.integer != x)
#define IFGTGAME(x)		if(g_gametype.integer > x)
#define IFGTEGAME(x)	if(g_gametype.integer >= x)
#define IFLTGAME(x)		if(g_gametype.integer < x)
#define IFLTEGAME(x)	if(g_gametype.integer <= x)

//For ease-of-use when declaring new variables (NULL ptrs, 0 structs, etc)
#define NEWVEC3( v )	vec3_t		v		= { 0.0f, 0.0f, 0.0f }
#define NEWVEC4( v )	vec4_t		v		= { 0.0f, 0.0f, 0.0f, 0.0f }
#define NEWTR( tr )		trace_t		tr		= { 0 }
#ifdef CGAME
#define NEWENT( ent )	centity_t	*ent	= NULL
#elif defined QAGAME
#define NEWENT( ent )	gentity_t	*ent	= NULL
#endif

char *Q_bSA(char arrayToBreak[32][64], int maximum);
//some other stuff I thought I should throw in
/*	"Gunslinger's Academy is a mod primary developed by eezstreet (Nick Whitlock) that aims to have a Call of Duty-esque
	experience in Jedi Academy. While the graphics may not be as great, the gameplay can be just as good, if not superior."

	- CREDITS -

	eezstreet - Coding, Mapping, Skinning
	Silverfang - Mapping, Modelling
	Raz0r - Code bytes here and there
	Didz
	Xycaleth
	[UU]BobaFett
	TheOneAndOnly - Effects
	Tobe_One - Skins
	Suibuku - Skins
	*/