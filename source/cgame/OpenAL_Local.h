#ifdef _WIN32
#pragma once
#include "cg_local.h"
#include <al.h>
#include <alc.h>
#include <AL/alut.h>
//#include "OpenAL/Framework.h"

void GSA_OpenALHackeryInit(void);
void GSA_OpenALHackeryExit(void);

#define NUM_ALBUFFERS	8 //Number of sound buffers //TODO: cvar me
#define BUFFERSIZEAL	4096

ALint soundBuffers[NUM_ALBUFFERS];

//void GSA_OpenALHelloWorld(void);
#endif