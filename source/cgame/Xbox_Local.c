#include "Xbox_Local.h"

extern vmCvar_t xbox_power;
extern vmCvar_t xbox_rumbleModifier;
extern vmCvar_t xbox_sensitivity;
extern vmCvar_t	xbox_invertXLook;
extern vmCvar_t	xbox_invertYLook;

extern void CG_CenterPrint( const char *str, int y, int charWidth );
void Xbox_ControllerStatus(void)
{
	DWORD dwResult;
	XINPUT_STATE state;

	if(!xbox_power.integer)
		return; //Not powered on!

	ZeroMemory( &state, sizeof(XINPUT_STATE) );
	dwResult = XInputGetState(0, &state); //Get the XInput state

	if(dwResult == ERROR_SUCCESS)
	{
		//Controller is connected.
		if(xboxSignalHandler[0] == 'r')
		{ //We have a rumble in our queue
			XINPUT_VIBRATION vibration;
			ZeroMemory( &vibration, sizeof(XINPUT_VIBRATION) );

			vibration.wLeftMotorSpeed = atoi(xboxAltSignal);
			vibration.wRightMotorSpeed = atoi(xboxAltSignal);

			XInputSetState(0, &vibration);

			xbox.rumbleIntensity = atoi(xboxAltSignal);
			xbox.rumbleTime = atoi(xboxSignalArg);
			xbox.rumbleStartTime = cg.time;
			xboxSignalHandler[0] = '\0';
		}
		else if((xbox.rumbleStartTime + xbox.rumbleTime) < cg.time)
		{ //We have no rumble currently playing or in our queue
			XINPUT_VIBRATION vibration;
			ZeroMemory( &vibration, sizeof(XINPUT_VIBRATION) );

			vibration.wLeftMotorSpeed = 0;
			vibration.wRightMotorSpeed = 0;

			XInputSetState(0, &vibration);
		}

		//Thumbstick checks - Right Stick
		{
			//Get threshold
			float threshold;
			float rightThumbY = state.Gamepad.sThumbRY;
			float rightThumbX = state.Gamepad.sThumbRX;
			char tempBuf[32];

			trap_Cvar_VariableStringBuffer("joy_threshold", tempBuf, 32);
			threshold = atof(tempBuf);
			threshold *= 32768; //XBOX controllers have thumbsticks handled in 1/32768ths, while JA's are normalized. BAD!
			if(threshold != 0)
			{
				//Our threshold is non-zero
				if(rightThumbX < (-threshold))
				{
					//Right stick moved to the left
					float yawSpeed = (((abs(rightThumbX)-abs(threshold))/32768.0f)*140.0f);
					float fovPercent = cg.refdef.fov_x/cg_fov.value;
					yawSpeed *= fovPercent;
					yawSpeed *= xbox_sensitivity.value;
					if(!xbox.isLookingLeft)
					{
						xbox.isLookingLeft = qtrue;
						trap_SendConsoleCommand("+left\n");
					}
					if(xbox_invertXLook.integer)
						yawSpeed *= -1.0f; //Inverted look
					trap_Cvar_Set("cl_yawspeed", va("%i", (int)yawSpeed));
				}
				else if(rightThumbX > threshold)
				{
					//Right stick moved to the right
					float yawSpeed = (((abs(rightThumbX)-abs(threshold))/32768.0f)*140.0f);
					float fovPercent = cg.refdef.fov_x/cg_fov.value;
					yawSpeed *= fovPercent;
					yawSpeed *= xbox_sensitivity.value;
					if(!xbox.isLookingRight)
					{
						xbox.isLookingRight = qtrue;
						trap_SendConsoleCommand("+right\n");
					}
					if(xbox_invertXLook.integer)
						yawSpeed *= -1.0f; //Inverted look
					trap_Cvar_Set("cl_yawspeed", va("%i", (int)yawSpeed));
				}
				else
				{
					//Right Stick is neither left or right
					if(xbox.isLookingLeft)
					{
						xbox.isLookingLeft = qfalse;
						trap_SendConsoleCommand("-left\n");
					}
					if(xbox.isLookingRight)
					{
						xbox.isLookingRight = qfalse;
						trap_SendConsoleCommand("-right\n");
					}
				}
				if(rightThumbY < (-threshold))
				{
					//Right Stick pointed down
					float pitchSpeed = (((abs(rightThumbY)-abs(threshold))/32768.0f)*140.0f);
					float fovPercent = cg.refdef.fov_y/cg_fov.value;
					pitchSpeed *= fovPercent;
					pitchSpeed *= xbox_sensitivity.value;
					if(!xbox.isLookingDown)
					{
						xbox.isLookingDown = qtrue;
						trap_SendConsoleCommand("+lookDown\n");
					}
					if(xbox_invertYLook.integer)
						pitchSpeed *= -1.0f; //Inverted look
					trap_Cvar_Set("cl_pitchspeed", va("%i", (int)pitchSpeed));
				}
				else if(rightThumbY > threshold)
				{
					//Right Stick pointed up
					float pitchSpeed = (((abs(rightThumbY)-abs(threshold))/32768.0f)*140.0f);
					float fovPercent = cg.refdef.fov_y/cg_fov.value;
					pitchSpeed *= fovPercent;
					pitchSpeed *= xbox_sensitivity.value;
					
					if(!xbox.isLookingUp)
					{
						xbox.isLookingUp = qtrue;
						trap_SendConsoleCommand("+lookUp\n");
					}
					if(xbox_invertYLook.integer)
						pitchSpeed *= -1.0f; //Inverted look
					trap_Cvar_Set("cl_pitchspeed", va("%i", (int)pitchSpeed));
				}
				else
				{
					//Right stick not up/down
					if(xbox.isLookingUp)
					{
						xbox.isLookingUp = qfalse;
						trap_SendConsoleCommand("-lookUp\n");
					}
					if(xbox.isLookingDown)
					{
						xbox.isLookingDown = qfalse;
						trap_SendConsoleCommand("-lookDown\n");
					}
				}
			}
		}
		//Thumbstick checks - Left stick
		{
			float threshold;
			float leftThumbY = state.Gamepad.sThumbLY;
			float leftThumbX = state.Gamepad.sThumbLX;
			char tempBuf[32];
			qboolean indecisive = qfalse;
			qboolean runOverride = qfalse;

			trap_Cvar_VariableStringBuffer("joy_threshold", tempBuf, 32);
			threshold = atof(tempBuf);
			threshold *= 32768; //XBOX controllers have thumbsticks handled in 1/32768ths, while JA's are normalized. BAD!
			if(threshold != 0)
			{
				if(leftThumbY > threshold)
				{
					if((((abs(leftThumbY)-abs(threshold))/32768.0f)*100.0f) < 50.0f)
					{
						if(!xbox.isWalking)
						{
							trap_SendConsoleCommand("+speed\n");
							xbox.isWalking = qtrue;
						}
					}
					else if(xbox.isWalking)
					{
						xbox.isWalking = qfalse;
						trap_SendConsoleCommand("-speed\n");
					}
					if(!xbox.isMovingForward)
					{
						xbox.isMovingForward = qtrue;
						trap_SendConsoleCommand("+forward\n");
					}
					if(!xbox.isWalking)
					{
						runOverride = qtrue;
					}
				}
				else if(leftThumbY < (-threshold))
				{
					if((((abs(leftThumbY)-abs(threshold))/32768.0f)*100.0f) < 50.0f)
					{
						if(!xbox.isWalking)
						{
							trap_SendConsoleCommand("+speed\n");
							xbox.isWalking = qtrue;
						}
					}
					else if(xbox.isWalking)
					{
						xbox.isWalking = qfalse;
						trap_SendConsoleCommand("-speed\n");
					}
					if(!xbox.isMovingBack)
					{
						xbox.isMovingBack = qtrue;
						trap_SendConsoleCommand("+back\n");
					}
					if(!xbox.isWalking)
					{
						runOverride = qtrue;
					}
				}
				else
				{
					if(xbox.isMovingForward)
					{
						xbox.isMovingForward = qfalse;
						trap_SendConsoleCommand("-forward\n");
					}
					if(xbox.isMovingBack)
					{
						xbox.isMovingBack = qfalse;
						trap_SendConsoleCommand("-back\n");
					}
					indecisive = qtrue;
				}
				if(leftThumbX < (-threshold))
				{
					if((((abs(leftThumbX)-abs(threshold))/32768.0f)*100.0f) < 50.0f)
					{
						if(!xbox.isWalking && !runOverride)
						{
							trap_SendConsoleCommand("+speed\n");
							xbox.isWalking = qtrue;
						}
					}
					else if(xbox.isWalking)
					{
						xbox.isWalking = qfalse;
						trap_SendConsoleCommand("-speed\n");
					}
					if(!xbox.isMovingLeft)
					{
						xbox.isMovingLeft = qtrue;
						trap_SendConsoleCommand("+moveleft\n");
					}
				}
				else if(leftThumbX > threshold)
				{
					if((((abs(leftThumbX)-abs(threshold))/32768.0f)*100.0f) < 50.0f)
					{
						if(!xbox.isWalking && !runOverride)
						{
							trap_SendConsoleCommand("+speed\n");
							xbox.isWalking = qtrue;
						}
					}
					else if(xbox.isWalking)
					{
						xbox.isWalking = qfalse;
						trap_SendConsoleCommand("-speed\n");
					}
					if(!xbox.isMovingRight)
					{
						xbox.isMovingRight = qtrue;
						trap_SendConsoleCommand("+moveright\n");
					}
				}
				else
				{
					if(xbox.isMovingLeft)
					{
						xbox.isMovingLeft = qfalse;
						trap_SendConsoleCommand("-moveleft\n");
					}
					if(xbox.isMovingRight)
					{
						xbox.isMovingRight = qfalse;
						trap_SendConsoleCommand("-moveright\n");
					}
					if(indecisive)
					{
						xbox.isWalking = qfalse;
						trap_SendConsoleCommand("-speed\n");
					}
				}
			}
		}
	}
	else
	{
		//Controller is disconnected
		CG_CenterPrint("Please reconnect your XBOX 360 controller.", 120, GIANTCHAR_WIDTH*2);
		return;
	}
}

DWORD WINAPI Xbox_Init(LPVOID args)
{ //Not used - Multithreaded version
	while(xboxSignalHandler[0] != '1')
	{
		Xbox_ControllerStatus();
		Sleep(1);
	}
	return 0;
}

void Xbox_Multithread_Init(void)
{
	//Start a new thread!
	/*DWORD threadID;
	HANDLE thread;
	thread = CreateThread( NULL, 0, Xbox_Init, NULL, 0, &threadID);*/

	//Initialize everything!
	xboxSignalHandler[0] = '\0';
	xboxSignalArg[0] = '\0';
	xboxAltSignal[0] = '\0';

	xbox.isLookingLeft = qfalse;
	xbox.isLookingRight = qfalse;
	xbox.isLookingUp = qfalse;
	xbox.isLookingDown = qfalse;
}

void Xbox_Multithread_Exit(void)
{
	//Not used - Multithreaded version
	xboxSignalHandler[0] = '1';
}

void Xbox_Rumble(short intensity, unsigned int msec)
{
	//Send a rumble to the controller
	xboxSignalHandler[0] = 'r'; //R is for rumble!
	strcpy(xboxAltSignal, va("%i", (int)(intensity * xbox_rumbleModifier.value)));
	strcpy(xboxSignalArg, va("%i", msec));
}