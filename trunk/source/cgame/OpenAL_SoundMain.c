#ifdef _WIN32
#include "OpenAL_Local.h"

void GSA_OpenALPlayWAV(const char *fileName)
{
}

/*void GSA_OpenALHelloWorld(void)
{
	ALuint sourcePoint;
	int errorCode = 0;
	soundBuffers[0] = alutCreateBufferHelloWorld();
	alGenSources(1, &sourcePoint);
	alSourcei(sourcePoint, AL_BUFFER, soundBuffers[0]);
	alSourcePlay(sourcePoint);

	errorCode = alGetError();
	if(errorCode)
	{
		Com_Printf("^1ERROR: %s\n", alutGetErrorString(errorCode));
	}
}*/
#endif