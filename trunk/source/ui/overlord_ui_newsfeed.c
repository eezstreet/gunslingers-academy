#define WIN32_LEAN_AND_MEAN
#include <curl/curl.h>
#include "ui_local.h"
#include "ui_shared.h"

char newsBuffer[1024];

struct FtpFile {
  const char *filename;
  FILE *stream;
};

extern uiInfo_t uiInfo;
extern menuDef_t *Menus_FindByName(const char *p);
extern void Menu_ShowItemByName(menuDef_t *menu, const char *p, qboolean bShow);

static size_t RefreshNewsCallback(void *buffer, size_t size, size_t nmemb, void *stream)
{
	struct FtpFile *out=(struct FtpFile *)stream;
	char *str = buffer;
	char str2[1024];
	int i = 0;
	int j = 0;
	if(out && !out->stream) {
    /* open file for writing */ 
		out->stream=fopen(out->filename, "wb");
		if(!out->stream)
			return -1; /* failure, can't open file to write */ 
	}
	while(str[i] != '\0')
	{
		if(str[i] == ';' && str[i+1] == '.' && str[i+2] == ';')
		{
			str2[j++] = '\n';
			i += 3;
			continue;
		}
		str2[j] = str[i];
		j++;
		i++;
	}
	str2[j] = '\0';
	strcpy(newsBuffer, str2);
	trap_Cvar_Set("ui_breakingNews", newsBuffer);
	if(!Q_stricmp(newsBuffer, "-1"))
	{
		menuDef_t *menu;
		menu = Menus_FindByName("main");
		Menu_ShowItemByName(menu, "newsbg", qfalse);
		Menu_ShowItemByName(menu, "newsText", qfalse);
	}
	else
	{
		menuDef_t *menu;
		menu = Menus_FindByName("main");
		Menu_ShowItemByName(menu, "newsbg", qtrue);
		Menu_ShowItemByName(menu, "newsText", qtrue);
	}
	return fwrite(buffer, size, nmemb, out->stream);
}

void Overlord_News_RefreshNews(char *newsBuffer)
{
	CURLcode rem;
	CURL *curl;
	char buffer[2048];
	char url[256];

	struct FtpFile ftpfile={
		"breakingnews.dtf", /* name to store the file as if succesful */ 
		NULL
	};

	curl_global_init(CURL_GLOBAL_DEFAULT);
	curl = curl_easy_init();

	if(!curl)
	{
		Com_Error(ERR_FATAL, "Overlord: curl_easy_init() failed");
		return;
	}

	strcpy(url, va("%s/breakingnews.dtf", GetOverlordURL("realmlist.dtf")));

	curl_easy_setopt(curl, CURLOPT_URL, url);
#ifdef _DEBUG
	curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
#endif
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, RefreshNewsCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &ftpfile);

	rem = curl_easy_perform(curl);

	fread(buffer, 1, sizeof(buffer), ftpfile.stream);

	if(ftpfile.stream)
		fclose(ftpfile.stream); /* close the local file */ 

	curl_easy_cleanup(curl);

	uiInfo.lastNewsRefreshTime = uiInfo.uiDC.realTime;
}

void Overlord_News_CheckForAutoRefresh(void)
{
	if(uiInfo.uiDC.realTime > uiInfo.lastNewsRefreshTime + 120000)
	{
		Overlord_News_RefreshNews("\0");
		return;
	}
}