#include "q_shared.h"
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOGDI //don't make me do this the hard way
#include <windows.h>
#endif

#if MAC_PORT
#include <sys/mman.h>
#endif

#ifdef _WIN32
#define WIN32_OR_LINUX
#endif
#ifdef _LINUX
#define WIN32_OR_LINUX
#endif

#if !MAC_PORT
typedef qboolean bool;
#endif
typedef struct hookEntry_s
{
	const unsigned int hookPosition; // The code we're patching
	unsigned char  origBytes[5]; // What it originally was
	unsigned char  hookOpcode;  // CALL or JMP
	const unsigned int hookForward; // Function to direct the control flow into
	const char   *name;   // Long name of the hook
} hookEntry_t;

bool UnlockMemory( int address, int size );
bool LockMemory( int address, int size );
void PlaceHook( hookEntry_t *hook );
void RemoveHook( const hookEntry_t *hook );

#define PATCH( address, type, to )		\
{									\
	UnlockMemory( address, 1 );		\
	*(type *)address = (type)to;	\
	LockMemory( address, 1 );		\
}

#ifndef JAMPDED
#define dwExStyleFullscreenAddress 0x4C5794
#define dwStyleFullscreenAddress 0x4C579C
#define dwStyleWindowedAddress 0x4C57B5

//PATCH( 0X4549A1, byte, 0xEB ); //Always enable alt-tab
#endif
