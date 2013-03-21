#include "cg_ini.h"
#include "cg_hud.h"

void loadIniFile(const char *fileName){
	dictionary *file;
	int i;
	char buf[128];
	const char *reference;
	vec4_t defaultvec4 = {1.0, 1.0, 1.0, 1.0};

	file = iniparser_load(fileName);

	//AMMO DATA
	data.hud.ammo.fadeTics = iniparser_getboolean(file, "Ammo:ActiveAmmoFadeTics", 1);
	data.hud.ammo.teamTics = iniparser_getboolean(file, "Ammo:ActiveAmmoTeamTics", 1);
	data.hud.ammo.numTics = iniparser_getint(file, "Ammo:AmmoNumTics", 1);
	memset(&data.hud.ammo.ammoTicFiles, '0', sizeof(data.hud.ammo.ammoTicFiles));
	for(i = 1; i <= data.hud.ammo.numTics; i++){
		strcpy(buf, va("ammo:AmmoTic%iFile", i));
		data.hud.ammo.ammoTicFiles[i] = iniparser_getstring(file, buf, "");
		strcpy(buf, va("ammo:AmmoTic%ixPos", i));
		data.hud.ammo.ammoTicPosX[i] = iniparser_getint(file, buf, 0);
		strcpy(buf, va("ammo:AmmoTic%iyPos", i));
		data.hud.ammo.ammoTicPosY[i] = iniparser_getint(file, buf, 0);
		strcpy(buf, va("ammo:AmmoTic%iHeight", i));
		data.hud.ammo.ammoTicHeight[i] = iniparser_getint(file, buf, 0);
		strcpy(buf, va("ammo:AmmoTic%iWidth", i));
		data.hud.ammo.ammoTicWidth[i] = iniparser_getint(file, buf, 0);
	}

	//CLIPS AND TOTAL
	data.hud.ammo.activeClip = iniparser_getboolean(file, "Clips:ActiveAmmoInClip", 0);
	//if(data.hud.ammo.activeClip)
	{
		data.hud.ammo.clipHeader = iniparser_getstring(file, "Clips:ClipHeader", "");
		data.hud.ammo.clipX = iniparser_getint(file, "Clips:ClipX", 0);
		data.hud.ammo.clipY = iniparser_getint(file, "Clips:ClipY", 0);
		data.hud.ammo.clipW = iniparser_getint(file, "Clips:ClipW", 0);
		data.hud.ammo.clipH = iniparser_getint(file, "Clips:ClipH", 0);

		data.hud.ammo.clipFont = iniparser_getint(file, "Clips:ClipFont", 0);
		data.hud.ammo.clipTextW = iniparser_getint(file, "Clips:ClipTextW", 0);
		data.hud.ammo.clipTextX = iniparser_getint(file, "Clips:ClipTextX", 0);
		data.hud.ammo.clipTextY = iniparser_getint(file, "Clips:ClipTextY", 0);
		data.hud.ammo.clipCharW = iniparser_getint(file, "Clips:ClipCharW", 0);
		data.hud.ammo.clipCharH = iniparser_getint(file, "Clips:ClipCharH", 0);
#ifdef AKIMBO
		data.hud.ammo.aClipX = iniparser_getint(file, "Clips:AkimboClipX", 0);
		data.hud.ammo.aClipY = iniparser_getint(file, "Clips:AkimboClipY", 0);
		data.hud.ammo.aClipW = iniparser_getint(file, "Clips:AkimboClipW", 0);
		data.hud.ammo.aClipH = iniparser_getint(file, "Clips:AkimboClipH", 0);

		data.hud.ammo.aClipTextW = iniparser_getint(file, "Clips:AkimboClipTextW", 0);
		data.hud.ammo.aClipTextX = iniparser_getint(file, "Clips:AkimboClipTextX", 0);
		data.hud.ammo.aClipTextY = iniparser_getint(file, "Clips:AkimboClipTextY", 0);
		data.hud.ammo.aClipCharW = iniparser_getint(file, "Clips:AkimboClipCharW", 0);
		data.hud.ammo.aClipCharH = iniparser_getint(file, "Clips:AkimboClipCharH", 0);
#endif
		data.hud.ammo.clipDrawZeroes = iniparser_getboolean(file, "Clips:ActiveDrawClipZeroes", 0);
		VectorCopy4(iniparser_getvec4(file, "Clips:ClipTextColor", defaultvec4), data.hud.ammo.clipColor); //holy hell how is this legal
	}
	data.hud.ammo.activeTotal = iniparser_getboolean(file, "Clips:ActiveAmmoTotal", 0);
	//if(data.hud.ammo.activeTotal)
	{
		data.hud.ammo.totalX = iniparser_getint(file, "Clips:AmmoTotalX", 0);
		data.hud.ammo.totalY = iniparser_getint(file, "Clips:AmmoTotalY", 0);
		data.hud.ammo.totalW = iniparser_getint(file, "Clips:AmmoTotalW", 0);
		data.hud.ammo.totalH = iniparser_getint(file, "Clips:AmmoTotalH", 0);

		data.hud.ammo.totalFont = iniparser_getint(file, "Clips:AmmoTotalFont", 0);
		data.hud.ammo.totalTextW = iniparser_getint(file, "Clips:AmmoTotalTextW", 0);
		data.hud.ammo.totalTextX = iniparser_getint(file, "Clips:AmmoTotalTextX", 0);
		data.hud.ammo.totalTextY = iniparser_getint(file, "Clips:AmmoTotalTextY", 0);
		data.hud.ammo.totalCharW = iniparser_getint(file, "Clips:AmmoTotalCharW", 0);
		data.hud.ammo.totalCharH = iniparser_getint(file, "Clips:AmmoTotalCharH", 0);
#ifdef AKIMBO
		data.hud.ammo.aTotalX = iniparser_getint(file, "Clips:AkimboAmmoTotalX", 0);
		data.hud.ammo.aTotalY = iniparser_getint(file, "Clips:AkimboAmmoTotalY", 0);
		data.hud.ammo.aTotalW = iniparser_getint(file, "Clips:AkimboAmmoTotalW", 0);
		data.hud.ammo.aTotalH = iniparser_getint(file, "Clips:AkimboAmmoTotalH", 0);

		data.hud.ammo.aTotalTextW = iniparser_getint(file, "Clips:AkimboAmmoTotalTextW", 0);
		data.hud.ammo.aTotalTextX = iniparser_getint(file, "Clips:AkimboAmmoTotalTextX", 0);
		data.hud.ammo.aTotalTextY = iniparser_getint(file, "Clips:AkimboAmmoTotalTextY", 0);
		data.hud.ammo.aTotalCharW = iniparser_getint(file, "Clips:AkimboAmmoTotalCharW", 0);
		data.hud.ammo.aTotalCharH = iniparser_getint(file, "Clips:AkimboAmmoTotalCharH", 0);
#endif
		data.hud.ammo.totalDrawZeroes = iniparser_getboolean(file, "Clips:ActiveDrawAmmoTotalZeroes", 0);
		VectorCopy4(iniparser_getvec4(file, "Clips:AmmoTotalTextColor", defaultvec4), data.hud.ammo.totalColor);
	}
	//iniparser_freedict(file);

	//BACKGROUND
	data.hud.general.bgLoc = iniparser_getstring(file, "background:File", "");
	data.hud.general.bgW = iniparser_getint(file, "background:BgW", 0);
	data.hud.general.bgH = iniparser_getint(file, "background:BgH", 0);
	//haxxy shortcut to make it draw in the bottom corner by default ~eezstreet
	data.hud.general.bgX = iniparser_getint(file, "background:BgX", SCREEN_WIDTH-data.hud.general.bgW);
	data.hud.general.bgY = iniparser_getint(file, "background:BgY", SCREEN_HEIGHT-data.hud.general.bgH);

	//QUICK THROW
	data.hud.general.activeNade = iniparser_getboolean(file, "quickthrow:ActiveDrawQuickNade", 0);
	if(data.hud.general.activeNade)
	{
		data.hud.general.nadeHeader = iniparser_getstring(file, "quickthrow:NadeHeader", "");
		data.hud.general.nadeX = iniparser_getint(file, "quickthrow:NadeX", 0);
		data.hud.general.nadeY = iniparser_getint(file, "quickthrow:NadeY", 0);
		data.hud.general.nadeW = iniparser_getint(file, "quickthrow:NadeW", 0);
		data.hud.general.nadeH = iniparser_getint(file, "quickthrow:NadeH", 0);

		data.hud.general.nadeFont = iniparser_getint(file, "Quickthrow:NadeFont", 0);
		data.hud.general.nadeTextW = iniparser_getint(file, "Quickthrow:NadeTextW", 0);
		data.hud.general.nadeTextX = iniparser_getint(file, "Quickthrow:NadeTextX", 0);
		data.hud.general.nadeTextY = iniparser_getint(file, "Quickthrow:NadeTextY", 0);
		data.hud.general.nadeCharW = iniparser_getint(file, "Quickthrow:NadeCharW", 0);
		data.hud.general.nadeCharH = iniparser_getint(file, "Quickthrow:NadeCharH", 0);
		data.hud.general.nadeDrawZeroes = iniparser_getboolean(file, "quickthrow:ActiveDrawQuickNadeZeroes", 0);
		VectorCopy4(iniparser_getvec4(file, "quickthrow:NadeTextColor", defaultvec4), data.hud.general.nadeTextColor);
	}

	data.hud.general.fmx = iniparser_getint(file, "FiringModes:FMX", 0);
	data.hud.general.fmy = iniparser_getint(file, "FiringModes:FMY", 0);

	//CTF DATA
	data.hud.ctf.activeModule = iniparser_getboolean(file, "CTF:ActiveDrawCTFFlags", 0);
	if(data.hud.ctf.activeModule){
		data.hud.ctf.horizontal = iniparser_getboolean(file, "CTF:ActiveHorizontalFlags", 0);
		data.hud.ctf.vertical = iniparser_getboolean(file, "CTF:ActiveVerticalFlags", 1);
		if(data.hud.ctf.vertical && data.hud.ctf.horizontal){
			Com_Printf("^3WARNING: data.hud.ctf: invalid horizontal/vertical flags\n");
			Com_Printf("^3Setting it to default values...\n");
			data.hud.ctf.horizontal = qfalse;
			data.hud.ctf.vertical = qtrue;
		}
		data.hud.ctf.yourTeamX = iniparser_getint(file, "CTF:YourTeamX", 0);
		data.hud.ctf.yourTeamY = iniparser_getint(file, "CTF:YourTeamY", 0);
		data.hud.ctf.enemyTeamX = iniparser_getint(file, "CTF:EnemyTeamX", 0);
		data.hud.ctf.enemyTeamY = iniparser_getint(file, "CTF:EnemyTeamY", 0);

		data.hud.ctf.flagIconSize = iniparser_getint(file, "CTF:FlagIconSize", 20);

		data.hud.ctf.CTFBlueFlagTaken = iniparser_getstring(file, "CTF:CTFBlueFlagTaken", "");
		data.hud.ctf.CTFBlueMyFlagTaken = iniparser_getstring(file, "CTF:CTFBlueMyFlagTaken", "");
		data.hud.ctf.CTFRedFlagTaken = iniparser_getstring(file, "CTF:CTFRedFlagTaken", "");
		data.hud.ctf.CTFRedMyFlagTaken = iniparser_getstring(file, "CTF:CTFRedMyFlagTaken", "");

		data.hud.ctf.CTYRedFlagTakenEnemy = iniparser_getstring(file, "CTF:CTYRedFlagTaken", "");
		data.hud.ctf.CTYBlueFlagTakenEnemy = iniparser_getstring(file, "CTF:CTYBlueFlagTaken", "");
	}

	//GENERAL SECTION
	data.hud.general.activeChatBox = iniparser_getboolean(file, "General:ActiveChatBox", 1);
	data.hud.general.activeEventBox = iniparser_getboolean(file, "General:ActiveEventBox", 1);
	data.hud.general.activeCenterPrint = iniparser_getboolean(file, "General:ActiveCenterPrint", 1);
	if(data.hud.general.activeChatBox){
		data.hud.general.chatBoxFontSize = (float)iniparser_getdouble(file, "General:chatBoxFontScale", 0.65f);
		reference = iniparser_getstring(file, "General:chatBoxFont", "FONT_SMALL");
		if(!Q_stricmp(reference, "FONT_SMALL")){
			data.hud.general.chatBoxFont = FONT_SMALL;
		} else if(!Q_stricmp(reference, "FONT_MEDIUM")){
			data.hud.general.chatBoxFont = FONT_MEDIUM;
		} else if(!Q_stricmp(reference, "FONT_LARGE")){
			data.hud.general.chatBoxFont = FONT_LARGE;
		} else if(!Q_stricmp(reference, "FONT_CUSTOM")){
			data.hud.general.chatBoxFont = FONT_SMALL2+1;
			data.hud.general.customChatBoxFont = qtrue;
			data.hud.general.chatBoxFontCustomLoc = iniparser_getstring(file, "General:chatBoxCustomFont", "");
		}
		data.hud.general.chatHeader = iniparser_getstring(file, "General:chatHeader", "^2");
		data.hud.general.chatFooter = iniparser_getstring(file, "General:chatFooter", "");

		data.hud.general.chatBoxX = iniparser_getint(file, "General:chatBoxX", 30);
		data.hud.general.chatBoxY = iniparser_getint(file, "General:chatBoxY", 350);
	}

	if(data.hud.general.activeEventBox){
		data.hud.general.evBoxFontSize = (float)iniparser_getdouble(file, "General:evBoxFontScale", 0.65f);
		reference = iniparser_getstring(file, "General:evBoxFont", "FONT_SMALL");
		if(!Q_stricmp(reference, "FONT_SMALL")){
			data.hud.general.evBoxFont = FONT_SMALL;
		} else if(!Q_stricmp(reference, "FONT_MEDIUM")){
			data.hud.general.evBoxFont = FONT_MEDIUM;
		} else if(!Q_stricmp(reference, "FONT_LARGE")){
			data.hud.general.evBoxFont = FONT_LARGE;
		} else if(!Q_stricmp(reference, "FONT_CUSTOM")){
			data.hud.general.evBoxFont = FONT_SMALL2+1;
			data.hud.general.customEVBoxFont = qtrue;
			data.hud.general.evBoxFontCustomLoc = iniparser_getstring(file, "General:evBoxCustomFont", "");
		}
		data.hud.general.evHeader = iniparser_getstring(file, "General:eventHeader", "^7");
		data.hud.general.evFooter = iniparser_getstring(file, "General:eventFooter", "");

		data.hud.general.eventBoxX = iniparser_getint(file, "General:eventBoxX", 200);
		data.hud.general.eventBoxY = iniparser_getint(file, "General:eventBoxY", 400);
	}

	if(data.hud.general.activeCenterPrint){
		data.hud.general.cbScale = (float)(iniparser_getint(file, "General:centerPrintScale", 100)/100);
		reference = iniparser_getstring(file, "General:centerPrintFont", "FONT_MEDIUM");
		if(!Q_stricmp(reference, "FONT_SMALL")){
			data.hud.general.centerPrintFont = FONT_SMALL;
		} else if(!Q_stricmp(reference, "FONT_MEDIUM")){
			data.hud.general.centerPrintFont = FONT_MEDIUM;
		} else if(!Q_stricmp(reference, "FONT_LARGE")){
			data.hud.general.centerPrintFont = FONT_LARGE;
		} else if(!Q_stricmp(reference, "FONT_CUSTOM")){
			data.hud.general.centerPrintFont = FONT_SMALL2+1;
			data.hud.general.customCBFont = qtrue;
			data.hud.general.cbFontCustomLoc = iniparser_getstring(file, "General:evBoxCustomFont", "");
		}
		data.hud.general.centerPrintHeader = iniparser_getstring(file, "General:centerPrintHeader", "^7");
		data.hud.general.centerPrintFooter = iniparser_getstring(file, "General:centerPrintFooter", "");
	}

}

void onHudSkinCvarChange(void)
{
	fileHandle_t f;
	int len;
	len = trap_FS_FOpenFile(va("%s.ini", cg_hudSkins.string), &f, FS_READ);
	if((!len || !f) && Q_stricmp(cg_hudSkins.string, "default"))
	{
		Com_Printf("^3WARNING: Could not find hudskin %s, trying default instead...\n", cg_hudSkins.string);
		trap_Cvar_Set("cg_hudSkins", "default");
		return;
	}
	loadIniFile(va("%s.ini",cg_hudSkins.string));
}