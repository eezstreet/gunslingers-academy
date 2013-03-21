// Filename:-	bg_weapons.h
//
// This crosses both client and server.  It could all be crammed into bg_public, but isolation of this type of data is best.

#ifndef __WEAPONS_H__
#define __WEAPONS_H__

#include "q_shared.h"
#include "db_simple.h"

//[Linux]
#ifndef __linux__
typedef enum {
#else
enum {
#endif
//[/Linux]
	WP_NONE,

	WP_STUN_BATON,
	WP_MELEE,
	WP_SABER,
	WP_BRYAR_PISTOL,
	WP_BLASTER,
	WP_DISRUPTOR,
	WP_BOWCASTER,
	WP_REPEATER,
	WP_DEMP2,
	WP_FLECHETTE,
	WP_ROCKET_LAUNCHER,
	WP_THERMAL,
	WP_TRIP_MINE,
	WP_DET_PACK,
	WP_CONCUSSION,
	WP_BRYAR_OLD,
	WP_EMPLACED_GUN,
	WP_TURRET,
	WP_AUXILIARY1,	//Glock
	WP_AUXILIARY2,	//M1014
	WP_AUXILIARY3,	//Sawed-Off
	WP_AUXILIARY4,	//AUG
	WP_AUXILIARY5,	//FN P90
	WP_AUXILIARY6,	//Stun
	WP_AUXILIARY7,	//Smoke
	WP_AUXILIARY8,	//Gas
	WP_AUXILIARY9,	//USP Match
	WP_AUXILIARY10,	//Micro Uzi
	WP_AUXILIARY11,	//M98C
	WP_AUXILIARY12,	//G36C
	WP_AUXILIARY13, //L85

//	WP_GAUNTLET,
//	WP_MACHINEGUN,			// Bryar
//	WP_SHOTGUN,				// Blaster
//	WP_GRENADE_LAUNCHER,	// Thermal
//	WP_LIGHTNING,			// 
//	WP_RAILGUN,				// 
//	WP_GRAPPLING_HOOK,

	WP_NUM_WEAPONS
};
typedef int weapon_t;

typedef enum
{
	HC_NONE,
	HC_TASER,
	HC_MELEE,
	HC_SABER,
	HC_MANDP,
	HC_TMP,
	HC_M14,
	HC_MP5,
	HC_M16,
	HC_MAGNUM,
	HC_ITHACA,
	HC_RPG,
	HC_GRENADE,
	HC_FLASHBANG,
	HC_C4,
	HC_M249SAW,
	HC_AK104,
	HC_EMPLACED,
	HC_TURRET,
	HC_SAWEDOFF,
	HC_M1014,
	HC_GLOCK18C,
	HC_AUG,
	HC_P90,
	HC_STUN_GRENADE,
	HC_SMOKE_GRENADE,
	HC_GAS_GRENADE,
	HC_USP_MATCH,
	HC_MICRO_UZI,
	HC_M98C,
	HC_G36C,
	HC_DUMMY2,
	HC_MAX
};

#define WP_BOT_LASER WP_NUM_WEAPONS+1 // 20
#define WP_RAPID_FIRE_CONC WP_BOT_LASER+1 // 21
#define WP_JAWA WP_RAPID_FIRE_CONC+1 // 22
#define WP_TUSKEN_RIFLE WP_JAWA+1 // 23
#define WP_TUSKEN_STAFF WP_TUSKEN_RIFLE+1 // 24
#define WP_SCEPTER WP_TUSKEN_STAFF+1 // 25
#define WP_NOGHRI_STICK WP_SCEPTER+1 // 26
#define WP_ATST_MAIN WP_NOGHRI_STICK+1 // 27
#define WP_ATST_SIDE WP_ATST_MAIN+1 // 28

//anything > this will be considered not player useable
#define LAST_USEABLE_WEAPON			WP_AUXILIARY13

//[MOREWEAPOPTIONS]
#define WP_MELEEONLY		524283
#define WP_SABERSONLY		524279
#define WP_MELEESABERS		524275
#define WP_NOEXPLOS			28672				

#define WP_ALLDISABLED		524287
#define FP_ALLDISABLED		262143
//[/MOREWEAPOPTIONS]

//[FireModes]
#define MAX_FIRINGMODES		3				//Max number of firing modes on gun (+1)
//[/FireModes]

typedef enum //# ammo_e
{
	AMMO_NONE,
	AMMO_FORCE,		// AMMO_PHASER
	AMMO_BLASTER,		// 9mm
	AMMO_POWERCELL,		// Revolver
	AMMO_METAL_BOLTS,	// .50 cal
	AMMO_ROCKETS,		// .45
	AMMO_EMPLACED,
	AMMO_THERMAL,
	AMMO_TRIPMINE,
	AMMO_DETPACK,
	AMMO_RPG,
	AMMO_MSIXTEEN,
	AMMO_MILKOR, //problem, conspiracyfag?
	AMMO_SHOTGUN,
	AMMO_AK,
	AMMO_LMG,
	AMMO_TASER,
	AMMO_NINE,
	AMMO_MAX
} ammo_t;
//[FireModes]
typedef enum
{
	FM_NONE,		//Used for melee, etc
	FM_FULLAUTO,	//Fully automatic fire.
	FM_SEMIAUTO,	//Semi automatic fire.
	FM_THREEBURST,	//Three round burst.
	FM_FOURBURST,	//Four round burst.
	FM_SINGLE,		//Taser Stun Gun - Single Cartridge
	FM_DOUBLE,		//Taser Stun Gun - Two cartridges
	FM_MAX
} firingModes_t;
//[/FireModes]

//[Reloading]
typedef enum
{
	RT_NONE,
	RT_STANDARD,
	RT_PROGRESSIVE
} reloadTypes_t;

typedef enum
{
	WT_PRIMARY,
	WT_SIDEARM,
	WT_EXPLOSIVE,
	WT_OTHER
} weaponTypes_t;


typedef enum
{
	WPLINE_NAME,
	WPLINE_INTERNAL,
	WPLINE_ID,
	WPLINE_HCIDX,
	WPLINE_VARIATION,
	WPLINE_AMMOINDEX,
	WPLINE_AMMOLOW,
	WPLINE_AMMOCLIPLOW,
	WPLINE_AMMOSTART,
	WPLINE_ENERGYPERSHOT,
	WPLINE_ALTENERGYPERSHOT,
	WPLINE_FIREFUNCINVERT,
	WPLINE_ALTFIREFUNCINVERT,
	WPLINE_FIREFUNC,
	WPLINE_ALTFIREFUNC,
	WPLINE_ALTSCOPE,
	WPLINE_SPREAD,
	WPLINE_ALTSPREAD,
	WPLINE_NUMSHOTS,
	WPLINE_ALTNUMSHOTS,
	WPLINE_RECOIL,				//new in B2.2
	WPLINE_ALTRECOIL,			//new in B2.2
	WPLINE_AKIMBORECOIL,	//Does not stack
	WPLINE_HRECOIL,
	WPLINE_ALTHRECOIL,
	WPLINE_AKIMBOHRECOIL,	//Does not stack
	WPLINE_DAM,
	WPLINE_ALTDAM,
	WPLINE_MOD,
	WPLINE_ALTMOD,
	WPLINE_SPEEDMOD,
	WPLINE_ADSSPEEDMOD,
	WPLINE_ADSFOVMOD,
	WPLINE_ADSTIME,
	WPLINE_ZOOMSTARTFOV,
	WPLINE_ZOOMENDFOV,
	WPLINE_ZOOMSPEED,
	WPLINE_ZOOMTYPE,
	WPLINE_WTYPE,
	WPLINE_ALWAYSSEMI,
	WPLINE_ALTALWAYSSEMI,
	WPLINE_NUMFIREMODES,
	WPLINE_FMODE1,
	WPLINE_FMODE2,
	WPLINE_FMODE3,
	WPLINE_AFMODE1,
	WPLINE_AFMODE2,
	WPLINE_AFMODE3,
	WPLINE_DFMODE,
	WPLINE_FTFMODE1,
	WPLINE_FTFMODE2,
	WPLINE_FTFMODE3,
	WPLINE_AFTFMODE1,
	WPLINE_AFTFMODE2,
	WPLINE_AFTFMODE3,
	WPLINE_ATHRESHOLD,
	WPLINE_ADRAINFM1,
	WPLINE_ADRAINFM2,
	WPLINE_ADRAINFM3,
	WPLINE_FDELAY1,
	WPLINE_FDELAY2,
	WPLINE_FDELAY3,
	WPLINE_CLIPSIZE,
	WPLINE_RTIME,
	WPLINE_RTYPE,
	WPLINE_AUTORELOAD,
	WPLINE_BULLETSPEED,
	WPLINE_ALTBULLETSPEED,
	WPLINE_HMOD,
	WPLINE_FMOD,
	WPLINE_SMOD,
	WPLINE_CMOD,
	WPLINE_STMOD,
	WPLINE_GMOD,
	WPLINE_LMOD,
	WPLINE_FOMOD,
	WPLINE_AHMOD,
	WPLINE_AFMOD,
	WPLINE_ASMOD,
	WPLINE_ACMOD,
	WPLINE_ASTMOD,
	WPLINE_AGMOD,
	WPLINE_ALMOD,
	WPLINE_AFOMOD,
	WPLINE_FLASHSOUND,
	WPLINE_ALTFLASHSOUND,
	WPLINE_MUZZLEEFFECT,
	WPLINE_ALTMUZZLEEFFECT,
	WPLINE_SELECTSOUND,
	WPLINE_SWAPICONL,
	WPLINE_SWAPICONW,
	WPLINE_MISSILEMODEL,
	WPLINE_MODELFOLDER,
	WPLINE_MD3NAME,
	WPLINE_GHOUL2,
	WPLINE_READYANIM,
	WPLINE_LEGSREADYANIM,
	WPLINE_ATTACKANIM,
	WPLINE_AKIMBOANIM,
	WPLINE_ATTACKANIMMS,
	WPLINE_AKIMBOATTACKANIMMS,
	WPLINE_LIMBDISMEMBER,
	WPLINE_BOBPITCH,
	WPLINE_BOBYAW,
	WPLINE_BOBROLL,
	WPLINE_SWAYPITCH,
	WPLINE_SWAYYAW,
	WPLINE_SWAYROLL,
	WPLINE_SWAYTIMESCALE,
	WPLINE_SWAYFREQUENCY,
	WPLINE_BOBCROUCHMOD,
	WPLINE_BOBX,
	WPLINE_BOBY,
	WPLINE_BOBZ,
	WPLINE_SWAYX,
	WPLINE_SWAYY,
	WPLINE_SWAYZ,
	WPLINE_SCREENSHAKE,
	WPLINE_ALTSCREENSHAKE,
	WPLINE_ADSSCREENSHAKE,
	WPLINE_ALTADSSCREENSHAKE,
	WPLINE_AKIMBOSCREENSHAKE,
	WPLINE_CANROLL,
	WPLINE_CANJUMP,
	WPLINE_CANSPRINT,
	WPLINE_CANSHOOTWHILEROLLING,
	WPLINE_AKIMBO,
	WPLINE_ICONPATH,
	WPLINE_SWAPICONPATH,
	WPLINE_NUMBARRELS,
	WPLINE_XLERPORIGIN,
	WPLINE_YLERPORIGIN,
	WPLINE_ZLERPORIGIN,
	WPLINE_PITCHLERPANGLES,
	WPLINE_YAWLERPANGLES,
	WPLINE_ROLLLERPANGLES,
	WPLINE_XNUDGE,
	WPLINE_YNUDGE,
	WPLINE_ZNUDGE,
	WPLINE_ADSXNUDGE,
	WPLINE_ADSYNUDGE,
	WPLINE_ADSZNUDGE,
	WPLINE_AKIMBOXNUDGE,
	WPLINE_AKIMBOYNUDGE,
	WPLINE_AKIMBOZNUDGE,
	WPLINE_XBOXRUMBLEINTENSITY,
	WPLINE_XBOXRUMBLELENGTH,
	WPLINE_EOL
} wpLineParse_t;

#define NUM_LIMBS		8


typedef struct weaponData_s
{

	//Basic Data
	char weaponName[32]; //Not used.
	char internalName[32]; //Not used?
	short weaponID;
	short hcIdx;
	short variationIdx;

	//Ammo Data
	unsigned short ammoIndex;
	unsigned short ammoLow;
	unsigned short ammoClipLow;
	unsigned short startingAmmo;

	//Ammo Costs
	unsigned short energyPerShot;
	unsigned short altEnergyPerShot;

	//Damage and Combat
	short damage;
	short altDamage;
	short methodOfDeath;
	short altMethodOfDeath;
	float Mods[NUM_LIMBS];
	float AltMods[NUM_LIMBS];
	float recoil;
	float altRecoil;
	float akimboRecoil;
	float hRecoil;
	float hAltRecoil;
	float hAkimboRecoil;
	float speedModifier;
	float ADSspeedModifier;
	short ADSfov;
	unsigned int ADSTime;

	float zoomStartFOV;
	float zoomEndFOV;
	float zoomSpeed;
	unsigned short zoomType;

	qboolean fireFuncInvert;
	qboolean altFireFuncInvert;
	//SERVERSIDE ONLY
#ifdef QAGAME
#ifdef AKIMBO
	void (*fireFunc)(struct gentity_s *self, qboolean altFire, qboolean primWeapon);
	void (*altFireFunc)(struct gentity_s *self, qboolean altFire, qboolean primWeapon);
#else
	void (*fireFunc)(struct gentity_s *self, qboolean altFire);
	void (*altFireFunc)(struct gentity_s *self, qboolean altFire);
#endif
#else
	//void (*fireFunc)(void);
	//void (*altFireFunc)(void);
	//Parse these as a string literal instead
	char fireFunc[MAX_QPATH];
	char altFireFunc[MAX_QPATH];
#endif
	qboolean altFireScope;

	float spread;
	float altSpread;
	unsigned short numShots;
	unsigned short altNumShots;

	//userInt correlation
	unsigned short weaponType;

	//Firing Modes
	qboolean alwaysSemi;
	qboolean altAlwaysSemi;
	
	unsigned short numFireModes;
	short fireModes[MAX_FIRINGMODES];
	short altFireModes[MAX_FIRINGMODES];
	qboolean fireModeDisplay;

	//Information that changes based on firing mode
	unsigned int fireTime[MAX_FIRINGMODES];
	unsigned int altFireTime[MAX_FIRINGMODES];
	unsigned short accuracyDrain[MAX_FIRINGMODES];
	float accuracyThreshold; //Doesn't change based on firing mode, but I don't feel like setting this off on its own
	unsigned short fireModeDelays[MAX_FIRINGMODES]; //Delay between bursts

	//Reloading
	unsigned short clipSize;
	unsigned short reloadTime;
	unsigned short reloadType;
	qboolean autoReload;

	//For Sui ;)
	unsigned int bulletSpeed;
	unsigned int altBulletSpeed;

	//Clientside stuff
	char flashSound[MAX_QPATH];
	char altFlashSound[MAX_QPATH];
	char muzzleEffect[MAX_QPATH];
	char altMuzzleEffect[MAX_QPATH];

	char selectSound[MAX_QPATH];

	unsigned short weaponSwapL;
	unsigned short weaponSwapW;

	char missileModel[MAX_QPATH];
	unsigned short numBarrels;
	char modelFolder[MAX_QPATH];
	char md3Name[MAX_QPATH];
	char ghoul2Name[MAX_QPATH];
	char readyAnim[MAX_QPATH];
	char legsReadyAnim[MAX_QPATH];
	char attackAnim[MAX_QPATH];
	char akimboAttackAnim[MAX_QPATH];
	unsigned short attackAnimMS;
	unsigned short akimboAttackAnimMS;

	qboolean limbDismember;

	float bobbing[ROLL+1];
	float sway[ROLL+1];
	float swayTimeScale;
	int swayFrequency;
	float bobCrouchModifier;
	float bobbingOrigin[ROLL+1];
	float swayOrigin[ROLL+1];
	vec2_t screenShake;
	vec2_t altScreenShake;
	vec2_t ADSscreenShake;
	vec2_t ADSaltScreenShake;
	vec2_t akimboScreenShake;

	//Acrobatics
	qboolean canRoll;
	qboolean canJump;
	qboolean canSprint;
	qboolean canShootWhileRolling;
	qboolean akimbo;

	char iconPath[MAX_QPATH];
	char swapIconPath[MAX_QPATH];

	unsigned short fireFunctionIndex;
	unsigned short altFireFunctionIndex;

	vec3_t lerpOrigin;
	vec3_t lerpAngles;
	vec3_t nudge;
	vec3_t ADSnudge;
	vec3_t akimboNudge;

	unsigned short xboxRumbleIntensity;
	unsigned short xboxRumbleDuration;

} weaponData_t;


typedef struct  ammoData_s
{
//	char	icon[32];	// Name of ammo icon file
	int		max;		// Max amount player can hold of ammo
} ammoData_t;


extern weaponData_t weaponData[WP_NUM_WEAPONS];
extern weaponData_t copyArray[256];
extern ammoData_t ammoData[AMMO_MAX];


// Specific weapon information

#define FIRST_WEAPON		WP_BRYAR_PISTOL		// this is the first weapon for next and prev weapon switching
//[CoOp]
#define MAX_PLAYER_WEAPONS	WP_BRYAR_OLD		// this is the max you can switch to and get with the give all.
//#define MAX_PLAYER_WEAPONS	WP_NUM_WEAPONS-1	// this is the max you can switch to and get with the give all.
//[/CoOp]


#define DEFAULT_SHOTGUN_SPREAD	700
#define DEFAULT_SHOTGUN_COUNT	11

#define	LIGHTNING_RANGE		768

void GSA_LoadWeaponTXT(const char *directory, char *mapName, weaponData_t *weaponsArray);
void GSA_LoadAmmoTXT(const char *fileName, ammoData_t *ammoArray);

int PrimaryWeapons[WP_NUM_WEAPONS];
int SidearmWeapons[WP_NUM_WEAPONS];
int Explosives[WP_NUM_WEAPONS];

#ifdef CGAME_OR_UI
void GSA_BuildWeaponsArrays();
#endif
int ConvertToPARM(int mode, int weapon);


#endif//#ifndef __WEAPONS_H__
