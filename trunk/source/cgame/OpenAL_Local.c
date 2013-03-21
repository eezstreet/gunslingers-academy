#ifdef _WIN32
#include "OpenAL_Local.h"

vec3_t listenerPos = {0, 0, 0};
vec3_t listenerVel = {0, 0, 0};
ALCdevice *gsaDevice = NULL;
ALCcontext *gsaDeviceContext = NULL;
ALboolean EAXcompatible;

void GSA_OpenALHackeryInit(void)
{
	int errorCode;
	Com_Printf("------------------------\n");
	Com_Printf("Initializing OpenAL 11\n");
	Com_Printf("------------------------\n");

	//Establish device context
	gsaDevice = alcOpenDevice(NULL);
	if(gsaDevice)
	{
		gsaDeviceContext = alcCreateContext(gsaDevice, NULL);
		alcMakeContextCurrent(gsaDeviceContext);
	}

	//Check for EAX 2.0 support (fixme: remove?)
	EAXcompatible = alIsExtensionPresent("EAX2.0");
	if(EAXcompatible)
	{
		Com_Printf("EAX 2.0 compatible\n");
	}
	else
	{
		Com_Printf("^3WARNING: EAX 2.0 is not supported\n");
	}

	//Clear errors
	alGetError();

	//Create sound buffers
	alGenBuffers(NUM_ALBUFFERS, soundBuffers);

	//Check for errors
	if((errorCode = alGetError()) != AL_NO_ERROR)
	{
		Com_Error(ERR_FATAL, "alGetError: %i", errorCode);
		return;
	}
}

void GSA_OpenALHackeryExit(void)
{
	gsaDeviceContext = alcGetCurrentContext();
	gsaDevice = alcGetContextsDevice(gsaDeviceContext);
	alcMakeContextCurrent(NULL);
	alcDestroyContext(gsaDeviceContext);
	alcCloseDevice(gsaDevice);
}
#endif