#ifdef CGAME
#include "../cgame/cg_local.h"
#elif defined UI
#include "../ui/ui_shared.h"
#include "../ui/ui_local.h"
#elif defined QAGAME
#include "../game/g_local.h"
#endif

char *GetOverlordURL(char *realmFile)
{
	fileHandle_t f;
	char buffer[1024];
	char retval[1024];
	int len = trap_FS_FOpenFile(va("overlord/%s", realmFile), &f, FS_READ);
	int i = 0;

	if(!len || len < 0 || !f)
	{
		if(f)
		{
			trap_FS_FCloseFile(f);
		}
		Com_Error(ERR_FATAL, "Overlord: Failed to find %s", realmFile);
		return NULL;
	}

	trap_FS_Read(buffer, len, f);
	trap_FS_FCloseFile(f);
	while(buffer[i] != '\0' && buffer[i] != '\r' && buffer[i] != '\n' && buffer[i] > 0)
	{
		retval[i] = buffer[i];
		i++;
	}
	retval[i] = 0;
	return retval;
}