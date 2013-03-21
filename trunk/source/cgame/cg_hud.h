#define MAX_TICS	25

typedef struct{
	int ammoTicPosX[MAX_TICS];
	int ammoTicPosY[MAX_TICS];
	int ammoTicHeight[MAX_TICS];
	int ammoTicWidth[MAX_TICS];
	int numTics;
	qboolean fadeTics;
	qboolean teamTics;
	char *ammoTicFiles[MAX_TICS];

	//Clips and total ammo
	qboolean activeClip;
	char *clipHeader;
	int clipX;
	int clipY;
	int clipW;
	int clipH;

#ifdef AKIMBO
	int aClipX;
	int aClipY;
	int aClipW;
	int aClipH;
#endif

	int clipTextX;
	int clipTextY;
	int clipTextW;
	int clipCharW;
	int clipCharH;
#ifdef AKIMBO
	int aClipTextX;
	int aClipTextY;
	int aClipTextW;
	int aClipCharW;
	int aClipCharH;
#endif
	int clipFont;
	qboolean clipDrawZeroes;
	vec4_t clipColor;

	qboolean activeTotal;
	int totalX;
	int totalY;
	int totalW;
	int totalH;
#ifdef AKIMBO
	int aTotalX;
	int aTotalY;
	int aTotalW;
	int aTotalH;
#endif
	
	int totalTextX;
	int totalTextY;
	int totalTextW;
	int totalCharW;
	int totalCharH;
#ifdef AKIMBO
	int aTotalTextX;
	int aTotalTextY;
	int aTotalTextW;
	int aTotalCharW;
	int aTotalCharH;
#endif
	int totalFont;
	qboolean totalDrawZeroes;
	vec4_t totalColor;
} ammoTics_t;

typedef struct{
	qboolean activeModule;
	qboolean horizontal;
	qboolean vertical;

	int yourTeamX;
	int yourTeamY;
	int enemyTeamX;
	int enemyTeamY;

	int flagIconSize;

	const char *CTYRedFlagTakenEnemy;
	const char *CTYBlueFlagTakenEnemy;

	const char *CTFRedMyFlagTaken;
	const char *CTFBlueMyFlagTaken;

	const char *CTFRedFlagTaken;
	const char *CTFBlueFlagTaken;
} ctfData_t;

typedef struct{
	qboolean activeChatBox;
	qboolean activeEventBox;
	qboolean activeCenterPrint;
	float chatBoxFontSize;
	float evBoxFontSize;

	int chatBoxFont;
	qboolean customChatBoxFont;
	const char *chatBoxFontCustomLoc;

	int evBoxFont;
	qboolean customEVBoxFont;
	const char *evBoxFontCustomLoc;

	const char *chatHeader;
	const char *evHeader;
	const char *chatFooter;
	const char *evFooter;

	int centerPrintFont;
	qboolean customCBFont;
	const char *cbFontCustomLoc;

	float cbScale;

	const char *centerPrintHeader;
	const char *centerPrintFooter;

	int chatBoxX;
	int chatBoxY;
	int eventBoxX;
	int eventBoxY;

	const char *bgLoc;
	int bgX;
	int bgY;
	int bgW;
	int bgH;

	qboolean activeNade;
	const char *nadeHeader;
	int nadeX;
	int nadeY;
	int nadeW;
	int nadeH;

	int nadeTextX;
	int nadeTextY;
	int nadeTextW;
	int nadeCharW;
	int nadeCharH;
	int nadeFont;
	qboolean nadeDrawZeroes;
	vec4_t nadeTextColor;

	int fmx;
	int fmy;
} generalData_t;

typedef struct{
	ammoTics_t ammo;
	ctfData_t ctf;
	generalData_t general;
} hudData_t;

typedef struct{
	hudData_t hud;
} iniFileData_t;

iniFileData_t data;