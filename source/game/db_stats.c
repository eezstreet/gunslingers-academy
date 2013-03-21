//......... ......... .......   ............  ...... ......  .......
//.... .ZMMMMMMMMMMMMMO   :MMMMMMMMMMMMMMMM7 ...... MMMMMMM8........
//....MMMMMMMMMMMMMMMM...MMMMMMMMMMMMMMMMM$....... ,MMMMMMMM........
//...MMMMMMMMMMMMMMMM...=MMMMMMMMMMMMMMMMM.........NMMMMMMMM7.......
//..DMMMMMMMMNNNNNND,...MMMMMMM88NNNNNNNN ........?MMMMMMMMMM.......
// ~MMMMMM7  ...........MMMMMM.      . .........  MMMMMMMMMMM7 .....
//.MMMMMMD.           . MMMMMMMMMMMMMMMM:........7MMMMMMMMMMMM,.....
// MMMMMM~ MMMMMMMMMMMM ~MMMMMMMMMMMMMMMMM ..... MMMMMMDMMMMMMN.....
//.MMMMMM? ,MMMMMMMMMMM..8MMMMMMMMMMMMMMMMM   ..=MMMMMM.,MMMMMM ....
// MMMMMMN .,MMMMMMMMMM... MMMMMMMMMMMMMMMMM.  .MMMMMMMMMMMMMMMD....
// NMMMMMM~. 8MMMMMMMMM....   . .... IMMMMMM.. NMMMMMMMMMMMMMMMM....
//  MMMMMMM7   .$MMMMMM.  ...........:MMMMMM ..MMMMMMIMMMMMMMMMMO...
//..:MMMMMMMMMMMMMMMMMM.MMMMMMMMMMMMMMMMMMMD .ZMMMMMM  MMMMMMMMMM . 
//.. :MMMMMMMMMMMMMMMMM.MMMMMMMMMMMMMMMMMMM . MMMMMM?......MMMMMMM. 
//.. ..$MMMMMMMMMMMMMMM.MMMMMMMMMMMMMMMMM8 ..8MMMMMM. ......MMMMMM:.
//.... . ,OMMMMMMMMMMMM.MMMMMMMMMMMMMMMI  ...MMMMMMZ........MMMMMM8.
//......  . ........................... .  .....  ..................

//db_stats.c: Communication with Stat Server

#include "g_local.h"
#include "db_stats.h"

#ifdef __GSALIBCURL
void retrieveStatURL(const char *fileName, char *buffer)
{
	fileHandle_t f;
	int len = trap_FS_FOpenFile(fileName, &f, FS_READ);

	if(!f){
		Com_Printf("%s: NOT OK\n");
		return;
	}
	if(len >= MAX_URLLEN){
		trap_FS_FCloseFile(f);
		Com_Printf("%s: TOO LARGE\n");
		return;
	}

	trap_FS_Read(buffer, len, f);
	buffer[len] = '\0';
	trap_FS_FCloseFile(f);
}

void ECSS(int eventType, int eParm1, int eParm2)
{
	CURL *cURLHandle = curl_easy_init();
	CURLcode start;
	char URL[MAX_URLLEN];
	int i;

	if(cURLHandle){
		retrieveStatURL("servdat/statserv.txt", URL);

		curl_easy_setopt(cURLHandle, CURLOPT_URL, URL);
		curl_easy_setopt(cURLHandle, CURLOPT_USERAGENT, GSA_USERAGENT);
		switch(eventType){
			case CSSEVENT_KILL:
				curl_easy_setopt(cURLHandle, CURLOPT_POSTFIELDS, va("event=%i&target=%i&killer=%i", eventType, eParm1, eParm2));
				start = curl_easy_perform(cURLHandle);
				break;
		}
	}

	curl_easy_cleanup(cURLHandle);
}
#endif