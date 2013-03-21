#include "q_shared.h"
#include "bg_public.h"
#include "bg_local.h"

//This primarily handles sounds for doors, there's basically no other purpose for it but that.
char NewDoorSounds[64][64];
static int numDoorSounds;

//This function should be called every startup.
void BG_CreateDoorSoundIndeces(void)
{
	fileHandle_t f;
	int i = 0;
	int j = 0;
	int k = 0;
	char buffer[512];
	int len = trap_FS_FOpenFile("sound/movers/new_doors.txt", &f, FS_READ);
	numDoorSounds = 0;
	if(!f || !len || len == -1)
	{
		if(f)
		{
			trap_FS_FCloseFile(f);
		}
		Com_Error(ERR_FATAL, "new_doors.txt not found.");
		return;
	}
	if(len > sizeof(buffer))
	{
		trap_FS_FCloseFile(f);
		Com_Error(ERR_DISCONNECT, "new_doors.txt too large (%i > %i)", len, sizeof(buffer));
		return;
	}

	trap_FS_Read(buffer, len, f);
	trap_FS_FCloseFile(f);
	while(buffer[i] != '\0')
	{
		if(buffer[i] == '\r')
		{
			i++;
			continue;
		}
		else if(buffer[i] == '\n')
		{
			j++;
			i++;
			k = 0;
			numDoorSounds++;
			continue;
		}
		NewDoorSounds[j][k++] = buffer[i];
		i++;
	}
}

int BG_GetDoorSoundIndex(char *compText)
{
	int i;
	for(i = 0; i < numDoorSounds; i++)
	{
		if(!Q_stricmp(compText, NewDoorSounds[i]))
		{
			return i;
		}
	}
	return -1;
}

void BG_GetDoorSoundName(int doorNum, char *buffer)
{
	strcpy(buffer, NewDoorSounds[doorNum]);
}