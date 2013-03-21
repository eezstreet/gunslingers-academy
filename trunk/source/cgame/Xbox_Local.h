#include "cg_local.h"
#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <XInput.h>
#pragma comment(lib, "XInput.lib")

char xboxSignalHandler[16];
char xboxAltSignal[16];
char xboxSignalArg[16];

typedef struct 
{
	int rumbleTime;
	int rumbleStartTime;
	int rumbleIntensity;
	qboolean isLookingLeft;
	qboolean isLookingRight;
	qboolean isLookingUp;
	qboolean isLookingDown;

	qboolean isMovingLeft;
	qboolean isMovingRight;
	qboolean isMovingForward;
	qboolean isMovingBack;

	qboolean isWalking;

} xboxController_t;

xboxController_t xbox;