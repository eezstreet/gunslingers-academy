// Copyright (C) 1999-2000 Id Software, Inc.
//
// cg_event.c -- handle entity events at snapshot or playerstate transitions

#include "cg_local.h"
#include "fx_local.h"
#include "../ui/ui_shared.h"
#include "../ui/ui_public.h"

// for the voice chats
//[SVN]
#include "../../ojpbasic/ui/jamp/menudef.h"
//#include "../../ui/menudef.h"
//[/SVN]

#include "../ghoul2/G2.h"
//==========================================================================

extern qboolean WP_SaberBladeUseSecondBladeStyle( saberInfo_t *saber, int bladeNum );
extern qboolean CG_VehicleWeaponImpact( centity_t *cent );
extern qboolean CG_InFighter( void );
extern qboolean CG_InATST( void );
extern int cg_saberFlashTime;
extern vec3_t cg_saberFlashPos;
extern char *showPowersName[];
extern void CG_EVBox_AddString(char *chatStr); //cg_draw.c
extern int cg_siegeDeathTime;
extern int cg_siegeDeathDelay;
extern int cg_vehicleAmmoWarning;
extern int cg_vehicleAmmoWarningTime;
void CG_TryPlayCustomSound( vec3_t origin, int entityNum, int channel, const char *soundName );

//I know, not siege, but...
typedef enum
{
	TAUNT_TAUNT = 0,
	TAUNT_BOW,
	TAUNT_MEDITATE,
	TAUNT_FLOURISH,
	TAUNT_GLOAT
};
/*
===================
CG_PlaceString

Also called by scoreboard drawing
===================
*/
const char	*CG_PlaceString( int rank ) {
	static char	str[64];
	char	*s, *t;
	// number extenstions, eg 1st, 2nd, 3rd, 4th etc.
	// note that the rules are different for french, but by changing the required strip strings they seem to work
	char sST[10];
	char sND[10];
	char sRD[10];
	char sTH[10];
	char sTiedFor[64];	// german is much longer, super safe...

	trap_SP_GetStringTextString("MP_INGAME_NUMBER_ST",sST, sizeof(sST) );
	trap_SP_GetStringTextString("MP_INGAME_NUMBER_ND",sND, sizeof(sND) );
	trap_SP_GetStringTextString("MP_INGAME_NUMBER_RD",sRD, sizeof(sRD) );
	trap_SP_GetStringTextString("MP_INGAME_NUMBER_TH",sTH, sizeof(sTH) );
	trap_SP_GetStringTextString("MP_INGAME_TIED_FOR" ,sTiedFor,sizeof(sTiedFor) );
	strcat(sTiedFor," ");	// save worrying about translators adding spaces or not

	if ( rank & RANK_TIED_FLAG ) {
		rank &= ~RANK_TIED_FLAG;
		t = sTiedFor;//"Tied for ";
	} else {
		t = "";
	}

	if ( rank == 1 ) {
		s = va("1%s",sST);//S_COLOR_BLUE "1st" S_COLOR_WHITE;		// draw in blue
	} else if ( rank == 2 ) {
		s = va("2%s",sND);//S_COLOR_RED "2nd" S_COLOR_WHITE;		// draw in red
	} else if ( rank == 3 ) {
		s = va("3%s",sRD);//S_COLOR_YELLOW "3rd" S_COLOR_WHITE;		// draw in yellow
	} else if ( rank == 11 ) {
		s = va("11%s",sTH);
	} else if ( rank == 12 ) {
		s = va("12%s",sTH);
	} else if ( rank == 13 ) {
		s = va("13%s",sTH);
	} else if ( rank % 10 == 1 ) {
		s = va("%i%s", rank,sST);
	} else if ( rank % 10 == 2 ) {
		s = va("%i%s", rank,sND);
	} else if ( rank % 10 == 3 ) {
		s = va("%i%s", rank,sRD);
	} else {
		s = va("%i%s", rank,sTH);
	}

	Com_sprintf( str, sizeof( str ), "%s%s", t, s );
	return str;
}

qboolean CG_ThereIsAMaster(void);

//[ImprovedObituary]
stringID_table_t NPCClasses[] =
{
	"None?",				CLASS_NONE, //This probably shouldn't happen.
	"AT-ST", 				CLASS_ATST,				
	"Bartender",			CLASS_BARTENDER,
	"Bespin Cop",			CLASS_BESPIN_COP,		
	"Claw",					CLASS_CLAW,
	"Commando",				CLASS_COMMANDO,
	"Desann",				CLASS_DESANN,			
	"Fish",					CLASS_FISH,
	"Fliers",				CLASS_FLIER2,
	"Galak",				CLASS_GALAK,
	"Glider",				CLASS_GLIDER,
	"Gonk Droid",			CLASS_GONK,				
	"Gran",					CLASS_GRAN,
	"Howler",				CLASS_HOWLER,
	"Imperial Officer",		CLASS_IMPERIAL,
	"Imperial Worker",		CLASS_IMPWORKER,
	"Interrogator Droid",	CLASS_INTERROGATOR,		// droid 
	"Jan",					CLASS_JAN,				
	"Jedi",					CLASS_JEDI,				
	"Kyle Katarn",			CLASS_KYLE,				
	"Lando Calrissian",		CLASS_LANDO,			
	"Lizard",				CLASS_LIZARD,
	"Luke Skywalker",		CLASS_LUKE,				
	"Mark 1 Droid",			CLASS_MARK1,			// droid
	"Mark 2 Droid",			CLASS_MARK2,			// droid
	"Galak's Mech Suit",	CLASS_GALAKMECH,		// droid
	"Mine Monster",			CLASS_MINEMONSTER,
	"Mon Mothma",			CLASS_MONMOTHA,			
	"Morgan Katarn",		CLASS_MORGANKATARN,
	"Mouse Droid",			CLASS_MOUSE,			// droid
	"Murjj",				CLASS_MURJJ,
	"Prisoner",				CLASS_PRISONER,
	"Probe Droid",			CLASS_PROBE,			// droid
	"Protocol Droid",		CLASS_PROTOCOL,			// droid
	"R2D2",					CLASS_R2D2,				// droid
	"R5D2",					CLASS_R5D2,				// droid
	"Rebel Soldier",		CLASS_REBEL,
	"Reborn",				CLASS_REBORN,
	"Reelo Baruk",			CLASS_REELO,
	"Droid Remote",			CLASS_REMOTE,
	"Rodian",				CLASS_RODIAN,
	"Seeker Droid",			CLASS_SEEKER,			// droid
	"Sentry",				CLASS_SENTRY,
	"Shadowtrooper",		CLASS_SHADOWTROOPER,
	"Stormtrooper",			CLASS_STORMTROOPER,
	"Swamp?",				CLASS_SWAMP,
	"Swamptrooper",			CLASS_SWAMPTROOPER,
	"Tavion",				CLASS_TAVION,
	"Trandoshan",			CLASS_TRANDOSHAN,
	"Ugnaught",				CLASS_UGNAUGHT,
	"Jawa",					CLASS_JAWA,
	"Weequay",				CLASS_WEEQUAY,
	"Boba Fett",			CLASS_BOBAFETT,
	"Vehicle",				CLASS_VEHICLE,
	"Fluffy the Rancor",	CLASS_RANCOR,
	"Frosty the Wampa",		CLASS_WAMPA,
	"Saboteur", 			CLASS_SABOTEUR,
	"Sand Creature", 		CLASS_SAND_CREATURE,
	"Alora", 				CLASS_ALORA,
	"Rockettrooper", 		CLASS_ROCKETTROOPER,
	"Saber Droid",			CLASS_SABER_DROID,
	"Assassin Droid", 		CLASS_ASSASSIN_DROID,
	"Hazard Trooper",		CLASS_HAZARD_TROOPER,
	"Player's Standin", 	CLASS_PLAYER,
	"Tusken Raider", 		CLASS_TUSKEN,
	"",						-1,	//CLASS_NUM_CLASSES
};
//[/ImprovedObituary]

/*
=============
CG_Obituary
=============
*/
extern void Xbox_Rumble(short intensity, unsigned int msec);
extern vmCvar_t	xbox_power;
static void CG_Obituary( entityState_t *ent ) {
	int			mod;
	int			target, attacker;
	char		*message;
//	char		*chatmessage;
	const char	*targetInfo;
	const char	*attackerInfo;
	char		targetName[32];
	//[Asteroids]
	char		targetVehName[32] = {0};
	//[/Asteroids]
	char		attackerName[32];
	//[Asteroids]
	char		attackerVehName[32] = {0};
	char		attackerVehWeapName[32] = {0};
	//[/Asteroids]
	//[TraitorKills]
	centity_t	*killercent;
	centity_t	*targetcent;
	//[/TraitorKills]
	gender_t	gender;
	clientInfo_t	*ci;
	//[Asteroids]
	qboolean	vehMessage = qfalse;
	//[/Asteroids]

	target = ent->otherEntityNum;
	attacker = ent->otherEntityNum2;
	mod = ent->eventParm;

	killercent = &cg_entities[attacker];
	targetcent = &cg_entities[target];

	if ( target < 0 || target >= MAX_CLIENTS ) {
		CG_Error( "CG_Obituary: target out of range" );
	}
	ci = &cgs.clientinfo[target];


	if ( attacker < 0 || attacker >= MAX_CLIENTS ) {
		//[Asteroids]
		//attacker = ENTITYNUM_WORLD;
		//[/Asteroids]
		attackerInfo = NULL;
	} else {
		attackerInfo = CG_ConfigString( CS_PLAYERS + attacker );
	}

	targetInfo = CG_ConfigString( CS_PLAYERS + target );
	if ( !targetInfo ) {
		return;
	}
	Q_strncpyz( targetName, Info_ValueForKey( targetInfo, "n" ), sizeof(targetName) - 2);
	strcat( targetName, S_COLOR_WHITE );

	//[Asteroids]
	// check for target in a vehicle
	if ( ent->lookTarget > VEHICLE_BASE && ent->lookTarget < MAX_VEHICLES && g_vehicleInfo[ent->lookTarget].name )
	{
		Q_strncpyz( targetVehName, g_vehicleInfo[ent->lookTarget].name, sizeof(targetVehName) - 2 );
	}

	// check for attacker in a vehicle
	if ( ent->brokenLimbs >= MAX_CLIENTS )
	{
		centity_t *attVehCent = &cg_entities[ent->brokenLimbs];
		if ( attVehCent && attVehCent->m_pVehicle && attVehCent->m_pVehicle->m_pVehicleInfo )
		{
			if ( attVehCent->m_pVehicle->m_pVehicleInfo->name )
			{
				Q_strncpyz( attackerVehName, attVehCent->m_pVehicle->m_pVehicleInfo->name, sizeof(attackerVehName) - 2 );
			}
		}
	}

	//check for specific vehicle weapon
	if ( ent->weapon > 0 )
	{
		if ( g_vehWeaponInfo[ent->weapon-1].name )
		{
			Q_strncpyz( attackerVehWeapName, g_vehWeaponInfo[ent->weapon-1].name, sizeof(attackerVehWeapName) - 2 );
		}
	}
	//[/Asteroids]

	// check for single client messages
	//[Asteroids]
	if ( ent->saberInFlight )
	{//asteroid->vehicle collision
		switch ( Q_irand( 0, 2 ) )
		{
		default:
		case 0:
            message = "DIED_ASTEROID1";
			break;
		case 1:
            message = "DIED_ASTEROID2";
			break;
		case 2:
            message = "DIED_ASTEROID3";
			break;
		}
		vehMessage = qtrue;
	}
	else
	{
		switch( mod ) {
		case MOD_VEHICLE:
		//case MOD_SUICIDE:
		case MOD_FALLING:
		//case MOD_COLLISION:
		//case MOD_VEH_EXPLOSION:
		case MOD_CRUSH:
		//case MOD_WATER:
		//case MOD_SLIME:
		//case MOD_LAVA:
		case MOD_TRIGGER_HURT:
			message = "DIED_GENERIC";
			break;
		case MOD_TARGET_LASER:
			vehMessage = qtrue;
			message = "DIED_TURBOLASER";
			break;
		default:
			message = NULL;
			break;
		}
	//[/Asteroids]
	}
#ifdef ACHIEVEMENTS
	if(target == cg.clientNum && mod == MOD_FALLING)
	{
		GSA_IncrementAchievement(&cgs.achievements[A_FALLDEATH], 1);
	}
#endif
	// Rumble the dead
	if(xbox_power.integer && cg.predictedPlayerState.clientNum == target)
	{
		Xbox_Rumble(60000, 500);
	}
	// Attacker killed themselves.  Ridicule them for it.
	if (attacker == target) 
	{
		//[Asteroids]
		vehMessage = qfalse;
		//[/Asteroids]
		gender = ci->gender;
		/*switch (mod) 
		{
		case MOD_BRYAR_PISTOL:
		case MOD_BRYAR_PISTOL_ALT:
		case MOD_BRYAR_PISTOL_HEAD:
		case MOD_BLASTER:
		case MOD_BLASTER_HEAD:
		case MOD_TURBLAST:
		case MOD_DISRUPTOR:
		case MOD_DISRUPTOR_SPLASH:
		case MOD_DISRUPTOR_SNIPER:
		case MOD_DISRUPTOR_HEAD:
		case MOD_BOWCASTER:
		case MOD_BOWCASTER_HEAD:
		case MOD_REPEATER:
		case MOD_REPEATER_ALT:
		case MOD_REPEATER_HEAD:
		case MOD_FLECHETTE:
		case MOD_FLECHETTE_HEAD:
			if ( gender == GENDER_FEMALE )
				message = "SUICIDE_SHOT_FEMALE";
			else if ( gender == GENDER_NEUTER )
				message = "SUICIDE_SHOT_GENDERLESS";
			else
				message = "SUICIDE_SHOT_MALE";
			break;
		case MOD_REPEATER_ALT_SPLASH:
		case MOD_FLECHETTE_ALT_SPLASH:
		case MOD_ROCKET:
		case MOD_ROCKET_SPLASH:
		case MOD_ROCKET_HOMING:
		case MOD_ROCKET_HOMING_SPLASH:
		case MOD_THERMAL:
		case MOD_THERMAL_SPLASH:
		case MOD_TRIP_MINE_SPLASH:
		case MOD_TIMED_MINE_SPLASH:
		case MOD_DET_PACK_SPLASH:
		case MOD_VEHICLE:
		case MOD_CONC:
		case MOD_CONC_ALT:
			if ( gender == GENDER_FEMALE ){
				message = "SUICIDE_EXPLOSIVES_FEMALE";
				chatmessage = va("%s blew herself up", targetName);
			}
			else if ( gender == GENDER_NEUTER ){
				message = "SUICIDE_EXPLOSIVES_GENDERLESS";
				chatmessage = va("%s blew itself up", targetName);
			}
			else{
				message = "SUICIDE_EXPLOSIVES_MALE";
				chatmessage = va("%s blew himself up", targetName);
			}
			break;
		case MOD_DEMP2:
			if ( gender == GENDER_FEMALE )
				message = "SUICIDE_ELECTROCUTED_FEMALE";
			else if ( gender == GENDER_NEUTER )
				message = "SUICIDE_ELECTROCUTED_GENDERLESS";
			else
				message = "SUICIDE_ELECTROCUTED_MALE";
			break;
		case MOD_FALLING:
			if ( gender == GENDER_FEMALE ){
				message = "SUICIDE_FALLDEATH_FEMALE";
				chatmessage = va("%s fell to her death.", targetName);
			}
			else if ( gender == GENDER_NEUTER ){
				message = "SUICIDE_FALLDEATH_GENDERLESS";
				chatmessage = va("%s fell to it's death.", targetName);
			}
			else{
				message = "SUICIDE_FALLDEATH_MALE";
				chatmessage = va("%s fell to his death.", targetName);
			}
			break;
		default:
			if ( gender == GENDER_FEMALE ){
				message = "SUICIDE_GENERICDEATH_FEMALE";
				chatmessage = va("%s died.", targetName);
			}
			else if ( gender == GENDER_NEUTER ){
				message = "SUICIDE_GENERICDEATH_GENDERLESS";
				chatmessage = va("%s died.", targetName);
			}
			else{
				message = "SUICIDE_GENERICDEATH_MALE";
				chatmessage = va("%s died.", targetName);
			}
			break;
		}*/
		switch((int)gender)
		{
			case GENDER_NEUTER:
				message = modData[mod].neuterSuicideString;
				break;
			case GENDER_FEMALE:
				message = modData[mod].femaleSuicideString;
				break;
			default:
				message = modData[mod].suicideString;
				break;
		}
	}

	if (target != attacker && target < MAX_CLIENTS && attacker < MAX_CLIENTS)
	{
		goto clientkilled;
	}

	if (message) 
	{
		gender = ci->gender;

		if (!message[0])
		{
			//[Asteroids]
			vehMessage = qfalse;
			//[/Asteroids]
			if ( gender == GENDER_FEMALE )
				message = "SUICIDE_GENERICDEATH_FEMALE";
			else if ( gender == GENDER_NEUTER )
				message = "SUICIDE_GENERICDEATH_GENDERLESS";
			else
				message = "SUICIDE_GENERICDEATH_MALE";
		}
		//[Asteroids]
		if ( vehMessage )
		{
			message = (char *)CG_GetStringEdString("OJP_INGAMEVEH", message);
		}
		else
		{
			message = (char *)CG_GetStringEdString("MP_INGAME", message);
		}
		//[/Asteroids]

		CG_Printf( "%s %s\n", targetName, message);
		return;
	}

clientkilled:

	// check for kill messages from the current clientNum
	if ( attacker == cg.snap->ps.clientNum ) {
		char	*s;

		if ( cgs.gametype < GT_TEAM && cgs.gametype != GT_DUEL && cgs.gametype != GT_POWERDUEL ) {
			if (cgs.gametype == GT_JEDIMASTER &&
				attacker < MAX_CLIENTS &&
				!ent->isJediMaster &&
				!cg.snap->ps.isJediMaster &&
				CG_ThereIsAMaster())
			{
				char part1[512];
				char part2[512];
				trap_SP_GetStringTextString("MP_INGAME_KILLED_MESSAGE", part1, sizeof(part1));
				trap_SP_GetStringTextString("MP_INGAME_JMKILLED_NOTJM", part2, sizeof(part2));
				s = va("%s %s\n%s\n", part1, targetName, part2);
			}
			else if (cgs.gametype == GT_JEDIMASTER &&
				attacker < MAX_CLIENTS &&
				!ent->isJediMaster &&
				!cg.snap->ps.isJediMaster)
			{ //no JM, saber must be out
				char part1[512];
				trap_SP_GetStringTextString("MP_INGAME_KILLED_MESSAGE", part1, sizeof(part1));
				/*
				kmsg1 = "for 0 points.\nGo for the saber!";
				strcpy(part2, kmsg1);

				s = va("%s %s %s\n", part1, targetName, part2);
				*/
				s = va("%s %s\n", part1, targetName);
			}
			else if (cgs.gametype == GT_POWERDUEL)
			{
				s = "";
			}
			else
			{
				char sPlaceWith[256];
				char sKilledStr[256];
				trap_SP_GetStringTextString("MP_INGAME_PLACE_WITH",     sPlaceWith, sizeof(sPlaceWith));
				trap_SP_GetStringTextString("MP_INGAME_KILLED_MESSAGE", sKilledStr, sizeof(sKilledStr));

				s = va("%s %s.\n%s %s %i.", sKilledStr, targetName, 
					CG_PlaceString( cg.snap->ps.persistant[PERS_RANK] + 1 ), 
					sPlaceWith,
					cg.snap->ps.persistant[PERS_SCORE] );
			}
		} else {
			char sKilledStr[256];
			trap_SP_GetStringTextString("MP_INGAME_KILLED_MESSAGE", sKilledStr, sizeof(sKilledStr));
			s = va("%s %s", sKilledStr, targetName );
		}
		//if (!(cg_singlePlayerActive.integer && cg_cameraOrbit.integer)) {
			CG_CenterPrint( s, SCREEN_HEIGHT * 0.30, BIGCHAR_WIDTH );
		//} 
		// print the text message as well
	}

	// check for double client messages
	if ( !attackerInfo ) {
		//[Asteroids]
		//attacker = ENTITYNUM_WORLD;
		//[/Asteroids]

		//[ImprovedObituary]
		strcpy(attackerName, GetStringForID( NPCClasses, cg_entities[attacker].currentState.NPC_class ));
		//strcpy( attackerName, "noname" );		
		//[/ImprovedObituary]
	} else {
		Q_strncpyz( attackerName, Info_ValueForKey( attackerInfo, "n" ), sizeof(attackerName) - 2);
		strcat( attackerName, S_COLOR_WHITE );
		// check for kill messages about the current clientNum
		if ( target == cg.snap->ps.clientNum ) {
			Q_strncpyz( cg.killerName, attackerName, sizeof( cg.killerName ) );
		}
	}

	if ( attacker != ENTITYNUM_WORLD ) {
		/*switch (mod) {
		case MOD_STUN_BATON:
			message = "KILLED_STUN";
			break;
		case MOD_MELEE:
			message = "KILLED_MELEE";
			break;
		case MOD_SABER:
			message = "KILLED_SABER";
			break;
		case MOD_BRYAR_PISTOL:
		case MOD_BRYAR_PISTOL_ALT:
		case MOD_BRYAR_PISTOL_HEAD:
			message = "KILLED_BRYAR";
			break;
		case MOD_BLASTER:
		case MOD_BLASTER_HEAD:
			message = "KILLED_BLASTER";
			break;
		case MOD_TURBLAST:
			message = "KILLED_BLASTER";
			break;
		case MOD_DISRUPTOR:
		case MOD_DISRUPTOR_SPLASH:
			message = "KILLED_DISRUPTOR";
			break;
		case MOD_DISRUPTOR_SNIPER:
		case MOD_DISRUPTOR_HEAD:
			message = "KILLED_DISRUPTORSNIPE";
			break;
		case MOD_BOWCASTER:
		case MOD_BOWCASTER_HEAD:
			message = "KILLED_BOWCASTER";
			break;
		case MOD_REPEATER:
		case MOD_REPEATER_HEAD:
			message = "KILLED_REPEATER";
			break;
		case MOD_REPEATER_ALT:
		case MOD_REPEATER_ALT_SPLASH:
			message = "KILLED_REPEATERALT";
			break;
		case MOD_DEMP2:
		case MOD_DEMP2_ALT:
		case MOD_DEMP2_HEAD:
			message = "KILLED_DEMP2";
			break;
		case MOD_FLECHETTE:
		case MOD_FLECHETTE_HEAD:
			message = "KILLED_FLECHETTE";
			break;
		case MOD_FLECHETTE_ALT_SPLASH:
			message = "KILLED_FLECHETTE_MINE";
			break;
		case MOD_ROCKET:
		case MOD_ROCKET_SPLASH:
			message = "KILLED_ROCKET";
			break;
		case MOD_ROCKET_HOMING:
		case MOD_ROCKET_HOMING_SPLASH:
			message = "KILLED_ROCKET_HOMING";
			break;
		case MOD_THERMAL:
		case MOD_THERMAL_SPLASH:
			message = "KILLED_THERMAL";
			break;
		case MOD_TRIP_MINE_SPLASH:
			message = "KILLED_TRIPMINE";
			break;
		case MOD_TIMED_MINE_SPLASH:
			message = "KILLED_TRIPMINE_TIMED";
			break;
		case MOD_DET_PACK_SPLASH:
			message = "KILLED_DETPACK";
			break;
		case MOD_VEHICLE:
			//[Asteroids]
			vehMessage = qtrue;
			switch ( ent->generic1 )
			{
			case WP_BLASTER://primary blasters
				switch ( Q_irand( 0, 2 ) )
				{
				case 2:
					message = "KILLED_VEH_BLASTER3";
					break;
				case 1:
					message = "KILLED_VEH_BLASTER2";
					break;
				default:
					message = "KILLED_VEH_BLASTER1";
					break;
				}
				break;
			case WP_ROCKET_LAUNCHER://missile
				if ( Q_irand( 0, 1 ) )
				{
					message = "KILLED_VEH_MISSILE2";
				}
				else
				{
					message = "KILLED_VEH_MISSILE1";
				}
				break;
			case WP_THERMAL://bomb
				message = "KILLED_VEH_BOMB";
				break;
			case WP_DEMP2://ion cannon
				message = "KILLED_VEH_ION";
				break;
			case WP_TURRET://turret
				message = "KILLED_VEH_TURRET";
				break;
			default:
				vehMessage = qfalse;
				message = "KILLED_GENERIC";
				break;
			}
			break;
			//[/Asteroids]
		case MOD_CONC:
		case MOD_CONC_ALT:
			message = "KILLED_GENERIC";
			break;
		case MOD_FORCE_DARK:
			message = "KILLED_DARKFORCE";
			break;
		case MOD_SENTRY:
			message = "KILLED_SENTRY";
			break;
		case MOD_TELEFRAG:
			message = "KILLED_TELEFRAG";
			break;
		case MOD_CRUSH:
			message = "KILLED_GENERIC";//"KILLED_FORCETOSS";
			break;
		case MOD_FALLING:
			message = "KILLED_FORCETOSS";
			chatmessage = va("%s threw %s to their doom!", attackerName, targetName);
			break;
		//[Asteroids]
		case MOD_COLLISION:
		case MOD_VEH_EXPLOSION:
			switch ( Q_irand( 0, 2 ) )
			{
			default:
			case 0:
				message = "KILLED_VEH_COLLISION1";
				break;
			case 1:
				message = "KILLED_VEH_COLLISION2";
				break;
			case 2:
				message = "KILLED_VEH_COLLISION3";
				break;
			}
			vehMessage = qtrue;
			break;
		//[/Asteroids]
		case MOD_TRIGGER_HURT:
			message = "KILLED_GENERIC";//"KILLED_FORCETOSS";
			break;
		//[Asteroids]
		case MOD_TARGET_LASER:
			if ( Q_irand(0,1) )
			{
				message = "KILLED_TURRET1";
			}
			else
			{
				message = "KILLED_TURRET2";
			}
			vehMessage = qtrue;
			break;
		//[/Asteroids]
		default:
			message = "KILLED_GENERIC";
			break;
		}*/
		message = modData[mod].deathMessage;

		if((attacker < MAX_CLIENTS && target < MAX_CLIENTS) && cgs.clientinfo[attacker].team == cgs.clientinfo[target].team && cgs.gametype >= GT_TEAM)
		{
			message = va("KILLED_TEAMKILL%i", Q_irand(1, 3));
		}

		//[Asteroids]	
		if (message) 
		{
			if ( vehMessage )
			{
				message = (char *)CG_GetStringEdString("OJP_INGAMEVEH", message);
			}
			else
			{
				message = (char *)CG_GetStringEdString("MP_INGAME", message);
			}

			CG_Printf( "%s ", targetName);
			if ( targetVehName[0] )
			{
				CG_Printf( "(%s) ", targetVehName);
			}
			if ( mod == MOD_TARGET_LASER )
			{//no attacker name, just a turbolaser or other kind of turret...
				CG_Printf( "%s", message);
			}
			else
			{
				//if(mod == MOD_BRYAR_PISTOL_HEAD || mod == MOD_BLASTER_HEAD || mod == MOD_DISRUPTOR_HEAD || mod == MOD_BOWCASTER_HEAD || mod == MOD_REPEATER_HEAD || mod == MOD_FLECHETTE_HEAD || mod == MOD_DEMP2_HEAD ){
				if(modData[mod].headShot > 0) {
					CG_Printf( "%s %s ^7<^1Head Shot^7>", message, attackerName);
					if(cg_obitInChat.integer == 1)
						CG_ChatBox_AddString(va("%s %s %s ^7<^1Head Shot^7>", targetName, message, attackerName));
				}
				else{
					CG_Printf( "%s %s", message, attackerName);
					if(cg_obitInChat.integer == 1)
						CG_ChatBox_AddString(va("%s %s %s", targetName, message, attackerName));
				}

				if ( attackerVehName[0]
					&& attackerVehWeapName[0] )
				{
					CG_Printf( " (%s %s)", attackerVehName, attackerVehWeapName );
				}
				else
				{
					if ( attackerVehName[0] )
					{
						CG_Printf( " (%s)", attackerVehName );
					}
					else if ( attackerVehWeapName[0] )
					{
						CG_Printf(" (%s)", attackerVehWeapName );
					}
				}
			}
			CG_Printf( "\n" );
		//[/Asteroids]
			return;
		}
	}

	// we don't know what it was
	CG_Printf( "%s %s\n", targetName, (char *)CG_GetStringEdString("MP_INGAME", "DIED_GENERIC") );
}

//==========================================================================

void CG_ToggleBinoculars(centity_t *cent, int forceZoom)
{
	if (cent->currentState.number != cg.snap->ps.clientNum)
	{
		return;
	}

	if (cg.snap->ps.weaponstate != WEAPON_READY)
	{ //So we can't fool it and reactivate while switching to the saber or something.
		return;
	}

	/*
	if (cg.snap->ps.weapon == WP_SABER)
	{ //No.
		return;
	}
	*/

	if (forceZoom)
	{
		if (forceZoom == 2)
		{
			cg.snap->ps.zoomMode = 0;
		}
		else if (forceZoom == 1)
		{
			cg.snap->ps.zoomMode = 2;
		}
	}

	if (cg.snap->ps.zoomMode == 0)
	{
		cg.zoomSoundPlayed = qfalse;
		cg.startZoomTime = cg.time + weaponData[cg.snap->ps.weapon].ADSTime;
	}
	else if (cg.snap->ps.zoomMode == 2)
	{
		trap_S_StartSound( NULL, cg.snap->ps.clientNum, CHAN_AUTO, cgs.media.zoomEnd );
	}
}

//set the local timing bar
extern int cg_genericTimerBar;
extern int cg_genericTimerDur;
extern vec4_t cg_genericTimerColor;
void CG_LocalTimingBar(int startTime, int duration)
{
    cg_genericTimerBar = startTime + duration;
	cg_genericTimerDur = duration;

	cg_genericTimerColor[0] = 1.0f;
	cg_genericTimerColor[1] = 1.0f;
	cg_genericTimerColor[2] = 0.0f;
	cg_genericTimerColor[3] = 1.0f;
}

/*
===============
CG_UseItem
===============
*/
static void CG_UseItem( centity_t *cent ) {
	clientInfo_t *ci;
	int			itemNum, clientNum;
	gitem_t		*item;
	entityState_t *es;

	es = &cent->currentState;
	
	itemNum = (es->event & ~EV_EVENT_BITS) - EV_USE_ITEM0;
	if ( itemNum < 0 || itemNum > HI_NUM_HOLDABLE ) {
		itemNum = 0;
	}

	// print a message if the local player
	if ( es->number == cg.snap->ps.clientNum ) {
		if ( !itemNum ) {
			//CG_CenterPrint( "No item to use", SCREEN_HEIGHT * 0.30, BIGCHAR_WIDTH );
		} else {
			item = BG_FindItemForHoldable( itemNum );
		}
	}

	switch ( itemNum ) {
	default:
	case HI_NONE:
		//trap_S_StartSound (NULL, es->number, CHAN_BODY, cgs.media.useNothingSound );
		break;

	case HI_BINOCULARS:
		if(rewardVariationForReward(HI_BINOCULARS) == 5)
			CG_ToggleBinoculars(cent, es->eventParm);
		break;

	case HI_SEEKER:
		trap_S_StartSound (NULL, es->number, CHAN_AUTO, cgs.media.deploySeeker );
		break;

	case HI_SHIELD:
	case HI_SENTRY_GUN:
		break;

//	case HI_MEDKIT:
	case HI_MEDPAC:
	case HI_MEDPAC_BIG:
		clientNum = cent->currentState.clientNum;
		if ( clientNum >= 0 && clientNum < MAX_CLIENTS ) {
			ci = &cgs.clientinfo[ clientNum ];
			ci->medkitUsageTime = cg.time;
		}
		//Different sound for big bacta?
		trap_S_StartSound (NULL, es->number, CHAN_AUTO, cgs.media.medkitSound );
		break;
	case HI_JETPACK:
		break; //Do something?
	case HI_HEALTHDISP:
		//CG_LocalTimingBar(cg.time, TOSS_DEBOUNCE_TIME);
		break;
	case HI_AMMODISP:
		//CG_LocalTimingBar(cg.time, TOSS_DEBOUNCE_TIME);
		break;
	case HI_EWEB:
		break;
	case HI_CLOAK:
		break; //Do something?
	case HI_AIRSTRIKE:
		break;
	}

	if (cg.snap && cg.snap->ps.clientNum == cent->currentState.number &&
		itemNum != HI_JETPACK && itemNum != HI_HEALTHDISP && itemNum != HI_AMMODISP && itemNum != HI_CLOAK && itemNum != HI_EWEB)
	{ //if not using binoculars/jetpack/dispensers/cloak, we just used that item up, so switch
		BG_CycleInven(&cg.snap->ps, 1);
		cg.itemSelect = -1; //update the client-side selection display
	}
}


/*
================
CG_ItemPickup

A new item was picked up this frame
================
*/
static void CG_WeaponItemPickup( int itemNum, qboolean ammo )
{
		cg.itemPickup = itemNum;
	cg.itemPickupTime = cg.time;
	cg.itemPickupBlendTime = cg.time;
	// see if it should be the grabbed weapon
	if(!ammo)
		cg.weaponSelect = bg_itemlist[itemNum].giTag;

	//rww - print pickup messages
	if (bg_itemlist[itemNum].classname && bg_itemlist[itemNum].classname[0] &&
		(bg_itemlist[itemNum].giType != IT_TEAM || (bg_itemlist[itemNum].giTag != PW_REDFLAG && bg_itemlist[itemNum].giTag != PW_BLUEFLAG)) )
	{ //don't print messages for flags, they have their own pickup event broadcasts
		char	text[1024];
		char	upperKey[1024];

		strcpy(upperKey, bg_itemlist[itemNum].classname);

		if ( trap_SP_GetStringTextString( va("SP_INGAME_%s",Q_strupr(upperKey)), text, sizeof( text )))
		{
			Com_Printf("%s %s\n", CG_GetStringEdString("MP_INGAME", "PICKUPLINE"), bg_itemlist[itemNum].pickup_name);
			CG_EVBox_AddString(va("Obtained %s", bg_itemlist[itemNum].pickup_name));

		}
		else
		{
			Com_Printf("%s %s\n", CG_GetStringEdString("MP_INGAME", "PICKUPLINE"), bg_itemlist[itemNum].classname);
			//CG_EVBox_AddString(va("%s %s\n", CG_GetStringEdString("MP_INGAME", "PICKUPLINE"), bg_itemlist[itemNum].classname));
			CG_EVBox_AddString(va("%s %s\n", CG_GetStringEdString("MP_INGAME", "PICKUPLINE"), bg_itemlist[itemNum].pickup_name));
		}
	}
}
static void CG_ItemPickup( int itemNum, qboolean weaponAmmo ) {
	cg.itemPickup = itemNum;
	cg.itemPickupTime = cg.time;
	cg.itemPickupBlendTime = cg.time;
	// see if it should be the grabbed weapon
	if ( cg.snap && bg_itemlist[itemNum].giType == IT_WEAPON && !weaponAmmo ) {
		cg.weaponSelect = bg_itemlist[itemNum].giTag;
	}

	//rww - print pickup messages
	if (bg_itemlist[itemNum].classname && bg_itemlist[itemNum].classname[0] &&
		(bg_itemlist[itemNum].giType != IT_TEAM || (bg_itemlist[itemNum].giTag != PW_REDFLAG && bg_itemlist[itemNum].giTag != PW_BLUEFLAG)) )
	{ //don't print messages for flags, they have their own pickup event broadcasts
		char	text[1024];
		char	upperKey[1024];

		strcpy(upperKey, bg_itemlist[itemNum].classname);

		if ( trap_SP_GetStringTextString( va("SP_INGAME_%s",Q_strupr(upperKey)), text, sizeof( text )))
		{
			Com_Printf("%s %s\n", CG_GetStringEdString("MP_INGAME", "PICKUPLINE"), text);
			CG_EVBox_AddString(va("Obtained %s", bg_itemlist[itemNum].pickup_name));

		}
		else
		{
			Com_Printf("%s %s\n", CG_GetStringEdString("MP_INGAME", "PICKUPLINE"), bg_itemlist[itemNum].classname);
			//CG_EVBox_AddString(va("%s %s\n", CG_GetStringEdString("MP_INGAME", "PICKUPLINE"), bg_itemlist[itemNum].classname));
			CG_EVBox_AddString(va("%s %s\n", CG_GetStringEdString("MP_INGAME", "PICKUPLINE"), bg_itemlist[itemNum].pickup_name));
		}
	}
}


/*
================
CG_PainEvent

Also called by playerstate transition
================
*/
void CG_PainEvent( centity_t *cent, int mod ) {
//	char	*snd;
	//int		cheatWank = Q_irand(1,4);
	//int		chokeWank = Q_irand(1,3);

	// don't do more than two pain sounds a second
	if ( cg.time - cent->pe.painTime < 500 ) {
		return;
	}

	//Xbox controller rumble
	if(xbox_power.integer && cg.predictedPlayerState.clientNum == cent->currentState.clientNum)
	{
		Xbox_Rumble(60000, 100);
	}

	/*if ( cheatWank == 1 ) {
		snd = "*pain25.wav";
	} else if ( cheatWank == 2 ) {
		snd = "*pain50.wav";
	} else if ( cheatWank == 3 ) {
		snd = "*pain75.wav";
	} else {
		snd = "*pain100.wav";
	}*/
	if(modData[mod].useChokeSound)
	{
		if(cent->currentState.clientNum > MAX_CLIENTS || !cgs.clientinfo[cent->currentState.clientNum].infoValid)
		{
			CG_TryPlayCustomSound( NULL, cent->currentState.clientNum, CHAN_VOICE, va("*choke%i.wav", Q_irand(1,3)) );
		}
		else
		{
		//snd = va("*choke%i.wav", chokeWank);
			PlayClientSound(&cgs.clientinfo[cent->currentState.clientNum], CST_CHOKE, cent->currentState.clientNum, NULL);
		}

	}
	else
	{
		if(cent->currentState.clientNum > MAX_CLIENTS || !cgs.clientinfo[cent->currentState.clientNum].infoValid)
		{
			int i = Q_irand(1,4);
			i = i*25;
			CG_TryPlayCustomSound( NULL, cent->currentState.clientNum, CHAN_VOICE, va("*pain%i.wav", i) );
		}
		else
		{
			PlayClientSound(&cgs.clientinfo[cent->currentState.clientNum], CST_PAIN, cent->currentState.clientNum, NULL);
		}
	}
	/*trap_S_StartSound( NULL, cent->currentState.number, CHAN_VOICE, 
		CG_CustomSound( cent->currentState.number, snd ) );*/

	// save pain time for programitic twitch animation
	cent->pe.painTime = cg.time;
	cent->pe.painDirection	^= 1;
}

extern qboolean BG_GetRootSurfNameWithVariant( void *ghoul2, const char *rootSurfName, char *returnSurfName, int returnSize );
void CG_ReattachLimb(centity_t *source)
{
	clientInfo_t *ci = NULL;
	
	if ( source->currentState.number >= MAX_CLIENTS )
	{
		ci = source->npcClient;
	}
	else
	{
		ci = &cgs.clientinfo[source->currentState.number];
	}
	if ( ci )
	{//re-apply the skin
		if ( ci->torsoSkin > 0 )
		{
			trap_G2API_SetSkin(source->ghoul2,0,ci->torsoSkin,ci->torsoSkin);
		}
	}
		
	/*
	char *limbName;
	char *stubCapName;
	int i = G2_MODELPART_HEAD;

	//rww NOTE: Assumes G2_MODELPART_HEAD is first and G2_MODELPART_RLEG is last
	while (i <= G2_MODELPART_RLEG)
	{
		if (source->torsoBolt & (1 << (i-10)))
		{
			switch (i)
			{
			case G2_MODELPART_HEAD:
				limbName = "head";
				stubCapName = "torso_cap_head";
				break;
			case G2_MODELPART_WAIST:
				limbName = "torso";
				stubCapName = "hips_cap_torso";
				break;
			case G2_MODELPART_LARM:
				limbName = "l_arm";
				stubCapName = "torso_cap_l_arm";
				break;
			case G2_MODELPART_RARM:
				limbName = "r_arm";
				stubCapName = "torso_cap_r_arm";
				break;
			case G2_MODELPART_RHAND:
				limbName = "r_hand";
				stubCapName = "r_arm_cap_r_hand";
				break;
			case G2_MODELPART_LLEG:
				limbName = "l_leg";
				stubCapName = "hips_cap_l_leg";
				break;
			case G2_MODELPART_RLEG:
				limbName = "r_leg";
				stubCapName = "hips_cap_r_leg";
				break;
			default:
				source->torsoBolt = 0;
				source->ghoul2weapon = NULL;
				return;
			}

			trap_G2API_SetSurfaceOnOff(source->ghoul2, limbName, 0);
			trap_G2API_SetSurfaceOnOff(source->ghoul2, stubCapName, 0x00000100);
		}
		i++;
	}
	*/
	source->torsoBolt = 0;

	source->ghoul2weapon = NULL;
#ifdef AKIMBO
	source->ghoul2weapon2 = NULL;
#endif
}

const char *CG_TeamName(int team)
{
	if (team==TEAM_RED)
		return "RED";
	else if (team==TEAM_BLUE)
		return "BLUE";
	else if (team==TEAM_SPECTATOR)
		return "SPECTATOR";
	return "FREE";
}

void CG_PrintCTFMessage(clientInfo_t *ci, const char *teamName, int ctfMessage)
{
	char printMsg[1024];
	char *refName = NULL;
	const char *psStringEDString = NULL;

	switch (ctfMessage)
	{
	case CTFMESSAGE_FRAGGED_FLAG_CARRIER:
		refName = "FRAGGED_FLAG_CARRIER";
		break;
	case CTFMESSAGE_FLAG_RETURNED:
		refName = "FLAG_RETURNED";
		break;
	case CTFMESSAGE_PLAYER_RETURNED_FLAG:
		refName = "PLAYER_RETURNED_FLAG";
		break;
	case CTFMESSAGE_PLAYER_CAPTURED_FLAG:
		refName = "PLAYER_CAPTURED_FLAG";
		break;
	case CTFMESSAGE_PLAYER_GOT_FLAG:
		refName = "PLAYER_GOT_FLAG";
		break;
	default:
		return;
	}

	psStringEDString = CG_GetStringEdString("MP_INGAME", refName);

	if (!psStringEDString || !psStringEDString[0])
	{
		return;
	}

	if (teamName && teamName[0])
	{
		const char *f = strstr(psStringEDString, "%s");

		if (f)
		{
			int strLen = 0;
			int i = 0;

			if (ci)
			{
				Com_sprintf(printMsg, sizeof(printMsg), "%s ", ci->name);
				strLen = strlen(printMsg);
			}

			while (psStringEDString[i] && i < 512)
			{
				if (psStringEDString[i] == '%' &&
					psStringEDString[i+1] == 's')
				{
					printMsg[strLen] = '\0';
					Q_strcat(printMsg, sizeof(printMsg), teamName);
					strLen = strlen(printMsg);

					i++;
				}
				else
				{
					printMsg[strLen] = psStringEDString[i];
					strLen++;
				}

				i++;
			}

			printMsg[strLen] = '\0';

			goto doPrint;
		}
	}

	if (ci)
	{
		Com_sprintf(printMsg, sizeof(printMsg), "%s %s", ci->name, psStringEDString);
	}
	else
	{
		Com_sprintf(printMsg, sizeof(printMsg), "%s", psStringEDString);
	}

doPrint:
	Com_Printf("%s\n", printMsg);
}

void CG_GetCTFMessageEvent(entityState_t *es)
{
	int clIndex = es->trickedentindex;
	int teamIndex = es->trickedentindex2;
	clientInfo_t *ci = NULL;
	const char *teamName = NULL;

	if (clIndex < MAX_CLIENTS)
	{
		ci = &cgs.clientinfo[clIndex];
	}

	if (teamIndex < 50)
	{
		teamName = CG_TeamName(teamIndex);
	}

	if (!ci)
	{
		return;
	}

	CG_PrintCTFMessage(ci, teamName, es->eventParm);
}

#include "../namespace_begin.h"
qboolean BG_InKnockDownOnly( int anim );
#include "../namespace_end.h"

//JLFRUMBLE
#ifdef _XBOX
extern void FF_XboxDamage(int damage, float xpos);
#endif

void DoFall(centity_t *cent, entityState_t *es, int clientNum)
{
	int delta = es->eventParm;

	if (cent->currentState.eFlags & EF_DEAD)
	{ //corpses crack into the ground ^_^
		if (delta > 22)
		{
			trap_S_StartSound (NULL, es->number, CHAN_AUTO, cgs.media.fallSound );
		}
		else
		{
			trap_S_StartSound (NULL, es->number, CHAN_AUTO, trap_S_RegisterSound( "sound/movers/objects/objectHit.wav" ) );
		}
	}
	else if (BG_InKnockDownOnly(es->legsAnim))
	{
		if (delta > 7)
		{
			trap_S_StartSound (NULL, es->number, CHAN_AUTO, cgs.media.fallSound );
		}
		else
		{
			trap_S_StartSound (NULL, es->number, CHAN_AUTO, trap_S_RegisterSound( "sound/movers/objects/objectHit.wav" ) );
		}
	}
	else if (delta > 25)
	{
		trap_S_StartSound (NULL, es->number, CHAN_AUTO, cgs.media.fallSound );
		/*trap_S_StartSound( NULL, cent->currentState.number, CHAN_VOICE, 
			CG_CustomSound( cent->currentState.number, va("*land%i.wav", Q_irand(1,3)) ) );*/
		PlayClientSound(&cgs.clientinfo[es->number], CST_LAND, es->number, NULL);
		cent->pe.painTime = cg.time;	// don't play a pain sound right after this
	}
	else if (delta > 22)
	{
		trap_S_StartSound (NULL, es->number, CHAN_AUTO, cgs.media.fallSound );
		/*trap_S_StartSound( NULL, cent->currentState.number, CHAN_VOICE, 
			CG_CustomSound( cent->currentState.number, va("*land%i.wav", Q_irand(1,3)) ) );*/
		PlayClientSound(&cgs.clientinfo[es->number], CST_LAND, es->number, NULL);
		cent->pe.painTime = cg.time;	// don't play a pain sound right after this
	}
	else
	{
		trap_S_StartSound (NULL, es->number, CHAN_AUTO, cgs.media.landSound );
	}
	
	if ( clientNum == cg.predictedPlayerState.clientNum )
	{
		// smooth landing z changes
		cg.landChange = -delta;
		if (cg.landChange > 32)
		{
			cg.landChange = 32;
		}
		if (cg.landChange < -32)
		{
			cg.landChange = -32;
		}
		cg.landTime = cg.time;
	}
//JLFRUMBLE
#ifdef _XBOX
	if ( cent->playerState)
	{

		if (BG_InKnockDownOnly(es->legsAnim))
		{
			if (delta > 14)
			{
				FF_XboxDamage(20, 0);
			}
			else
			{
				FF_XboxDamage(14, 0);
			}
			return;
		}
		if ( delta > 50)
			FF_XboxDamage(50, 0);
		else 
			FF_XboxDamage(delta, 0);
/*		else if (delta > 44)
			FF_XboxDamage(44, 0);
		else 
			FF_XboxDamage(20, 0);
*/
	}
#endif



}

int CG_InClientBitflags(entityState_t *ent, int client)
{
	int checkIn;
	int sub = 0;

	if (client > 47)
	{
		checkIn = ent->trickedentindex4;
		sub = 48;
	}
	else if (client > 31)
	{
		checkIn = ent->trickedentindex3;
		sub = 32;
	}
	else if (client > 15)
	{
		checkIn = ent->trickedentindex2;
		sub = 16;
	}
	else
	{
		checkIn = ent->trickedentindex;
	}

	if (checkIn & (1 << (client-sub)))
	{
		return 1;
	}
	
	return 0;
}

void CG_PlayDoorLoopSound( centity_t *cent );
void CG_PlayDoorSound( centity_t *cent, int type );

void CG_TryPlayCustomSound( vec3_t origin, int entityNum, int channel, const char *soundName )
{
	sfxHandle_t cSound = CG_CustomSound(entityNum, soundName);

	if (cSound <= 0)
	{
		return;
	}

	trap_S_StartSound(origin, entityNum, channel, cSound);

}

void CG_G2MarkEvent(entityState_t *es)
{
	//es->origin should be the hit location of the projectile,
	//whereas es->origin2 is the predicted position of the
	//projectile. (based on the trajectory upon impact) -rww
	centity_t *pOwner = &cg_entities[es->otherEntityNum];
	vec3_t startPoint;
	float	size = 0.0f;
	qhandle_t shader = 0;

	if (!pOwner->ghoul2)
	{ //can't do anything then...
		return;
	}

	//ROP VEHICLE_IMP START
	//Cannot be marked if we are a vehicle which - well - can't be marked ;)
	if(pOwner->currentState.NPC_class == CLASS_VEHICLE 
		&& pOwner->m_pVehicle && pOwner->m_pVehicle->m_pVehicleInfo->ResistsMarking)
	{
		return;
	}
	//ROP VEHICLE_IMP END

	//es->eventParm being non-0 means to do a special trace check
	//first. This will give us an impact right at the surface to
	//project the mark on. Typically this is used for radius
	//explosions and such, where the source position could be
	//way outside of model space.
	if (es->eventParm)
	{
		trace_t tr;
		int ignore = ENTITYNUM_NONE;

		CG_G2Trace(&tr, es->origin, NULL, NULL, es->origin2, ignore, MASK_PLAYERSOLID);

		if (tr.entityNum != es->otherEntityNum)
		{ //try again if we hit an ent but not the one we wanted.
			//CG_TestLine(es->origin, es->origin2, 2000, 0x0000ff, 1);
			if (tr.entityNum < ENTITYNUM_WORLD)
			{
				ignore = tr.entityNum;
				CG_G2Trace(&tr, es->origin, NULL, NULL, es->origin2, ignore, MASK_PLAYERSOLID);
				if (tr.entityNum != es->otherEntityNum)
				{ //try extending the trace a bit.. or not
					/*
					vec3_t v;

					VectorSubtract(es->origin2, es->origin, v);
					VectorScale(v, 64.0f, v);
					VectorAdd(es->origin2, v, es->origin2);

					CG_G2Trace(&tr, es->origin, NULL, NULL, es->origin2, ignore, MASK_PLAYERSOLID);
					if (tr.entityNum != es->otherEntityNum)
					{
						return;
					}
					*/
					//didn't manage to collide with the desired person. No mark will be placed then.
					return;
				}
			}
		}

		//otherwise we now have a valid starting point.
		VectorCopy(tr.endpos, startPoint);
	}
	else
	{
		VectorCopy(es->origin, startPoint);
	}

	if ( (es->eFlags&EF_JETPACK_ACTIVE) )
	{// a vehicle weapon, make it a larger size mark
		//OR base this on the size of the thing you hit?
		if ( g_vehWeaponInfo[es->otherEntityNum2].fG2MarkSize )
		{
			size = flrand( 0.6f, 1.4f )*g_vehWeaponInfo[es->otherEntityNum2].fG2MarkSize;
		}
		else
		{	
			size = flrand( 32.0f, 72.0f );
		}
		//specify mark shader in vehWeapon file
		if ( g_vehWeaponInfo[es->otherEntityNum2].iG2MarkShaderHandle )
		{//have one we want to use instead of defaults
			shader = g_vehWeaponInfo[es->otherEntityNum2].iG2MarkShaderHandle;
		}
	}
	switch(es->weapon)
	{
	case WP_BRYAR_PISTOL:
	case WP_CONCUSSION:
	case WP_BRYAR_OLD:
	case WP_BLASTER:
	case WP_DISRUPTOR:
	case WP_BOWCASTER:
	case WP_REPEATER:
	case WP_TURRET:
		if ( !size )
		{
			size = 4.0f;
		}
		if ( !shader )
		{
			shader = cgs.media.bdecal_bodyburn1;
		}
		CG_AddGhoul2Mark(shader, size, 
			startPoint, es->origin2, es->owner, pOwner->lerpOrigin,
			pOwner->lerpAngles[YAW], pOwner->ghoul2,
			pOwner->modelScale, Q_irand(10000, 20000));
		break;
	case WP_ROCKET_LAUNCHER:
	case WP_THERMAL:
		if ( !size )
		{
			size = 24.0f;
		}
		if ( !shader )
		{
			shader = cgs.media.bdecal_burn1;
		}
		CG_AddGhoul2Mark(shader, size, 
			startPoint, es->origin2, es->owner, pOwner->lerpOrigin,
			pOwner->lerpAngles[YAW], pOwner->ghoul2,
			pOwner->modelScale, Q_irand(10000, 20000));
		break;
		/*
	case WP_FLECHETTE:
		CG_AddGhoul2Mark(cgs.media.bdecal_bodyburn1, flrand(0.5f, 1.0f), 
			startPoint, es->origin2, es->owner, pOwner->lerpOrigin,
			pOwner->lerpAngles[YAW], pOwner->ghoul2,
			pOwner->modelScale);
		break;
		*/
		//Issues with small scale?
	default:
		break;
	}
}

void CG_CalcVehMuzzle(Vehicle_t *pVeh, centity_t *ent, int muzzleNum)
{
	mdxaBone_t boltMatrix;
	vec3_t	vehAngles;

	assert(pVeh);

	if (pVeh->m_iMuzzleTime[muzzleNum] == cg.time)
	{ //already done for this frame, don't need to do it again
		return;
	}
	//Uh... how about we set this, hunh...?  :)
	pVeh->m_iMuzzleTime[muzzleNum] = cg.time;

	VectorCopy( ent->lerpAngles, vehAngles );
	if ( pVeh->m_pVehicleInfo )
	{
		if (pVeh->m_pVehicleInfo->type == VH_ANIMAL
			 ||pVeh->m_pVehicleInfo->type == VH_WALKER)
		{
			vehAngles[PITCH] = vehAngles[ROLL] = 0.0f;
		}
		else if (pVeh->m_pVehicleInfo->type == VH_SPEEDER)
		{
			vehAngles[PITCH] = 0.0f;
		}
	}
	trap_G2API_GetBoltMatrix_NoRecNoRot(ent->ghoul2, 0, pVeh->m_iMuzzleTag[muzzleNum], &boltMatrix, vehAngles,
		ent->lerpOrigin, cg.time, NULL, ent->modelScale);
	BG_GiveMeVectorFromMatrix(&boltMatrix, ORIGIN, pVeh->m_vMuzzlePos[muzzleNum]);
	BG_GiveMeVectorFromMatrix(&boltMatrix, NEGATIVE_Y, pVeh->m_vMuzzleDir[muzzleNum]);
}

//corresponds to G_VehMuzzleFireFX -rww
void CG_VehMuzzleFireFX(centity_t *veh, entityState_t *broadcaster)
{
	Vehicle_t *pVeh = veh->m_pVehicle;
	int curMuz = 0, muzFX = 0;

	if (!pVeh || !veh->ghoul2)
	{
		return;
	}

	for ( curMuz = 0; curMuz < MAX_VEHICLE_MUZZLES; curMuz++ )
	{//go through all muzzles and 
		if ( pVeh->m_iMuzzleTag[curMuz] != -1//valid muzzle bolt
			&& (broadcaster->trickedentindex&(1<<curMuz)) )//fired
		{//this muzzle fired
			muzFX = 0;
			if ( pVeh->m_pVehicleInfo->weapMuzzle[curMuz] == 0 )
			{//no weaopon for this muzzle?  check turrets
				int i, j;
				for ( i = 0; i < MAX_VEHICLE_TURRETS; i++ )
				{
					for ( j = 0; j < MAX_VEHICLE_TURRETS; j++ )
					{
						if ( pVeh->m_pVehicleInfo->turret[i].iMuzzle[j]-1 == curMuz )
						{//this muzzle belongs to this turret
							muzFX = g_vehWeaponInfo[pVeh->m_pVehicleInfo->turret[i].iWeapon].iMuzzleFX;
							break;
						}
					}
				}
			}
			else
			{
				muzFX = g_vehWeaponInfo[pVeh->m_pVehicleInfo->weapMuzzle[curMuz]].iMuzzleFX;
			}
			if ( muzFX )
			{
				//CG_CalcVehMuzzle(pVeh, veh, curMuz);
				//trap_FX_PlayEffectID(muzFX, pVeh->m_vMuzzlePos[curMuz], pVeh->m_vMuzzleDir[curMuz], -1, -1);
				trap_FX_PlayBoltedEffectID(muzFX, veh->currentState.origin, veh->ghoul2, pVeh->m_iMuzzleTag[curMuz], veh->currentState.number, 0, 0, qtrue);
			}
		}
	}
}

const char	*cg_stringEdVoiceChatTable[MAX_CUSTOM_SIEGE_SOUNDS] = 
{
	"VC_ATT",//"*att_attack",
	"VC_ATT_PRIMARY",//"*att_primary",
	"VC_ATT_SECONDARY",//"*att_second",
	"VC_DEF_GUNS",//"*def_guns",
	"VC_DEF_POSITION",//"*def_position",
	"VC_DEF_PRIMARY",//"*def_primary",
	"VC_DEF_SECONDARY",//"*def_second",
	"VC_REPLY_COMING",//"*reply_coming",
	"VC_REPLY_GO",//"*reply_go",
	"VC_REPLY_NO",//"*reply_no",
	"VC_REPLY_STAY",//"*reply_stay",
	"VC_REPLY_YES",//"*reply_yes",
	"VC_REQ_ASSIST",//"*req_assist",
	"VC_REQ_DEMO",//"*req_demo",
	"VC_REQ_HVY",//"*req_hvy",
	"VC_REQ_MEDIC",//"*req_medic",
	"VC_REQ_SUPPLY",//"*req_sup",
	"VC_REQ_TECH",//"*req_tech",
	"VC_SPOT_AIR",//"*spot_air",
	"VC_SPOT_DEF",//"*spot_defenses",
	"VC_SPOT_EMPLACED",//"*spot_emplaced",
	"VC_SPOT_SNIPER",//"*spot_sniper",
	"VC_SPOT_TROOP",//"*spot_troops",
	"VC_TAC_COVER",//"*tac_cover",
	"VC_TAC_FALLBACK",//"*tac_fallback",
	"VC_TAC_FOLLOW",//"*tac_follow",
	"VC_TAC_HOLD",//"*tac_hold",
	"VC_TAC_SPLIT",//"*tac_split",
	"VC_TAC_TOGETHER",//"*tac_together",
	NULL
};

//stupid way of figuring out what string to use for voice chats
const char *CG_GetStringForVoiceSound(const char *s)
{
	int i = 0;
	while (i < MAX_CUSTOM_SIEGE_SOUNDS)
	{
		if (bg_customSiegeSoundNames[i] &&
			!Q_stricmp(bg_customSiegeSoundNames[i], s))
		{ //get the matching reference name
			assert(cg_stringEdVoiceChatTable[i]);
			return CG_GetStringEdString("MENUS", (char *)cg_stringEdVoiceChatTable[i]);
		}
		i++;
	}

	return "voice chat";
}

/*
==============
CG_EntityEvent

An entity has an event value
also called by CG_CheckPlayerstateEvents
==============
*/
#define	DEBUGNAME(x) if(cg_debugEvents.integer){CG_Printf(x"\n");}
extern void CG_ChatBox_AddString(char *chatStr); //cg_draw.c
void CG_EntityEvent( centity_t *cent, vec3_t position ) {
	entityState_t	*es;
	int				event;
	vec3_t			dir;
//	vec3_t			origin;
	const char		*s;
	int				clientNum;
	clientInfo_t	*ci;
	int				eID = 0;
	int				isnd = 0;
	centity_t		*cl_ent;
//	int				i;
//	qboolean		found = qfalse;

	es = &cent->currentState;
	event = es->event & ~EV_EVENT_BITS;

	if ( cg_debugEvents.integer ) {
		CG_Printf( "ent:%3i  event:%3i ", es->number, event );
	}

	if ( !event ) {
		DEBUGNAME("ZEROEVENT");
		return;
	}

	clientNum = es->clientNum;
	if ( clientNum < 0 || clientNum >= MAX_CLIENTS ) {
		clientNum = 0;
	}

	if (es->eType == ET_NPC)
	{
		clientNum = es->number;

		if (!cent->npcClient)
		{
			CG_CreateNPCClient(&cent->npcClient); //allocate memory for it

			if (!cent->npcClient)
			{
				assert(0);
				return;
			}

			memset(cent->npcClient, 0, sizeof(clientInfo_t));
			cent->npcClient->ghoul2Model = NULL;
			//[CoOp]
			//figure out the gender for this model
			cent->npcClient->gender = FindGender(CG_ConfigString( CS_MODELS+cent->currentState.modelindex ), cent);
			//[/CoOp]
		}

		ci = cent->npcClient;

		assert(ci);
	}
	else
	{
		ci = &cgs.clientinfo[ clientNum ];
	}

	switch ( event ) {
	//
	// movement generated events
	//
	case EV_CLIENTJOIN:
		DEBUGNAME("EV_CLIENTJOIN");

		//Slight hack to force a local reinit of client entity on join.
		cl_ent = &cg_entities[es->eventParm];

		if (cl_ent)
		{
			//cl_ent->torsoBolt = 0;
			cl_ent->bolt1 = 0;
			cl_ent->bolt2 = 0;
			cl_ent->bolt3 = 0;
			cl_ent->bolt4 = 0;
			cl_ent->bodyHeight = 0;//SABER_LENGTH_MAX;
			//cl_ent->saberExtendTime = 0;
			cl_ent->boltInfo = 0;
			cl_ent->frame_minus1_refreshed = 0;
			cl_ent->frame_minus2_refreshed = 0;
			cl_ent->frame_hold_time = 0;
			cl_ent->frame_hold_refreshed = 0;
			cl_ent->trickAlpha = 0;
			cl_ent->trickAlphaTime = 0;
			cl_ent->ghoul2weapon = NULL;
#ifdef AKIMBO
			cl_ent->ghoul2weapon2 = NULL;
#endif
			cl_ent->weapon = WP_NONE;
			cl_ent->teamPowerEffectTime = 0;
			cl_ent->teamPowerType = 0;
			cl_ent->numLoopingSounds = 0;
			//cl_ent->localAnimIndex = 0;
		}
		CG_EVBox_AddString("Player connected.");
		break;

	case EV_FOOTSTEP:
		DEBUGNAME("EV_FOOTSTEP");
		if (cg_footsteps.integer) {
			footstep_t	soundType;
			switch( es->eventParm )
			{
			case MATERIAL_MUD:
				soundType = FOOTSTEP_MUDWALK;
				break;
			case MATERIAL_DIRT:			
				soundType = FOOTSTEP_DIRTWALK;
				break;
			case MATERIAL_SAND:			
				soundType = FOOTSTEP_SANDWALK;
				break;
			case MATERIAL_SNOW:			
				soundType = FOOTSTEP_SNOWWALK;
				break;
			case MATERIAL_SHORTGRASS:		
			case MATERIAL_LONGGRASS:		
				soundType = FOOTSTEP_GRASSWALK;
				break;
			case MATERIAL_SOLIDMETAL:		
				soundType = FOOTSTEP_METALWALK;
				break;
			case MATERIAL_HOLLOWMETAL:	
				soundType = FOOTSTEP_PIPEWALK;
				break;
			case MATERIAL_GRAVEL:
				soundType = FOOTSTEP_GRAVELWALK;
				break;
			case MATERIAL_CARPET:
			case MATERIAL_FABRIC:
			case MATERIAL_CANVAS:
			case MATERIAL_RUBBER:
			case MATERIAL_PLASTIC:
				soundType = FOOTSTEP_RUGWALK;
				break;
			case MATERIAL_SOLIDWOOD:
			case MATERIAL_HOLLOWWOOD:
				soundType = FOOTSTEP_WOODWALK;
				break;

			default:
				soundType = FOOTSTEP_STONEWALK;
				break;
			}

			trap_S_StartSound (NULL, es->number, CHAN_BODY, cgs.media.footsteps[ soundType ][rand()&3] );
		}
		break;
	case EV_FOOTSTEP_METAL:
		DEBUGNAME("EV_FOOTSTEP_METAL");
		if (cg_footsteps.integer) {
			trap_S_StartSound (NULL, es->number, CHAN_BODY, 
				cgs.media.footsteps[ FOOTSTEP_METALWALK ][rand()&3] );
		}
		break;
	case EV_FOOTSPLASH:
		DEBUGNAME("EV_FOOTSPLASH");
		if (cg_footsteps.integer) {
			trap_S_StartSound (NULL, es->number, CHAN_BODY, 
				cgs.media.footsteps[ FOOTSTEP_SPLASH ][rand()&3] );
		}
		break;
	case EV_FOOTWADE:
		DEBUGNAME("EV_FOOTWADE");
		if (cg_footsteps.integer) {
			trap_S_StartSound (NULL, es->number, CHAN_BODY, 
				cgs.media.footsteps[ FOOTSTEP_SPLASH ][rand()&3] );
		}
		break;
	case EV_SWIM:
		DEBUGNAME("EV_SWIM");
		if (cg_footsteps.integer) {
			trap_S_StartSound (NULL, es->number, CHAN_BODY, 
				cgs.media.footsteps[ FOOTSTEP_SPLASH ][rand()&3] );
		}
		break;


	case EV_FALL:
		DEBUGNAME("EV_FALL");
		if (es->number == cg.snap->ps.clientNum && cg.snap->ps.fallingToDeath)
		{
			break;
		}
		DoFall(cent, es, clientNum);
		break;
	case EV_STEP_4:
	case EV_STEP_8:
	case EV_STEP_12:
	case EV_STEP_16:		// smooth out step up transitions
		DEBUGNAME("EV_STEP");
	{
		float	oldStep;
		int		delta;
		int		step;

		if ( clientNum != cg.predictedPlayerState.clientNum ) {
			break;
		}
		// if we are interpolating, we don't need to smooth steps
		if ( cg.demoPlayback || (cg.snap->ps.pm_flags & PMF_FOLLOW) ||
			cg_nopredict.integer || cg_synchronousClients.integer ) {
			break;
		}
		// check for stepping up before a previous step is completed
		delta = cg.time - cg.stepTime;
		if (delta < STEP_TIME) {
			oldStep = cg.stepChange * (STEP_TIME - delta) / STEP_TIME;
		} else {
			oldStep = 0;
		}

		// add this amount
		step = 4 * (event - EV_STEP_4 + 1 );
		cg.stepChange = oldStep + step;
		if ( cg.stepChange > MAX_STEP_CHANGE ) {
			cg.stepChange = MAX_STEP_CHANGE;
		}
		cg.stepTime = cg.time;
		break;
	}

	case EV_DOOR_LOCKED:
		DEBUGNAME("EV_DOOR_LOCKED");
		{
			if(es->eventParm)
			{
				trap_S_StartSound( NULL, es->number, CHAN_BODY, cgs.media.doorUnlockedSound  );
			}
			else
			{
				CG_EVBox_AddString("This door is locked.");
				CG_EVBox_AddString("Break it down, kick it down, or hold Aim Down Sights + USE to unlock.");
				trap_S_StartSound( NULL, es->number, CHAN_BODY, cgs.media.doorLockedSound  );
			}
		}
		break;

	case EV_JUMP_PAD:
		DEBUGNAME("EV_JUMP_PAD");
		break;

	case EV_GHOUL2_MARK:
		DEBUGNAME("EV_GHOUL2_MARK");

		if (cg_ghoul2Marks.integer)
		{ //Can we put a burn mark on him?
			CG_G2MarkEvent(es);
		}
		break;

	case EV_GLOBAL_DUEL:
		DEBUGNAME("EV_GLOBAL_DUEL");
		//used for beginning of power duels
		//if (cg.predictedPlayerState.persistant[PERS_TEAM] != TEAM_SPECTATOR)
		if (es->otherEntityNum == cg.predictedPlayerState.clientNum ||
			es->otherEntityNum2 == cg.predictedPlayerState.clientNum ||
			es->groundEntityNum == cg.predictedPlayerState.clientNum)
		{
			CG_CenterPrint( CG_GetStringEdString("MP_SVGAME", "BEGIN_DUEL"), 120, GIANTCHAR_WIDTH*2 );				
			trap_S_StartLocalSound( cgs.media.countFightSound, CHAN_ANNOUNCER );
		}
		break;

	case EV_PRIVATE_DUEL:
		DEBUGNAME("EV_PRIVATE_DUEL");

		if (cg.snap->ps.clientNum != es->number)
		{
			break;
		}

		if (es->eventParm)
		{ //starting the duel
			if (es->eventParm == 2)
			{
				CG_CenterPrint( CG_GetStringEdString("MP_SVGAME", "BEGIN_DUEL"), 120, GIANTCHAR_WIDTH*2 );				
				trap_S_StartLocalSound( cgs.media.countFightSound, CHAN_ANNOUNCER );
			}
			else
			{
				trap_S_StartBackgroundTrack( "music/mp/duel.mp3", "music/mp/duel.mp3", qfalse );
			}
		}
		else
		{ //ending the duel
			CG_StartMusic(qtrue);
		}
		break;

	case EV_JUMP:
		DEBUGNAME("EV_JUMP");
		if (cg_jumpSounds.integer)
		{
			//trap_S_StartSound (NULL, es->number, CHAN_VOICE, CG_CustomSound( es->number, va("*jump%i.wav", Q_irand(1, 3)) ) );
			PlayClientSound(&cgs.clientinfo[es->number], CST_JUMP, es->number, NULL);
		}
		break;
	case EV_ROLL:
		DEBUGNAME("EV_ROLL");
		if (es->number == cg.snap->ps.clientNum && cg.snap->ps.fallingToDeath)
		{
			break;
		}
		if (es->eventParm)
		{ //fall-roll-in-one event
			DoFall(cent, es, clientNum);
		}

		//trap_S_StartSound (NULL, es->number, CHAN_VOICE, CG_CustomSound( es->number, va("*jump%i.wav", Q_irand(1, 3)) ) );
		PlayClientSound(&cgs.clientinfo[es->number], CST_JUMP, es->number, NULL);
		trap_S_StartSound( NULL, es->number, CHAN_BODY, cgs.media.rollSound  );

		//FIXME: need some sort of body impact on ground sound and maybe kick up some dust?
		break;

	case EV_TAUNT:
		DEBUGNAME("EV_TAUNT");
		{
			int soundIndex = 0;
			//[AllTAUNTS]
			//We want players to have a larger range of taunt taunt sounds in all gametypes.
			/*
			if ( cgs.gametype != GT_DUEL
				&& cgs.gametype != GT_POWERDUEL
				&& es->eventParm == TAUNT_TAUNT )
			{//normal taunt
				soundIndex = CG_CustomSound( es->number, "*taunt.wav" );
			}
			else
			*/
			//[/AllTAUNTS]
			{
				switch ( es->eventParm )
				{
				case TAUNT_TAUNT:
				default:
					//Dafuq is this shite..
					/*if ( Q_irand( 0, 1 ) )
					{
						soundIndex = CG_CustomSound( es->number, va("*anger%d.wav", Q_irand(1,3)) );
					}
					else
					{
						soundIndex = CG_CustomSound( es->number, va("*taunt%d.wav", Q_irand(1,3)) );
						if ( !soundIndex )
						{
							soundIndex = CG_CustomSound( es->number, va("*anger%d.wav", Q_irand(1,3)) );
						}
					}*/
					soundIndex = 0;
					break;
				case TAUNT_BOW:
					//soundIndex = CG_CustomSound( es->number, va("*respect%d.wav", Q_irand(1,3)) );
					break;
				case TAUNT_MEDITATE:
					//soundIndex = CG_CustomSound( es->number, va("*meditate%d.wav", Q_irand(1,3)) );
					break;
				case TAUNT_FLOURISH:
					if ( Q_irand( 0, 1 ) )
					{
						soundIndex = CG_CustomSound( es->number, va("*deflect%d.wav", Q_irand(1,3)) );
						if ( !soundIndex )
						{
							soundIndex = CG_CustomSound( es->number, va("*gloat%d.wav", Q_irand(1,3)) );
							if ( !soundIndex )
							{
								soundIndex = CG_CustomSound( es->number, va("*anger%d.wav", Q_irand(1,3)) );
							}
						}
					}
					else
					{
						soundIndex = CG_CustomSound( es->number, va("*gloat%d.wav", Q_irand(1,3)) );
						if ( !soundIndex )
						{
							soundIndex = CG_CustomSound( es->number, va("*deflect%d.wav", Q_irand(1,3)) );
							if ( !soundIndex )
							{
								soundIndex = CG_CustomSound( es->number, va("*anger%d.wav", Q_irand(1,3)) );
							}
						}
					}
					break;
				case TAUNT_GLOAT:
					soundIndex = CG_CustomSound( es->number, va("*victory%d.wav", Q_irand(1,3)) );
					break;
				}
			}
			if ( !soundIndex )
			{
				//soundIndex = CG_CustomSound( es->number, "*taunt.wav" );
				PlayClientSound(&cgs.clientinfo[es->number], CST_TAUNT, es->number, NULL);

			}
			if ( soundIndex )
			{
				trap_S_StartSound (NULL, es->number, CHAN_VOICE, soundIndex );
			}
		}
		break;

		//Begin NPC sounds
	case EV_ANGER1:	//Say when acquire an enemy when didn't have one before
	case EV_ANGER2:
	case EV_ANGER3:
		DEBUGNAME("EV_ANGERx");
		CG_TryPlayCustomSound( NULL, es->number, CHAN_VOICE, va("*anger%i.wav", event - EV_ANGER1 + 1) );
		break;

	case EV_VICTORY1:	//Say when killed an enemy
	case EV_VICTORY2:
	case EV_VICTORY3:
		DEBUGNAME("EV_VICTORYx");
		CG_TryPlayCustomSound( NULL, es->number, CHAN_VOICE, va("*victory%i.wav", event - EV_VICTORY1 + 1) );
		break;

	case EV_CONFUSE1:	//Say when confused
	case EV_CONFUSE2:
	case EV_CONFUSE3:
		DEBUGNAME("EV_CONFUSEDx");
		CG_TryPlayCustomSound( NULL, es->number, CHAN_VOICE, va("*confuse%i.wav", event - EV_CONFUSE1 + 1) );
		break;

	case EV_PUSHED1:	//Say when pushed
	case EV_PUSHED2:
	case EV_PUSHED3:
		DEBUGNAME("EV_PUSHEDx");
		CG_TryPlayCustomSound( NULL, es->number, CHAN_VOICE, va("*pushed%i.wav", event - EV_PUSHED1 + 1) );
		break;

	case EV_CHOKE1:	//Say when choking
	case EV_CHOKE2:
	case EV_CHOKE3:
		DEBUGNAME("EV_CHOKEx");
		CG_TryPlayCustomSound( NULL, es->number, CHAN_VOICE, va("*choke%i.wav", event - EV_CHOKE1 + 1) );
		break;

	case EV_FFWARN:	//Warn ally to stop shooting you
		DEBUGNAME("EV_FFWARN");
		CG_TryPlayCustomSound( NULL, es->number, CHAN_VOICE, "*ffwarn.wav" );
		break;

	case EV_FFTURN:	//Turn on ally after being shot by them
		DEBUGNAME("EV_FFTURN");
		CG_TryPlayCustomSound( NULL, es->number, CHAN_VOICE, "*ffturn.wav" );
		break;

	//extra sounds for ST
	case EV_CHASE1:
	case EV_CHASE2:
	case EV_CHASE3:
		DEBUGNAME("EV_CHASEx");
		CG_TryPlayCustomSound( NULL, es->number, CHAN_VOICE, va("*chase%i.wav", event - EV_CHASE1 + 1) );
		break;
	case EV_COVER1:
	case EV_COVER2:
	case EV_COVER3:
	case EV_COVER4:
	case EV_COVER5:
		DEBUGNAME("EV_COVERx");
		CG_TryPlayCustomSound( NULL, es->number, CHAN_VOICE, va("*cover%i.wav", event - EV_COVER1 + 1) );
		break;
	case EV_DETECTED1:
	case EV_DETECTED2:
	case EV_DETECTED3:
	case EV_DETECTED4:
	case EV_DETECTED5:
		DEBUGNAME("EV_DETECTEDx");
		CG_TryPlayCustomSound( NULL, es->number, CHAN_VOICE, va("*detected%i.wav", event - EV_DETECTED1 + 1) );
		break;
	case EV_GIVEUP1:
	case EV_GIVEUP2:
	case EV_GIVEUP3:
	case EV_GIVEUP4:
		DEBUGNAME("EV_GIVEUPx");
		CG_TryPlayCustomSound( NULL, es->number, CHAN_VOICE, va("*giveup%i.wav", event - EV_GIVEUP1 + 1) );
		break;
	case EV_LOOK1:
	case EV_LOOK2:
		DEBUGNAME("EV_LOOKx");
		CG_TryPlayCustomSound( NULL, es->number, CHAN_VOICE, va("*look%i.wav", event - EV_LOOK1 + 1) );
		break;
	case EV_LOST1:
		DEBUGNAME("EV_LOST1");
		CG_TryPlayCustomSound( NULL, es->number, CHAN_VOICE, "*lost1.wav" );
		break;
	case EV_OUTFLANK1:
	case EV_OUTFLANK2:
		DEBUGNAME("EV_OUTFLANKx");
		CG_TryPlayCustomSound( NULL, es->number, CHAN_VOICE, va("*outflank%i.wav", event - EV_OUTFLANK1 + 1) );
		break;
	case EV_ESCAPING1:
	case EV_ESCAPING2:
	case EV_ESCAPING3:
		DEBUGNAME("EV_ESCAPINGx");
		CG_TryPlayCustomSound( NULL, es->number, CHAN_VOICE, va("*escaping%i.wav", event - EV_ESCAPING1 + 1) );
		break;
	case EV_SIGHT1:
	case EV_SIGHT2:
	case EV_SIGHT3:
		DEBUGNAME("EV_SIGHTx");
		CG_TryPlayCustomSound( NULL, es->number, CHAN_VOICE, va("*sight%i.wav", event - EV_SIGHT1 + 1) );
		break;
	case EV_SOUND1:
	case EV_SOUND2:
	case EV_SOUND3:
		DEBUGNAME("EV_SOUNDx");
		CG_TryPlayCustomSound( NULL, es->number, CHAN_VOICE, va("*sound%i.wav", event - EV_SOUND1 + 1) );
		break;
	case EV_SUSPICIOUS1:
	case EV_SUSPICIOUS2:
	case EV_SUSPICIOUS3:
	case EV_SUSPICIOUS4:
	case EV_SUSPICIOUS5:
		DEBUGNAME("EV_SUSPICIOUSx");
		CG_TryPlayCustomSound( NULL, es->number, CHAN_VOICE, va("*suspicious%i.wav", event - EV_SUSPICIOUS1 + 1) );
		break;
	//extra sounds for Jedi
	case EV_COMBAT1:
	case EV_COMBAT2:
	case EV_COMBAT3:
		DEBUGNAME("EV_COMBATx");
		CG_TryPlayCustomSound( NULL, es->number, CHAN_VOICE, va("*combat%i.wav", event - EV_COMBAT1 + 1) );
		break;
	case EV_JDETECTED1:
	case EV_JDETECTED2:
	case EV_JDETECTED3:
		DEBUGNAME("EV_JDETECTEDx");
		CG_TryPlayCustomSound( NULL, es->number, CHAN_VOICE, va("*jdetected%i.wav", event - EV_JDETECTED1 + 1) );
		break;
	case EV_TAUNT1:
	case EV_TAUNT2:
	case EV_TAUNT3:
		DEBUGNAME("EV_TAUNTx");
		CG_TryPlayCustomSound( NULL, es->number, CHAN_VOICE, va("*taunt%i.wav", event - EV_TAUNT1 + 1) );
		break;
	case EV_JCHASE1:
	case EV_JCHASE2:
	case EV_JCHASE3:
		DEBUGNAME("EV_JCHASEx");
		CG_TryPlayCustomSound( NULL, es->number, CHAN_VOICE, va("*jchase%i.wav", event - EV_JCHASE1 + 1) );
		break;
	case EV_JLOST1:
	case EV_JLOST2:
	case EV_JLOST3:
		DEBUGNAME("EV_JLOSTx");
		CG_TryPlayCustomSound( NULL, es->number, CHAN_VOICE, va("*jlost%i.wav", event - EV_JLOST1 + 1) );
		break;
	case EV_DEFLECT1:
	case EV_DEFLECT2:
	case EV_DEFLECT3:
		DEBUGNAME("EV_DEFLECTx");
		CG_TryPlayCustomSound( NULL, es->number, CHAN_VOICE, va("*deflect%i.wav", event - EV_DEFLECT1 + 1) );
		break;
	case EV_GLOAT1:
	case EV_GLOAT2:
	case EV_GLOAT3:
		DEBUGNAME("EV_GLOATx");
		CG_TryPlayCustomSound( NULL, es->number, CHAN_VOICE, va("*gloat%i.wav", event - EV_GLOAT1 + 1) );
		break;
	case EV_PUSHFAIL:
		DEBUGNAME("EV_PUSHFAIL");
		CG_TryPlayCustomSound( NULL, es->number, CHAN_VOICE, "*pushfail.wav" );
		break;
		//End NPC sounds

	case EV_SIEGESPEC:
		DEBUGNAME("EV_SIEGESPEC");
		if ( es->owner == cg.predictedPlayerState.clientNum )
		{
			cg_siegeDeathTime = es->time;
		}

		break;
		
	case EV_WATER_TOUCH:
		DEBUGNAME("EV_WATER_TOUCH");
		trap_S_StartSound (NULL, es->number, CHAN_AUTO, cgs.media.watrInSound );
		break;
	case EV_WATER_LEAVE:
		DEBUGNAME("EV_WATER_LEAVE");
		trap_S_StartSound (NULL, es->number, CHAN_AUTO, cgs.media.watrOutSound );
		break;
	case EV_WATER_UNDER:
		DEBUGNAME("EV_WATER_UNDER");
		trap_S_StartSound (NULL, es->number, CHAN_AUTO, cgs.media.watrUnSound );
		break;
	case EV_WATER_CLEAR:
		DEBUGNAME("EV_WATER_CLEAR");
		trap_S_StartSound (NULL, es->number, CHAN_AUTO, CG_CustomSound( es->number, "*gasp.wav" ) );
		break;

	//eezstreet add
	case EV_ITEM_PICKUP:
		DEBUGNAME("EV_ITEM_PICKUP");
		{
			gitem_t	*item;
			int		index;
			qboolean	newindex = qfalse;

			index = cg_entities[es->eventParm].currentState.modelindex;		// player predicted

			if (index < 1 && cg_entities[es->eventParm].currentState.isJediMaster)
			{ //a holocron most likely
				index = cg_entities[es->eventParm].currentState.trickedentindex4;
				trap_S_StartSound (NULL, es->number, CHAN_AUTO,	cgs.media.holocronPickup );
								
				if (es->number == cg.snap->ps.clientNum && showPowersName[index])
				{
					const char *strText = CG_GetStringEdString("MP_INGAME", "PICKUPLINE");

					//Com_Printf("%s %s\n", strText, showPowersName[index]);
					CG_CenterPrint( va("%s %s\n", strText, CG_GetStringEdString("SP_INGAME",showPowersName[index])), SCREEN_HEIGHT * 0.30, BIGCHAR_WIDTH );
				}

				//Show the player their force selection bar in case picking the holocron up changed the current selection
				if (index != FP_LEVITATION && index != FP_SABER_OFFENSE && index != FP_SABER_DEFENSE && index != FP_SABERTHROW &&
					es->number == cg.snap->ps.clientNum &&
					(index == cg.snap->ps.fd.forcePowerSelected || !(cg.snap->ps.fd.forcePowersActive & (1 << cg.snap->ps.fd.forcePowerSelected))))
				{
					if (cg.forceSelect != index)
					{
						cg.forceSelect = index;
						newindex = qtrue;
					}
				}

				if (es->number == cg.snap->ps.clientNum && newindex)
				{
					if (cg.forceSelectTime < cg.time)
					{
						cg.forceSelectTime = cg.time;
					}
				}

				break;
			}

			if (cg_entities[es->eventParm].weapon >= cg.time)
			{ //rww - an unfortunately necessary hack to prevent double item pickups
				break;
			}

			//Hopefully even if this entity is somehow removed and replaced with, say, another
			//item, this time will have expired by the time that item needs to be picked up.
			//Of course, it's quite possible this will fail miserably, so if you've got a better
			//solution then please do use it.
			cg_entities[es->eventParm].weapon = cg.time+500;

			if ( index < 1 || index >= bg_numItems ) {
				break;
			}
			item = &bg_itemlist[ index ];

			if ( /*item->giType != IT_POWERUP && */item->giType != IT_TEAM) {
				if (item->pickup_sound && item->pickup_sound[0])
				{
					trap_S_StartSound (NULL, es->number, CHAN_AUTO,	trap_S_RegisterSound( item->pickup_sound ) );
				}
			}

			// show icon and name on status bar
			if ( es->number == cg.snap->ps.clientNum ) {
				CG_ItemPickup( index, qfalse );
			}
		}
		break;

	case EV_WEAPON_PICKUP:
		DEBUGNAME("EV_WEAPON_PICKUP");
		{
			gitem_t	*item;
			int		index;

			index = cg_entities[es->eventParm].currentState.modelindex;		// player predicted

			if (cg_entities[es->eventParm].weapon >= cg.time)
			{ //rww - an unfortunately necessary hack to prevent double item pickups
				break;
			}

			//Hopefully even if this entity is somehow removed and replaced with, say, another
			//item, this time will have expired by the time that item needs to be picked up.
			//Of course, it's quite possible this will fail miserably, so if you've got a better
			//solution then please do use it.
			cg_entities[es->eventParm].weapon = cg.time+500;

			if ( index < 1 || index >= bg_numItems ) {
				break;
			}
			item = &bg_itemlist[ index ];

			if (item->pickup_sound && item->pickup_sound[0])
			{
					trap_S_StartSound (NULL, es->number, CHAN_AUTO,	trap_S_RegisterSound( "sound/ui/pickupgun" ) );
			}

			// show icon and name on status bar
			if ( es->number == cg.snap->ps.clientNum ) {
				CG_ItemPickup( index, qfalse );
			}
		}
		break;

	case EV_AMMO_PICKUP:
		DEBUGNAME("EV_AMMO_PICKUP");
		{
			gitem_t	*item;
			int		index;

			index = cg_entities[es->eventParm].currentState.modelindex;		// player predicted

			if (cg_entities[es->eventParm].weapon >= cg.time)
			{ //rww - an unfortunately necessary hack to prevent double item pickups
				break;
			}

			//Hopefully even if this entity is somehow removed and replaced with, say, another
			//item, this time will have expired by the time that item needs to be picked up.
			//Of course, it's quite possible this will fail miserably, so if you've got a better
			//solution then please do use it.
			cg_entities[es->eventParm].weapon = cg.time+500;

			if ( index < 1 || index >= bg_numItems ) {
				break;
			}
			item = &bg_itemlist[ index ];

			if (item->pickup_sound && item->pickup_sound[0])
			{
					trap_S_StartSound (NULL, es->number, CHAN_AUTO,	trap_S_RegisterSound( "sound/ui/pickupammo" ) );
			}

			// show icon and name on status bar
			if ( es->number == cg.snap->ps.clientNum ) {
				CG_ItemPickup( index, qtrue );
			}
		}

	case EV_GLOBAL_ITEM_PICKUP:
		DEBUGNAME("EV_GLOBAL_ITEM_PICKUP");
		{
			gitem_t	*item;
			int		index;

			index = es->eventParm;		// player predicted

			if ( index < 1 || index >= bg_numItems ) {
				break;
			}
			item = &bg_itemlist[ index ];
			// powerup pickups are global
			if( item->pickup_sound && item->pickup_sound[0] ) {
				trap_S_StartSound (NULL, cg.snap->ps.clientNum, CHAN_AUTO, trap_S_RegisterSound( item->pickup_sound) );
			}

			// show icon and name on status bar
			if ( es->number == cg.snap->ps.clientNum ) {
				CG_ItemPickup( index, qfalse );
			}
		}
		break;

	case EV_VEH_FIRE:
		DEBUGNAME("EV_VEH_FIRE");
		{
			centity_t *veh = &cg_entities[es->owner];
			CG_VehMuzzleFireFX(veh, es);
		}
		break;

	//
	// weapon events
	//
	case EV_NOAMMO:
		DEBUGNAME("EV_NOAMMO");
//		trap_S_StartSound (NULL, es->number, CHAN_AUTO, cgs.media.noAmmoSound );
		if ( es->number == cg.snap->ps.clientNum )
		{
			if ( CG_InFighter() || CG_InATST() || cg.snap->ps.weapon == WP_NONE )
			{//just letting us know our vehicle is out of ammo
				//FIXME: flash something on HUD or give some message so we know we have no ammo
				centity_t *localCent = &cg_entities[cg.snap->ps.clientNum];
				if ( localCent->m_pVehicle 
					&& localCent->m_pVehicle->m_pVehicleInfo
					&& localCent->m_pVehicle->m_pVehicleInfo->weapon[es->eventParm].soundNoAmmo )
				{//play the "no Ammo" sound for this weapon
					trap_S_StartSound (NULL, cg.snap->ps.clientNum, CHAN_AUTO, localCent->m_pVehicle->m_pVehicleInfo->weapon[es->eventParm].soundNoAmmo );
				}
				else
				{//play the default "no ammo" sound
					trap_S_StartSound (NULL, cg.snap->ps.clientNum, CHAN_AUTO, cgs.media.noAmmoSound );
				}
				//flash the HUD so they associate the sound with the visual indicator that they don't have enough ammo
				if ( cg_vehicleAmmoWarningTime < cg.time
					|| cg_vehicleAmmoWarning != es->eventParm )
				{//if there's already one going, don't interrupt it (unless they tried to fire another weapon that's out of ammo)
					cg_vehicleAmmoWarning = es->eventParm;
					cg_vehicleAmmoWarningTime = cg.time+500;
				}
			}
			else if ( cg.snap->ps.weapon == WP_SABER )
			{
				cg.forceHUDTotalFlashTime = cg.time + 1000;
			}
			else
			{
				/*int weap = 0;

				if (es->eventParm && es->eventParm < WP_NUM_WEAPONS)
				{
					cg.snap->ps.stats[STAT_WEAPONS] &= ~(1 << es->eventParm);
					weap = cg.snap->ps.weapon;
				}
				else if (es->eventParm)
				{
					weap = (es->eventParm-WP_NUM_WEAPONS);
				}
				trap_S_StartSound (NULL, cg.snap->ps.clientNum, CHAN_AUTO, cgs.media.noAmmoSound );
				CG_OutOfAmmoChange(weap);*/
				trap_S_StartSound( NULL, es->number, CHAN_AUTO, cgs.media.noAmmoSound );
				if(pm && pm->ps)
				{
#ifndef AKIMBO
					if(weaponData[pm->ps->weapon].clipSize && cg.snap->ps.ammo[weaponData[cg.snap->ps.weapon].ammoIndex] > 0)
						trap_SendClientCommand( "reload" );
					//Sleight of Hand is no longer a valid achievement.
#else
					if(!pm->ps->weapon2)
					{
						trap_SendClientCommand( "reload" );
					}
					else
					{
						qboolean rValid = qfalse;
						qboolean lValid = qfalse;
						if(cg.snap->ps.ammo[weaponData[cg.snap->ps.weapon].ammoIndex] > 0 && weaponData[pm->ps->weapon].clipSize && pm->ps->stats[STAT_CLIP] < weaponData[pm->ps->weapon].clipSize)
						{
							rValid = qtrue;
						}
						if(cg.snap->ps.ammo[weaponData[cg.snap->ps.weapon2].ammoIndex] > 0 && weaponData[pm->ps->weapon2].clipSize && pm->ps->stats[STAT_CLIP2] < weaponData[pm->ps->weapon2].clipSize)
						{
							lValid = qtrue;
						}
						if(rValid && !lValid)
						{
							trap_SendClientCommand( "reload 1" );
						}
						else if(!rValid && lValid)
						{
							trap_SendClientCommand( "reload 2" );
						}
						else
						{
							trap_SendClientCommand( "reload" );
						}
					}
#endif
				}
				else
				{
					int weap = 0;

					if (es->eventParm && es->eventParm < WP_NUM_WEAPONS)
					{
						cg.snap->ps.stats[STAT_WEAPONS] &= ~(1 << es->eventParm);
						weap = cg.snap->ps.weapon;
					}
					else if (es->eventParm)
					{
						weap = (es->eventParm-WP_NUM_WEAPONS);
					}
					trap_S_StartSound (NULL, cg.snap->ps.clientNum, CHAN_AUTO, cgs.media.noAmmoSound );
					CG_OutOfAmmoChange(weap);
				}
			}
		}
		break;
	case EV_CHANGE_WEAPON:
		DEBUGNAME("EV_CHANGE_WEAPON");
		{
			int weapon = es->eventParm;
			weaponInfo_t *weaponInfo;
			
			assert(weapon >= 0 && weapon < MAX_WEAPONS);

			weaponInfo = &cg_weapons[weapon];

			assert(weaponInfo);

			if (weaponInfo->selectSound)
			{
				trap_S_StartSound (NULL, es->number, CHAN_AUTO, weaponInfo->selectSound );
			}
			else if (weapon != WP_SABER)
			{ //not sure what SP is doing for this but I don't want a select sound for saber (it has the saber-turn-on)
				trap_S_StartSound (NULL, es->number, CHAN_AUTO, cgs.media.selectSound );
			}
		}
		break;
	//Boba/Raz credit
	case EV_RELOAD:
		DEBUGNAME("EV_RELOAD");
		{
			int weapon = es->eventParm;
			weaponInfo_t *weaponInfo;

			if(weapon < 0 || weapon >= WP_NUM_WEAPONS)
			{
				Com_Error(ERR_DISCONNECT, "Attempted reloading weapon %i (nonexistant)", weapon);
				break;
			}
#ifdef AKIMBO
			if(es->userInt1)
				cg.reloadingBothGuns = qtrue;
			else
				cg.reloadingBothGuns = qfalse;
			cg.reloadingLeft = qfalse;
			cg.reloadingRight = qtrue;
#endif

			weaponInfo = &cg_weapons[weapon];

			trap_S_StartSound( NULL, cent->currentState.number, CHAN_AUTO, weaponInfo->selectSound ? weaponInfo->selectSound : cgs.media.selectSound );
			cg.reloadTimeStart = cg.time;
			cg.reloadTimeDuration = es->time;

		}
		break;
#ifdef AKIMBO
	case EV_RELOAD_R:
		DEBUGNAME("EV_RELOAD_R");
		{
			//Reloading right weapon
			weaponInfo_t *weaponInfo;
			int weapon = es->eventParm;

			if(weapon < 0 || weapon >= WP_NUM_WEAPONS)
			{
				Com_Error(ERR_DISCONNECT, "Attempted reloading weapon2 %i (nonexistant)", weapon);
				break;
			}

			cg.reloadingRight = qtrue;
			cg.reloadingBothGuns = qfalse;
			cg.reloadingLeft = qfalse;

			weaponInfo = &cg_weapons[weapon];

			trap_S_StartSound( NULL, cent->currentState.number, CHAN_AUTO, weaponInfo->selectSound ? weaponInfo->selectSound : cgs.media.selectSound );
			cg.reloadTimeRightStart = cg.time;
			cg.reloadTimeRightDuration = es->time;
		}
		break;
	case EV_RELOAD_L:
		DEBUGNAME("EV_RELOAD_L");
		//Reloading left weapon
		{
			weaponInfo_t *weaponInfo;
			int weapon = es->eventParm;

			if(weapon < 0 || weapon >= WP_NUM_WEAPONS)
			{
				Com_Error(ERR_DISCONNECT, "Attempted reloading weapon %i (nonexistant)", weapon);
				break;
			}

			cg.reloadingRight = qfalse;
			cg.reloadingBothGuns = qfalse;
			cg.reloadingLeft = qtrue;

			weaponInfo = &cg_weapons[weapon];

			trap_S_StartSound( NULL, cent->currentState.number, CHAN_AUTO, weaponInfo->selectSound ? weaponInfo->selectSound : cgs.media.selectSound );

			cg.reloadTimeLeftStart = cg.time;
			cg.reloadTimeLeftDuration = es->time;
		}
		break;
	case EV_RECEIVE_AKIMBO:
		DEBUGNAME("EV_RECEIVE_AKIMBO");
		if(cent->currentState.number == cg.clientNum)
		{
			cg.akimboGuns[cg.predictedPlayerState.weapon] = es->eventParm;
		}
		break;
#endif
	case EV_FIRE_WEAPON:
		DEBUGNAME("EV_FIRE_WEAPON");
		if (cent->currentState.number >= MAX_CLIENTS && cent->currentState.eType != ET_NPC)
		{ //special case for turret firing
			vec3_t gunpoint, gunangle;
			mdxaBone_t matrix;

			weaponInfo_t *weaponInfo = &cg_weapons[WP_TURRET];

			if ( !weaponInfo->registered )
			{
				CG_RegisterWeapon(WP_TURRET);
			}

			if (cent->ghoul2)
			{
				if (!cent->bolt1)
				{
					cent->bolt1 = trap_G2API_AddBolt(cent->ghoul2, 0, "*flash01");
				}
				if (!cent->bolt2)
				{
					cent->bolt2 = trap_G2API_AddBolt(cent->ghoul2, 0, "*flash02");
				}
				trap_G2API_SetBoneAnim(cent->ghoul2, 0, "Bone02", 1, 4, BONE_ANIM_OVERRIDE_FREEZE|BONE_ANIM_BLEND,
					1.0f, cg.time, -1, 300);
			}
			else
			{
				break;
			}

			if (cent->currentState.eventParm)
			{
				trap_G2API_GetBoltMatrix(cent->ghoul2, 0, cent->bolt2, &matrix, cent->currentState.angles, cent->currentState.origin, cg.time, cgs.gameModels, cent->modelScale);
			}
			else
			{
				trap_G2API_GetBoltMatrix(cent->ghoul2, 0, cent->bolt1, &matrix, cent->currentState.angles, cent->currentState.origin, cg.time, cgs.gameModels, cent->modelScale);
			}

			gunpoint[0] = matrix.matrix[0][3];
			gunpoint[1] = matrix.matrix[1][3];
			gunpoint[2] = matrix.matrix[2][3];

			gunangle[0] = -matrix.matrix[0][0];
			gunangle[1] = -matrix.matrix[1][0];
			gunangle[2] = -matrix.matrix[2][0];

			trap_FX_PlayEffectID(cgs.effects.mEmplacedMuzzleFlash, gunpoint, gunangle, -1, -1);
		}
		else if (cent->currentState.weapon != WP_EMPLACED_GUN || cent->currentState.eType == ET_NPC)
		{
			if (cent->currentState.eType == ET_NPC &&
				cent->currentState.NPC_class == CLASS_VEHICLE &&
				cent->m_pVehicle)
			{ //vehicles do nothing for clientside weapon fire events.. at least for now.
				break;
			}
#ifdef AKIMBO
			CG_FireWeapon( cent, qfalse, qfalse );
#else
			CG_FireWeapon( cent, qfalse );
#endif
		}
		break;

	case EV_ALT_FIRE:
		DEBUGNAME("EV_ALT_FIRE");

		if (cent->currentState.weapon == WP_EMPLACED_GUN)
		{ //don't do anything for emplaced stuff
			break;
		}

		if (cent->currentState.eType == ET_NPC &&
			cent->currentState.NPC_class == CLASS_VEHICLE &&
			cent->m_pVehicle)
		{ //vehicles do nothing for clientside weapon fire events.. at least for now.
			break;
		}
#ifdef AKIMBO
		CG_FireWeapon( cent, qtrue, qfalse );
#else
		CG_FireWeapon( cent, qtrue );
#endif

		//if you just exploded your detpacks and you have no ammo left for them, autoswitch
		if ( cg.snap->ps.clientNum == cent->currentState.number &&
			cg.snap->ps.weapon == WP_DET_PACK )
		{
			if (cg.snap->ps.ammo[weaponData[WP_DET_PACK].ammoIndex] == 0) 
			{
				CG_OutOfAmmoChange(WP_DET_PACK);
			}
		}
		break;
#ifdef AKIMBO
	case EV_FIRE_LEFT_WEAPON:
		DEBUGNAME("EV_FIRE_LEFT_WEAPON");
		{
			if(cent->currentState.weapon2 == WP_NONE)
			{
				//Invalid!
				break;
			}
			CG_FireWeapon(cent, qfalse, qtrue);
		}
		CG_AddRadarEnt(cent);
		break;
	case EV_FIRE_BOTH_GUNS:
		DEBUGNAME("EV_FIRE_BOTH_GUNS");
		{
			if(cent->currentState.weapon2 == WP_NONE || cent->currentState.weapon == WP_NONE)
			{
				//Invalid!
				break;
			}
			if (cent->currentState.weapon == WP_EMPLACED_GUN)
			{ //don't do anything for emplaced stuff
				break;
			}
			CG_FireWeapon(cent, qfalse, qtrue);
			CG_FireWeapon(cent, qfalse, qfalse);
		}
		break;
#endif

	case EV_SABER_ATTACK:
		DEBUGNAME("EV_SABER_ATTACK");
		{
			qhandle_t swingSound = trap_S_RegisterSound(va("sound/weapons/saber/saberhup%i.wav", Q_irand(1, 8)));
			clientInfo_t *client = NULL;
			if ( cg_entities[es->number].currentState.eType == ET_NPC )
			{
				client = cg_entities[es->number].npcClient;
			}
			else if ( es->number < MAX_CLIENTS )
			{
				client = &cgs.clientinfo[es->number];
			}
			if ( client && client->infoValid && client->saber[0].swingSound[0] )
			{//custom swing sound
				swingSound = client->saber[0].swingSound[Q_irand(0,2)];
			}
            trap_S_StartSound(es->pos.trBase, es->number, CHAN_WEAPON, swingSound );
		}
		break;

	case EV_SABER_HIT:
		DEBUGNAME("EV_SABER_HIT");
		{
			int hitPersonFxID = cgs.effects.mSaberBloodSparks;
			int hitPersonSmallFxID = cgs.effects.mSaberBloodSparksSmall;
			int hitPersonMidFxID = cgs.effects.mSaberBloodSparksMid;
			int hitOtherFxID = cgs.effects.mSaberCut;
			int hitSound = trap_S_RegisterSound(va("sound/weapons/saber/saberhit%i.wav", Q_irand(1, 3)));
			
			if ( es->otherEntityNum2 >= 0
				&& es->otherEntityNum2 < ENTITYNUM_NONE )
			{//we have a specific person who is causing this effect, see if we should override it with any custom saber effects/sounds
				clientInfo_t *client = NULL;
				if ( cg_entities[es->otherEntityNum2].currentState.eType == ET_NPC )
				{
					client = cg_entities[es->otherEntityNum2].npcClient;
				}
				else if ( es->otherEntityNum2 < MAX_CLIENTS )
				{
					client = &cgs.clientinfo[es->otherEntityNum2];
				}
				if ( client && client->infoValid )
				{
					int saberNum = es->weapon;
					int bladeNum = es->legsAnim;
					if ( WP_SaberBladeUseSecondBladeStyle( &client->saber[saberNum], bladeNum ) )
					{//use second blade style values
						if ( client->saber[saberNum].hitPersonEffect2 )
						{//custom hit person effect
							hitPersonFxID = hitPersonSmallFxID = hitPersonMidFxID = client->saber[saberNum].hitPersonEffect2;
						}
						if ( client->saber[saberNum].hitOtherEffect2 )
						{//custom hit other effect
							hitOtherFxID = client->saber[saberNum].hitOtherEffect2;
						}
						if ( client->saber[saberNum].hit2Sound[0] )
						{//custom hit sound
							hitSound = client->saber[saberNum].hit2Sound[Q_irand(0,2)];
						}
					}
					else
					{//use first blade style values
						if ( client->saber[saberNum].hitPersonEffect )
						{//custom hit person effect
							hitPersonFxID = hitPersonSmallFxID = hitPersonMidFxID = client->saber[saberNum].hitPersonEffect;
						}
						if ( client->saber[saberNum].hitOtherEffect )
						{//custom hit other effect
							hitOtherFxID = client->saber[0].hitOtherEffect;
						}
						if ( client->saber[saberNum].hitSound[0] )
						{//custom hit sound
							hitSound = client->saber[saberNum].hitSound[Q_irand(0,2)];
						}
					}
				}
			}

			if (es->eventParm == 16)
			{ //Make lots of sparks, something special happened
				vec3_t fxDir;
				VectorCopy(es->angles, fxDir);
				if (!fxDir[0] && !fxDir[1] && !fxDir[2])
				{
					fxDir[1] = 1;
				}
				trap_S_StartSound(es->origin, es->number, CHAN_AUTO, hitSound );
				trap_FX_PlayEffectID( hitPersonFxID, es->origin, fxDir, -1, -1 );
				trap_FX_PlayEffectID( hitPersonFxID, es->origin, fxDir, -1, -1 );
				trap_FX_PlayEffectID( hitPersonFxID, es->origin, fxDir, -1, -1 );
				trap_FX_PlayEffectID( hitPersonFxID, es->origin, fxDir, -1, -1 );
				trap_FX_PlayEffectID( hitPersonFxID, es->origin, fxDir, -1, -1 );
				trap_FX_PlayEffectID( hitPersonFxID, es->origin, fxDir, -1, -1 );
			}
			else if (es->eventParm)
			{ //hit a person
				vec3_t fxDir;
				VectorCopy(es->angles, fxDir);
				if (!fxDir[0] && !fxDir[1] && !fxDir[2])
				{
					fxDir[1] = 1;
				}
				trap_S_StartSound(es->origin, es->number, CHAN_AUTO, hitSound );
				if ( es->eventParm == 3 )
				{	// moderate or big hits.
					trap_FX_PlayEffectID( hitPersonSmallFxID, es->origin, fxDir, -1, -1 );
				}
				else if ( es->eventParm == 2 )
				{	// this is for really big hits.
					trap_FX_PlayEffectID( hitPersonMidFxID, es->origin, fxDir, -1, -1 );
				}
				else
				{	// this should really just be done in the effect itself, no?
					trap_FX_PlayEffectID( hitPersonFxID, es->origin, fxDir, -1, -1 );
					trap_FX_PlayEffectID( hitPersonFxID, es->origin, fxDir, -1, -1 );
					trap_FX_PlayEffectID( hitPersonFxID, es->origin, fxDir, -1, -1 );
				}
			}
			else
			{ //hit something else
				vec3_t fxDir;
				VectorCopy(es->angles, fxDir);
				if (!fxDir[0] && !fxDir[1] && !fxDir[2])
				{
					fxDir[1] = 1;
				}
				//old jk2mp method
				/*
				trap_S_StartSound(es->origin, es->number, CHAN_AUTO, trap_S_RegisterSound("sound/weapons/saber/saberhit.wav"));
				trap_FX_PlayEffectID( trap_FX_RegisterEffect("saber/spark.efx"), es->origin, fxDir, -1, -1 );
				*/

				trap_FX_PlayEffectID( hitOtherFxID, es->origin, fxDir, -1, -1 );
			}

			//rww - this means we have the number of the ent being hit and the ent that owns the saber doing
			//the hit. This being the case, we can store these indecies and the current time in order to do
			//some visual tricks on the client between frames to make it look like we're actually continuing
			//to hit between server frames.
			if (es->otherEntityNum != ENTITYNUM_NONE && es->otherEntityNum2 != ENTITYNUM_NONE)
			{
				centity_t *saberOwner;

				saberOwner = &cg_entities[es->otherEntityNum2];

				saberOwner->serverSaberHitIndex = es->otherEntityNum;
				saberOwner->serverSaberHitTime = cg.time;

				if (es->eventParm)
				{
					saberOwner->serverSaberFleshImpact = qtrue;
				}
				else
				{
					saberOwner->serverSaberFleshImpact = qfalse;
				}
			}
		}
		break;

	case EV_SABER_BLOCK:
		DEBUGNAME("EV_SABER_BLOCK");
		{
			if (es->eventParm)
			{ //saber block
				qboolean cullPass = qfalse;
				int			blockFXID = cgs.effects.mSaberBlock;
				qhandle_t	blockSound = trap_S_RegisterSound(va( "sound/weapons/saber/saberblock%d.wav", Q_irand(1, 9) ));
				qboolean	noFlare = qfalse;

				if ( es->otherEntityNum2 >= 0
					&& es->otherEntityNum2 < ENTITYNUM_NONE )
				{//we have a specific person who is causing this effect, see if we should override it with any custom saber effects/sounds
					clientInfo_t *client = NULL;
					if ( cg_entities[es->otherEntityNum2].currentState.eType == ET_NPC )
					{
						client = cg_entities[es->otherEntityNum2].npcClient;
					}
					else if ( es->otherEntityNum2 < MAX_CLIENTS )
					{
						client = &cgs.clientinfo[es->otherEntityNum2];
					}
					if ( client && client->infoValid )
					{
						int saberNum = es->weapon;
						int bladeNum = es->legsAnim;
						if ( WP_SaberBladeUseSecondBladeStyle( &client->saber[saberNum], bladeNum ) )
						{//use second blade style values
							if ( client->saber[saberNum].blockEffect2 )
							{//custom saber block effect
								blockFXID = client->saber[saberNum].blockEffect2;
							}
							if ( client->saber[saberNum].block2Sound[0] )
							{//custom hit sound
								blockSound = client->saber[saberNum].block2Sound[Q_irand(0,2)];
							}
						}
						else
						{
							if ( client->saber[saberNum].blockEffect )
							{//custom saber block effect
								blockFXID = client->saber[saberNum].blockEffect;
							}
							if ( client->saber[saberNum].blockSound[0] )
							{//custom hit sound
								blockSound = client->saber[saberNum].blockSound[Q_irand(0,2)];
							}
						}
						if ( (client->saber[saberNum].saberFlags2&SFL2_NO_CLASH_FLARE) )
						{
							noFlare = qtrue;
						}
					}
				}
				if (cg.mInRMG)
				{
					trace_t tr;
					vec3_t vecSub;

					VectorSubtract(cg.refdef.vieworg, es->origin, vecSub);

					if (VectorLength(vecSub) < 5000)
					{
						CG_Trace(&tr, cg.refdef.vieworg, NULL, NULL, es->origin, ENTITYNUM_NONE, CONTENTS_TERRAIN|CONTENTS_SOLID);

						if (tr.fraction == 1.0 || tr.entityNum < MAX_CLIENTS)
						{
							cullPass = qtrue;
						}
					}
				}
				else
				{
					cullPass = qtrue;
				}

				if (cullPass)
				{
					vec3_t fxDir;

					VectorCopy(es->angles, fxDir);
					if (!fxDir[0] && !fxDir[1] && !fxDir[2])
					{
						fxDir[1] = 1;
					}
					trap_S_StartSound(es->origin, es->number, CHAN_AUTO, blockSound );
					trap_FX_PlayEffectID( blockFXID, es->origin, fxDir, -1, -1 );

					if ( !noFlare )
					{
						cg_saberFlashTime = cg.time-50;
						VectorCopy( es->origin, cg_saberFlashPos );
					}
				}
			}
			else
			{ //projectile block
				vec3_t fxDir;
				VectorCopy(es->angles, fxDir);
				if (!fxDir[0] && !fxDir[1] && !fxDir[2])
				{
					fxDir[1] = 1;
				}
				trap_FX_PlayEffectID(cgs.effects.mBlasterDeflect, es->origin, fxDir, -1, -1);
			}
		}
		break;

	case EV_SABER_CLASHFLARE:
		DEBUGNAME("EV_SABER_CLASHFLARE");
		{
			qboolean cullPass = qfalse;

			if (cg.mInRMG)
			{
				trace_t tr;
				vec3_t vecSub;

				VectorSubtract(cg.refdef.vieworg, es->origin, vecSub);

				if (VectorLength(vecSub) < 5000)
				{
					CG_Trace(&tr, cg.refdef.vieworg, NULL, NULL, es->origin, ENTITYNUM_NONE, CONTENTS_TERRAIN|CONTENTS_SOLID);

					if (tr.fraction == 1.0 || tr.entityNum < MAX_CLIENTS)
					{
						cullPass = qtrue;
					}
				}
			}
			else
			{
				cullPass = qtrue;
			}

			if (cullPass)
			{
				cg_saberFlashTime = cg.time-50;
				VectorCopy( es->origin, cg_saberFlashPos );
			}
			trap_S_StartSound ( es->origin, -1, CHAN_WEAPON, trap_S_RegisterSound( va("sound/weapons/saber/saberhitwall%i", Q_irand(1, 3)) ) );
		}
		break;

	case EV_SABER_UNHOLSTER:
		DEBUGNAME("EV_SABER_UNHOLSTER");
		{
			clientInfo_t *ci = NULL;

			if (es->eType == ET_NPC)
			{
				ci = cg_entities[es->number].npcClient;
			}
			else if (es->number < MAX_CLIENTS)
			{
				ci = &cgs.clientinfo[es->number];
			}

			if (ci)
			{
				if (ci->saber[0].soundOn)
				{
					trap_S_StartSound (NULL, es->number, CHAN_AUTO, ci->saber[0].soundOn );
				}
				if (ci->saber[1].soundOn)
				{
					trap_S_StartSound (NULL, es->number, CHAN_AUTO, ci->saber[1].soundOn );
				}
			}
		}
		break;

	case EV_BECOME_JEDIMASTER:
		DEBUGNAME("EV_SABER_UNHOLSTER");
		{
			trace_t tr;
			vec3_t playerMins = {-15, -15, DEFAULT_MINS_2+8};
			vec3_t playerMaxs = {15, 15, DEFAULT_MAXS_2};
			vec3_t ang, pos, dpos;

			VectorClear(ang);
			ang[ROLL] = 1;

			VectorCopy(position, dpos);
			dpos[2] -= 4096;

			CG_Trace(&tr, position, playerMins, playerMaxs, dpos, es->number, MASK_SOLID);
			VectorCopy(tr.endpos, pos);
			
			if (tr.fraction == 1)
			{
				break;
			}
			trap_FX_PlayEffectID(cgs.effects.mJediSpawn, pos, ang, -1, -1);

			trap_S_StartSound (NULL, es->number, CHAN_AUTO, trap_S_RegisterSound( "sound/weapons/saber/saberon.wav" ) );

			if (cg.snap->ps.clientNum == es->number)
			{
				trap_S_StartLocalSound(cgs.media.happyMusic, CHAN_LOCAL);
				CGCam_SetMusicMult(0.3, 5000);
			}
		}
		CG_EVBox_AddString("The katana has been retrieved.");
		es->eventParm = GTS_NEW_JEDIMASTER;
		break;

	case EV_DISRUPTOR_MAIN_SHOT:
		DEBUGNAME("EV_DISRUPTOR_MAIN_SHOT");
		{
			vec3_t	direction;
			CG_GetClientWeaponMuzzleBoltPoint(cent->currentState.eventParm, cent->currentState.origin2);
			VectorSubtract( cent->lerpOrigin, cent->currentState.origin2, direction);
			VectorNormalize(direction);
			trap_FX_PlayEffectID( cgs.effects.bowcasterShotEffect, cent->currentState.origin2, direction, -1, -1);
		}
		//FX_DisruptorMainShot( cent->currentState.origin2, cent->lerpOrigin ); 
		break;

	case EV_DISRUPTOR_SNIPER_SHOT:
		DEBUGNAME("EV_DISRUPTOR_SNIPER_SHOT");
		if (cent->currentState.eventParm != cg.snap->ps.clientNum ||
			cg.renderingThirdPerson)
		{ //h4q3ry
			CG_GetClientWeaponMuzzleBoltPoint(cent->currentState.eventParm, cent->currentState.origin2);
		}
		else
		{
			if (cg.lastFPFlashPoint[0] ||cg.lastFPFlashPoint[1] || cg.lastFPFlashPoint[2])
			{ //get the position of the muzzle flash for the first person weapon model from the last frame
				VectorCopy(cg.lastFPFlashPoint, cent->currentState.origin2);
			}
		}
		FX_DisruptorAltShot( cent->currentState.origin2, cent->lerpOrigin, cent->currentState.shouldtarget );
		break;

	case EV_DISRUPTOR_SNIPER_MISS:
		DEBUGNAME("EV_DISRUPTOR_SNIPER_MISS");
		ByteToDir( es->eventParm, dir );
		if (es->weapon)
		{ //primary
			FX_DisruptorHitWall( cent->lerpOrigin, dir );
		}
		else
		{ //secondary
			FX_DisruptorAltMiss( cent->lerpOrigin, dir );
		}
		break;

	case EV_DISRUPTOR_HIT:
		DEBUGNAME("EV_DISRUPTOR_HIT");
		ByteToDir( es->eventParm, dir );
		if (es->weapon)
		{ //client
			FX_DisruptorHitPlayer( cent->lerpOrigin, dir, qtrue );
		}
		else
		{ //non-client
			FX_DisruptorHitWall( cent->lerpOrigin, dir );
		}
		break;

	case EV_DISRUPTOR_ZOOMSOUND:
		DEBUGNAME("EV_DISRUPTOR_ZOOMSOUND");
		if (es->number == cg.snap->ps.clientNum)
		{
			if (cg.snap->ps.zoomMode)
			{
				trap_S_StartLocalSound(trap_S_RegisterSound("sound/weapons/disruptor/zoomstart.wav"), CHAN_AUTO);
			}
			else
			{
				cg.zoomTime = cg.time + weaponData[cg.snap->ps.weapon].ADSTime + 50;
				cg.zoomStartTime = cg.time;
				//trap_S_StartLocalSound(trap_S_RegisterSound("sound/weapons/disruptor/zoomend.wav"), CHAN_AUTO);
			}
		}
		break;
	case EV_PREDEFSOUND:
		DEBUGNAME("EV_PREDEFSOUND");
		{
			int sID = -1;

			switch (es->eventParm)
			{
			case PDSOUND_PROTECTHIT:
				sID = trap_S_RegisterSound("sound/weapons/force/protecthit.mp3");
				break;
			case PDSOUND_PROTECT:
				sID = trap_S_RegisterSound("sound/weapons/force/protect.mp3");
				break;
			case PDSOUND_ABSORBHIT:
				sID = trap_S_RegisterSound("sound/weapons/force/absorbhit.mp3");
				if (es->trickedentindex >= 0 && es->trickedentindex < MAX_CLIENTS)
				{
					int clnum = es->trickedentindex;

					cg_entities[clnum].teamPowerEffectTime = cg.time + 1000;
					cg_entities[clnum].teamPowerType = 3;
				}
				break;
			case PDSOUND_ABSORB:
				sID = trap_S_RegisterSound("sound/weapons/force/absorb.mp3");
				break;
			case PDSOUND_FORCEJUMP:
				sID = trap_S_RegisterSound("sound/weapons/force/jump.mp3");
				break;
			case PDSOUND_FORCEGRIP:
				sID = trap_S_RegisterSound("sound/weapons/force/grip.mp3");
				break;
			default:
				break;
			}

			if (sID != 1)
			{
				trap_S_StartSound(es->origin, es->number, CHAN_AUTO, sID);
			}
		}
		break;

	case EV_TEAM_POWER:
		DEBUGNAME("EV_TEAM_POWER");
		{
			int clnum = 0;

			while (clnum < MAX_CLIENTS)
			{
				if (CG_InClientBitflags(es, clnum))
				{
					if (es->eventParm == 1)
					{ //eventParm 1 is heal
						trap_S_StartSound (NULL, clnum, CHAN_AUTO, cgs.media.teamHealSound );
						cg_entities[clnum].teamPowerEffectTime = cg.time + 1000;
						cg_entities[clnum].teamPowerType = 1;
					}
					else
					{ //eventParm 2 is force regen
						trap_S_StartSound (NULL, clnum, CHAN_AUTO, cgs.media.teamRegenSound );
						cg_entities[clnum].teamPowerEffectTime = cg.time + 1000;
						cg_entities[clnum].teamPowerType = 0;
					}
				}
				clnum++;
			}
		}
		break;

	case EV_SCREENSHAKE:
		DEBUGNAME("EV_SCREENSHAKE");
		if (!es->modelindex || cg.predictedPlayerState.clientNum == es->modelindex-1)
		{
			CGCam_Shake(es->angles[0], es->time);
		}
		break;
	case EV_LOCALTIMER:
		DEBUGNAME("EV_LOCALTIMER");
		if (es->owner == cg.predictedPlayerState.clientNum)
		{
			CG_LocalTimingBar(es->time, es->time2);
		}
		break;
	case EV_USE_ITEM0:
		DEBUGNAME("EV_USE_ITEM0");
		CG_UseItem( cent );
		break;
	case EV_USE_ITEM1:
		DEBUGNAME("EV_USE_ITEM1");
		CG_UseItem( cent );
		break;
	case EV_USE_ITEM2:
		DEBUGNAME("EV_USE_ITEM2");
		CG_UseItem( cent );
		break;
	case EV_USE_ITEM3:
		DEBUGNAME("EV_USE_ITEM3");
		CG_UseItem( cent );
		break;
	case EV_USE_ITEM4:
		DEBUGNAME("EV_USE_ITEM4");
		CG_UseItem( cent );
		break;
	case EV_USE_ITEM5:
		DEBUGNAME("EV_USE_ITEM5");
		CG_UseItem( cent );
		break;
	case EV_USE_ITEM6:
		DEBUGNAME("EV_USE_ITEM6");
		CG_UseItem( cent );
		break;
	case EV_USE_ITEM7:
		DEBUGNAME("EV_USE_ITEM7");
		CG_UseItem( cent );
		break;
	case EV_USE_ITEM8:
		DEBUGNAME("EV_USE_ITEM8");
		CG_UseItem( cent );
		break;
	case EV_USE_ITEM9:
		DEBUGNAME("EV_USE_ITEM9");
		CG_UseItem( cent );
		break;
	case EV_USE_ITEM10:
		DEBUGNAME("EV_USE_ITEM10");
		CG_UseItem( cent );
		break;
	case EV_USE_ITEM11:
		DEBUGNAME("EV_USE_ITEM11");
		CG_UseItem( cent );
		break;
	case EV_USE_ITEM12:
		DEBUGNAME("EV_USE_ITEM12");
		CG_UseItem( cent );
		break;
	case EV_USE_ITEM13:
		DEBUGNAME("EV_USE_ITEM13");
		CG_UseItem( cent );
		break;
	case EV_USE_ITEM14:
		DEBUGNAME("EV_USE_ITEM14");
		CG_UseItem( cent );
		break;

	case EV_ITEMUSEFAIL:
		DEBUGNAME("EV_ITEMUSEFAIL");
		if (cg.snap->ps.clientNum == es->number)
		{
			char *psStringEDRef = NULL;

			switch(es->eventParm)
			{
			case SENTRY_NOROOM:
				psStringEDRef = (char *)CG_GetStringEdString("MP_INGAME", "SENTRY_NOROOM");
				break;
			case SENTRY_ALREADYPLACED:
				psStringEDRef = (char *)CG_GetStringEdString("MP_INGAME", "SENTRY_ALREADYPLACED");
				break;
			case SHIELD_NOROOM:
				psStringEDRef = (char *)CG_GetStringEdString("MP_INGAME", "SHIELD_NOROOM");
				break;
			case SEEKER_ALREADYDEPLOYED:
				psStringEDRef = (char *)CG_GetStringEdString("MP_INGAME", "SEEKER_ALREADYDEPLOYED");
				break;
			default:
				break;
			}

			if (!psStringEDRef)
			{
				break;
			}

			Com_Printf("%s\n", psStringEDRef);
		}
		break;

	//=================================================================

	//
	// other events
	//
	case EV_PLAYER_TELEPORT_IN:
		DEBUGNAME("EV_PLAYER_TELEPORT_IN");
		{
			trace_t tr;
			vec3_t playerMins = {-15, -15, DEFAULT_MINS_2+8};
			vec3_t playerMaxs = {15, 15, DEFAULT_MAXS_2};
			vec3_t ang, pos, dpos;

			VectorClear(ang);
			ang[ROLL] = 1;

			VectorCopy(position, dpos);
			dpos[2] -= 4096;

			CG_Trace(&tr, position, playerMins, playerMaxs, dpos, es->number, MASK_SOLID);
			VectorCopy(tr.endpos, pos);
			
			trap_S_StartSound (NULL, es->number, CHAN_AUTO, cgs.media.teleInSound );

			if (tr.fraction == 1)
			{
				break;
			}
			trap_FX_PlayEffectID(cgs.effects.mSpawn, pos, ang, -1, -1);
		}
		break;

	case EV_PLAYER_TELEPORT_OUT:
		DEBUGNAME("EV_PLAYER_TELEPORT_OUT");
		{
			trace_t tr;
			vec3_t playerMins = {-15, -15, DEFAULT_MINS_2+8};
			vec3_t playerMaxs = {15, 15, DEFAULT_MAXS_2};
			vec3_t ang, pos, dpos;

			VectorClear(ang);
			ang[ROLL] = 1;

			VectorCopy(position, dpos);
			dpos[2] -= 4096;

			CG_Trace(&tr, position, playerMins, playerMaxs, dpos, es->number, MASK_SOLID);
			VectorCopy(tr.endpos, pos);

			trap_S_StartSound (NULL, es->number, CHAN_AUTO, cgs.media.teleOutSound );

			if (tr.fraction == 1)
			{
				break;
			}
			trap_FX_PlayEffectID(cgs.effects.mSpawn, pos, ang, -1, -1);
		}
		break;

	case EV_ITEM_POP:
		DEBUGNAME("EV_ITEM_POP");
		trap_S_StartSound (NULL, es->number, CHAN_AUTO, cgs.media.respawnSound );
		break;
	case EV_ITEM_RESPAWN:
		DEBUGNAME("EV_ITEM_RESPAWN");
		cent->miscTime = cg.time;	// scale up from this
		trap_S_StartSound (NULL, es->number, CHAN_AUTO, cgs.media.respawnSound );
		break;

	case EV_GRENADE_BOUNCE:
		DEBUGNAME("EV_GRENADE_BOUNCE");
		//Do something here?
		break;

	case EV_SCOREPLUM:
		DEBUGNAME("EV_SCOREPLUM");
		CG_ScorePlum( cent->currentState.otherEntityNum, cent->lerpOrigin, cent->currentState.time );
		break;

	case EV_CTFMESSAGE:
		DEBUGNAME("EV_CTFMESSAGE");
		CG_GetCTFMessageEvent(es);
		break;

	case EV_BODYFADE:
		if (es->eType != ET_BODY)
		{
			assert(!"EV_BODYFADE event from a non-corpse");
			break;
		}
		
		if (cent->ghoul2 && trap_G2_HaveWeGhoul2Models(cent->ghoul2))
		{
			//turn the inside of the face off, to avoid showing the mouth when we start alpha fading the corpse
			trap_G2API_SetSurfaceOnOff( cent->ghoul2, "head_eyes_mouth", 0x00000002/*G2SURFACEFLAG_OFF*/ );
		}

		cent->bodyFadeTime = cg.time + 60000;
		break;

	//
	// siege gameplay events
	//
	case EV_SIEGE_ROUNDOVER:
		DEBUGNAME("EV_SIEGE_ROUNDOVER");
		CG_SiegeRoundOver(&cg_entities[cent->currentState.weapon], cent->currentState.eventParm);
		break;
	case EV_SIEGE_OBJECTIVECOMPLETE:
		DEBUGNAME("EV_SIEGE_OBJECTIVECOMPLETE");
		CG_SiegeObjectiveCompleted(&cg_entities[cent->currentState.weapon], cent->currentState.eventParm, cent->currentState.trickedentindex);
		break;

	case EV_DESTROY_GHOUL2_INSTANCE:
		DEBUGNAME("EV_DESTROY_GHOUL2_INSTANCE");
		if (cg_entities[es->eventParm].ghoul2 && trap_G2_HaveWeGhoul2Models(cg_entities[es->eventParm].ghoul2))
		{
			if (es->eventParm < MAX_CLIENTS)
			{ //You try to do very bad thing!
#ifdef _DEBUG
				Com_Printf("WARNING: Tried to kill a client ghoul2 instance with a server event!\n");
#endif
				break;
			}
			trap_G2API_CleanGhoul2Models(&(cg_entities[es->eventParm].ghoul2));
		}
		break;

	case EV_DESTROY_WEAPON_MODEL:
		DEBUGNAME("EV_DESTROY_WEAPON_MODEL");
		if (cg_entities[es->eventParm].ghoul2 && trap_G2_HaveWeGhoul2Models(cg_entities[es->eventParm].ghoul2) &&
			trap_G2API_HasGhoul2ModelOnIndex(&(cg_entities[es->eventParm].ghoul2), 1))
		{
			trap_G2API_RemoveGhoul2Model(&(cg_entities[es->eventParm].ghoul2), 1);
		}
		break;

	case EV_GIVE_NEW_RANK:
		DEBUGNAME("EV_GIVE_NEW_RANK");
		if (es->trickedentindex == cg.snap->ps.clientNum)
		{
			trap_Cvar_Set("ui_rankChange", va("%i", es->eventParm));

			trap_Cvar_Set("ui_myteam", va("%i", es->bolt2));

			if (!( trap_Key_GetCatcher() & KEYCATCH_UI ) && !es->bolt1)
			{
				trap_OpenUIMenu(UIMENU_PLAYERCONFIG);
			}
		}
		break;

	case EV_SET_FREE_SABER:
		DEBUGNAME("EV_SET_FREE_SABER");

		trap_Cvar_Set("ui_freeSaber", va("%i", es->eventParm));
		break;

	case EV_SET_FORCE_DISABLE:
		DEBUGNAME("EV_SET_FORCE_DISABLE");

		trap_Cvar_Set("ui_forcePowerDisable", va("%i", es->eventParm));
		break;

	//
	// missile impacts
	//
	case EV_CONC_ALT_IMPACT:
		DEBUGNAME("EV_CONC_ALT_IMPACT");
		{
			float dist;
			float shotDist = VectorNormalize(es->angles);
			vec3_t spot;

			for (dist = 0.0f; dist < shotDist; dist += 64.0f)
			{ //one effect would be.. a whole lot better
				VectorMA( es->origin2, dist, es->angles, spot );
                trap_FX_PlayEffectID(cgs.effects.mConcussionAltRing, spot, es->angles2, -1, -1);
			}

			ByteToDir( es->eventParm, dir );
			CG_MissileHitWall(WP_CONCUSSION, es->owner, position, dir, IMPACTSOUND_DEFAULT, qfalse, 0, MATERIAL_NONE);

			FX_ConcAltShot(es->origin2, spot);

			//steal the bezier effect from the disruptor
			FX_DisruptorAltMiss(position, dir);
		}
		break;

	case EV_MISSILE_STICK:
		DEBUGNAME("EV_MISSILE_STICK");
//		trap_S_StartSound (NULL, es->number, CHAN_AUTO, cgs.media.missileStick );
		break;

	case EV_MISSILE_HIT:
		DEBUGNAME("EV_MISSILE_HIT");
		ByteToDir( es->eventParm, dir );
		if ( es->emplacedOwner )
		{//hack: this is an index to a custom effect to use
			trap_FX_PlayEffectID(cgs.gameEffects[es->emplacedOwner], position, dir, -1, -1);
		}
		else if ( CG_VehicleWeaponImpact( cent ) )
		{//a vehicle missile that uses an overridden impact effect...
		}
		else if (cent->currentState.eFlags & EF_ALT_FIRING)
		{
			CG_MissileHitPlayer( es->weapon, position, dir, es->otherEntityNum, qtrue);
		}
		else
		{
			CG_MissileHitPlayer( es->weapon, position, dir, es->otherEntityNum, qfalse);
		}

		if (cg_ghoul2Marks.integer &&
			es->trickedentindex)
		{ //flag to place a ghoul2 mark
			CG_G2MarkEvent(es);
		}
		break;

	case EV_MISSILE_MISS:
		DEBUGNAME("EV_MISSILE_MISS");
		ByteToDir( es->eventParm, dir );
		if ( es->emplacedOwner )
		{//hack: this is an index to a custom effect to use
			trap_FX_PlayEffectID(cgs.gameEffects[es->emplacedOwner], position, dir, -1, -1);
		}
		else if ( CG_VehicleWeaponImpact( cent ) )
		{//a vehicle missile that used an overridden impact effect...
		}
		else if (es->eFlags & EF_ALT_FIRING)
		{
			CG_MissileHitWall(es->weapon, 0, position, dir, IMPACTSOUND_DEFAULT, qtrue, es->generic1, es->generic1);
		}
		else
		{
			CG_MissileHitWall(es->weapon, 0, position, dir, IMPACTSOUND_DEFAULT, qfalse, 0, es->generic1); //tl;dr version: I'm lazy and don't want to change netf_overrides.txt
		}

		if (cg_ghoul2Marks.integer &&
			es->trickedentindex)
		{ //flag to place a ghoul2 mark
			CG_G2MarkEvent(es);
		}
		break;

	case EV_MISSILE_MISS_METAL:
		DEBUGNAME("EV_MISSILE_MISS_METAL");
		ByteToDir( es->eventParm, dir );
		if ( es->emplacedOwner )
		{//hack: this is an index to a custom effect to use
			trap_FX_PlayEffectID(cgs.gameEffects[es->emplacedOwner], position, dir, -1, -1);
		}
		else if ( CG_VehicleWeaponImpact( cent ) )
		{//a vehicle missile that used an overridden impact effect...
		}
		else if (cent->currentState.eFlags & EF_ALT_FIRING)
		{
			CG_MissileHitWall(es->weapon, 0, position, dir, IMPACTSOUND_METAL, qtrue, es->generic1, MATERIAL_HOLLOWMETAL);
		}
		else
		{
			CG_MissileHitWall(es->weapon, 0, position, dir, IMPACTSOUND_METAL, qfalse, 0, MATERIAL_HOLLOWMETAL);
		}
		break;

	case EV_PLAY_EFFECT:
		DEBUGNAME("EV_PLAY_EFFECT");
		switch(es->eventParm)
		{ //it isn't a hack, it's ingenuity!
		case EFFECT_SMOKE:
			eID = cgs.effects.mEmplacedDeadSmoke;
			break;
		case EFFECT_EXPLOSION:
			eID = cgs.effects.mEmplacedExplode;
			break;
		case EFFECT_EXPLOSION_PAS:
			eID = cgs.effects.mTurretExplode;
			break;
		case EFFECT_SPARK_EXPLOSION:
			eID = cgs.effects.mSparkExplosion;
			break;
		case EFFECT_EXPLOSION_TRIPMINE:
			eID = cgs.effects.mTripmineExplosion;
			break;
		case EFFECT_EXPLOSION_DETPACK:
			eID = cgs.effects.mDetpackExplosion;
			break;
		case EFFECT_EXPLOSION_FLECHETTE:
			eID = cgs.effects.mFlechetteAltBlow;
			break;
		case EFFECT_STUNHIT:
			eID = cgs.effects.mStunBatonFleshImpact;
			break;
		case EFFECT_EXPLOSION_DEMP2ALT:
			FX_DEMP2_AltDetonate( cent->lerpOrigin, es->weapon );
			eID = cgs.effects.mAltDetonate;
			break;
		case EFFECT_EXPLOSION_TURRET:
			eID = cgs.effects.mTurretExplode;
			break;
		case EFFECT_SPARKS:
			eID = cgs.effects.mSparksExplodeNoSound;
			break;
		case EFFECT_WATER_SPLASH:
			eID = cgs.effects.waterSplash;
			break;
		case EFFECT_ACID_SPLASH:
			eID = cgs.effects.acidSplash;
			break;
		case EFFECT_LAVA_SPLASH:
			eID = cgs.effects.lavaSplash;
			break;
		case EFFECT_LANDING_MUD:
			eID = cgs.effects.landingMud;
			break;
		case EFFECT_LANDING_SAND:
			eID = cgs.effects.landingSand;
			break;
		case EFFECT_LANDING_DIRT:
			eID = cgs.effects.landingDirt;
			break;
		case EFFECT_LANDING_SNOW:
			eID = cgs.effects.landingSnow;
			break;
		case EFFECT_LANDING_GRAVEL:
			eID = cgs.effects.landingGravel;
			break;
		default:
			eID = -1;
			break;
		}

		if (eID != -1)
		{
			vec3_t fxDir;

			VectorCopy(es->angles, fxDir);

			if (!fxDir[0] && !fxDir[1] && !fxDir[2])
			{
				fxDir[1] = 1;
			}

			trap_FX_PlayEffectID(eID, es->origin, fxDir, -1, -1);
		}
		break;

	//[Bolted effect]
	case EV_PLAY_EFFECT_BOLTED:
		DEBUGNAME("EV_PLAY_EFFECT_BOLTED");
		{
			centity_t *effectOn = &cg_entities[es->owner];

			if( !effectOn->ghoul2 )					// don't play bolted effect if no ghoul on the entity
				break;

			eID = 0;

			//if the effect is already registered go ahead grab it
			if ( cgs.gameEffects[ es->eventParm ] )
				eID = cgs.gameEffects[es->eventParm];
			else
			{
				//else it must be registered before using it
				s = CG_ConfigString( CS_EFFECTS + es->eventParm );
				if (s && s[0])
					eID = trap_FX_RegisterEffect(s);
			}

			if( ( es->bolt1 == -1 ) || !eID )		//we don't have this particular bone or effect so can't play it
				break;

			//attach the effect on the entity
			trap_FX_PlayBoltedEffectID( eID, es->origin, effectOn->ghoul2, 
				es->generic1, es->owner, 0, 0, qtrue);
		}
		break;
	//[/Bolted effect]

	case EV_PLAY_EFFECT_ID:
	case EV_PLAY_PORTAL_EFFECT_ID:
		DEBUGNAME("EV_PLAY_EFFECT_ID");
		{
			vec3_t fxDir;
			qboolean portalEffect = qfalse;
			int efxIndex = 0;

			if (event == EV_PLAY_PORTAL_EFFECT_ID)
			{ //This effect should only be played inside sky portals.
				portalEffect = qtrue;
			}

			AngleVectors(es->angles, fxDir, 0, 0);
			
			if (!fxDir[0] && !fxDir[1] && !fxDir[2])
			{
				fxDir[1] = 1;
			}

			if ( cgs.gameEffects[ es->eventParm ] )
			{
				efxIndex = cgs.gameEffects[es->eventParm];
			}
			else
			{
				s = CG_ConfigString( CS_EFFECTS + es->eventParm );
				if (s && s[0])
				{
					efxIndex = trap_FX_RegisterEffect(s);
				}
			}

			if (efxIndex)
			{
				if (portalEffect)
				{
					trap_FX_PlayPortalEffectID(efxIndex, position, fxDir, -1, -1 );
				}
				else
				{
					trap_FX_PlayEffectID(efxIndex, position, fxDir, -1, -1 );
				}
			}
		}
		break;

	case EV_NEW_DOORSOUND:
		CG_PlayNewDoorSound(cent, es->eventParm);
		break;

	case EV_PLAYDOORSOUND:
		CG_PlayDoorSound(cent, es->eventParm);
		break;
	case EV_PLAYDOORLOOPSOUND:
		CG_PlayDoorLoopSound(cent);
		break;
	case EV_BMODEL_SOUND:
		DEBUGNAME("EV_BMODEL_SOUND");
		{
			sfxHandle_t sfx;
			const char *soundSet;
			
			soundSet = CG_ConfigString( CS_AMBIENT_SET + es->soundSetIndex );

			if (!soundSet || !soundSet[0])
			{
				break;
			}

			sfx = trap_AS_GetBModelSound(soundSet, es->eventParm);

			if (sfx == -1)
			{
				break;
			}

			trap_S_StartSound( NULL, es->number, CHAN_AUTO, sfx );
		}
		break;


	case EV_MUTE_SOUND:
		DEBUGNAME("EV_MUTE_SOUND");
		if (cg_entities[es->trickedentindex2].currentState.eFlags & EF_SOUNDTRACKER)
		{
			cg_entities[es->trickedentindex2].currentState.eFlags -= EF_SOUNDTRACKER;
		}
		trap_S_MuteSound(es->trickedentindex2, es->trickedentindex);
		CG_S_StopLoopingSound(es->trickedentindex2, -1);
		break;

	case EV_VOICECMD_SOUND:
		DEBUGNAME("EV_VOICECMD_SOUND");
		if (es->groundEntityNum >= MAX_CLIENTS)
		{ //don't ever use this unless it is being used on a real client
			break;
		}
		{
			sfxHandle_t sfx = cgs.gameSounds[ es->eventParm ];
			clientInfo_t *ci = &cgs.clientinfo[es->groundEntityNum];
			centity_t *vChatEnt = &cg_entities[es->groundEntityNum];
			char descr[1024];

			strcpy(descr, CG_GetStringForVoiceSound(CG_ConfigString( CS_SOUNDS + es->eventParm )));

			if (!sfx)
			{
				s = CG_ConfigString( CS_SOUNDS + es->eventParm );
				sfx = CG_CustomSound( es->groundEntityNum, s );
			}

			if (sfx)
			{
				if (es->groundEntityNum != cg.predictedPlayerState.clientNum)
				{ //play on the head as well to simulate hearing in radio and in world
					if (ci->team == cg.predictedPlayerState.persistant[PERS_TEAM])
					{ //don't hear it if this person is on the other team, but they can still
						//hear it in the world spot.
						trap_S_StartSound (NULL, cg.snap->ps.clientNum, CHAN_MENU1, sfx);
					}
				}
				if (ci->team == cg.predictedPlayerState.persistant[PERS_TEAM])
				{ //add to the chat box
					//hear it in the world spot.
					char vchatstr[1024];
					strcpy(vchatstr, va("<%s: %s>\n", ci->name, descr));
					CG_Printf(vchatstr);
					CG_ChatBox_AddString(vchatstr);
				}

				//and play in world for everyone
				trap_S_StartSound (NULL, es->groundEntityNum, CHAN_VOICE, sfx);
				vChatEnt->vChatTime = cg.time + 1000;
			}
		}
		break;

	case EV_GENERAL_SOUND:
		DEBUGNAME("EV_GENERAL_SOUND");
		if (es->saberEntityNum == TRACK_CHANNEL_2 || es->saberEntityNum == TRACK_CHANNEL_3 ||
			es->saberEntityNum == TRACK_CHANNEL_5)
		{ //channels 2 and 3 are for speed and rage, 5 for sight
			if ( cgs.gameSounds[ es->eventParm ] )
			{
				CG_S_AddRealLoopingSound(es->number, es->pos.trBase, vec3_origin, cgs.gameSounds[ es->eventParm ] );
			}
		}
		else
		{
			if ( cgs.gameSounds[ es->eventParm ] ) {
				trap_S_StartSound (NULL, es->number, es->saberEntityNum, cgs.gameSounds[ es->eventParm ] );
			} else {
				s = CG_ConfigString( CS_SOUNDS + es->eventParm );
				trap_S_StartSound (NULL, es->number, es->saberEntityNum, CG_CustomSound( es->number, s ) );
			}
		}
		break;

	case EV_GLOBAL_SOUND:	// play from the player's head so it never diminishes
		DEBUGNAME("EV_GLOBAL_SOUND");
		if ( cgs.gameSounds[ es->eventParm ] ) {
			trap_S_StartSound (NULL, cg.snap->ps.clientNum, CHAN_MENU1, cgs.gameSounds[ es->eventParm ] );
		} else {
			s = CG_ConfigString( CS_SOUNDS + es->eventParm );
			trap_S_StartSound (NULL, cg.snap->ps.clientNum, CHAN_MENU1, CG_CustomSound( es->number, s ) );
		}
		break;

	case EV_GLOBAL_TEAM_SOUND:	// play from the player's head so it never diminishes
		//eezstreet edit: fixed the team sounds as_per cg_main.c
		{
			DEBUGNAME("EV_GLOBAL_TEAM_SOUND");
			switch( es->eventParm ) {
				case GTS_RED_CAPTURE: // CTF: red team captured the blue flag, 1FCTF: red team captured the neutral flag
					//CG_AddBufferedSound( cgs.media.redScoredSound );
					break;
				case GTS_BLUE_CAPTURE: // CTF: blue team captured the red flag, 1FCTF: blue team captured the neutral flag
					//CG_AddBufferedSound( cgs.media.blueScoredSound );
					break;
				case GTS_RED_RETURN: // CTF: blue flag returned, 1FCTF: never used
					if (cg.snap->ps.persistant[PERS_TEAM] == TEAM_BLUE)
					{
						//trap_S_StartSound (NULL, cg.snap->ps.clientNum, CHAN_ANNOUNCER, cgs.media.blueFlagReturnedSound);
						CG_AddBufferedSound( cgs.media.blueFlagReturnedSound );
					}
					else if(cg.snap->ps.persistant[PERS_TEAM] == TEAM_RED){
						//trap_S_StartSound (NULL, cg.snap->ps.clientNum, CHAN_ANNOUNCER, cgs.media.redFlagReturnedSound);
						CG_AddBufferedSound( cgs.media.redFlagReturnedSound );
					}
					else{
						//trap_S_StartSound (NULL, cg.snap->ps.clientNum, CHAN_ANNOUNCER, cgs.media.redFlagReturnedSound);
						CG_AddBufferedSound( cgs.media.redFlagReturnedSound );
					}
					if(cgs.gametype == GT_CTF || cgs.gametype == GT_CTY)
						CG_EVBox_AddString("The blue team has returned their flag.");
					/*if (cgs.gametype == GT_CTY)
					{
						CG_AddBufferedSound( cgs.media.blueYsalReturnedSound );
					}
					else
					{
						CG_AddBufferedSound( cgs.media.blueFlagReturnedSound );
					}*/
					break;
				case GTS_BLUE_RETURN: // CTF red flag returned, 1FCTF: neutral flag returned
					if (cg.snap->ps.persistant[PERS_TEAM] == TEAM_BLUE)
					{
						//trap_S_StartSound (NULL, cg.snap->ps.clientNum, CHAN_ANNOUNCER, cgs.media.redFlagReturnedSound);
						CG_AddBufferedSound( cgs.media.redFlagReturnedSound );
					}
					else if(cg.snap->ps.persistant[PERS_TEAM] == TEAM_RED){
						//trap_S_StartSound (NULL, cg.snap->ps.clientNum, CHAN_ANNOUNCER, cgs.media.blueFlagReturnedSound);
						CG_AddBufferedSound( cgs.media.blueFlagReturnedSound );
					}
					else{
						//trap_S_StartSound (NULL, cg.snap->ps.clientNum, CHAN_ANNOUNCER, cgs.media.blueFlagReturnedSound);
						CG_AddBufferedSound( cgs.media.blueFlagReturnedSound );
					}
					if(cgs.gametype == GT_CTF || cgs.gametype == GT_CTY)
						CG_EVBox_AddString("The red team has returned their flag.");
					/*if (cgs.gametype == GT_CTY)
					{
						CG_AddBufferedSound( cgs.media.redYsalReturnedSound );
					}
					else
					{
						CG_AddBufferedSound( cgs.media.redFlagReturnedSound );
					}*/
					break;

				case GTS_RED_TAKEN: // CTF: red team took blue flag, 1FCTF: blue team took the neutral flag
					// if this player picked up the flag then a sound is played in CG_CheckLocalSounds
					if(cgs.gametype == GT_TEAM || cgs.gametype == GT_HERO){
						return; //this should never happen
					}
					else if (cg.snap->ps.persistant[PERS_TEAM] == TEAM_BLUE)
					{
						//trap_S_StartSound (NULL, cg.snap->ps.clientNum, CHAN_ANNOUNCER, cgs.media.blueTookFlagSound);
						CG_AddBufferedSound( cgs.media.blueTookFlagSound );
						CG_EVBox_AddString("The red team has stolen the flag!");
					}
					else if(cg.snap->ps.persistant[PERS_TEAM] == TEAM_RED){
						//trap_S_StartSound (NULL, cg.snap->ps.clientNum, CHAN_ANNOUNCER, cgs.media.redTookFlagSound);
						CG_AddBufferedSound( cgs.media.redTookFlagSound );
						CG_EVBox_AddString("The red team has stolen the flag!");
					}
					else{
						//trap_S_StartSound (NULL, cg.snap->ps.clientNum, CHAN_ANNOUNCER, cgs.media.redTookFlagSound);
						CG_AddBufferedSound( cgs.media.redTookFlagSound );
						CG_EVBox_AddString("The red team has stolen the flag!");
					}
					/*
					if (cgs.gametype == GT_CTY)
					{
						CG_AddBufferedSound( cgs.media.redTookYsalSound );
					}
					else
					{
					 	CG_AddBufferedSound( cgs.media.redTookFlagSound );
					}
					*/

					break;
				case GTS_BLUE_TAKEN: // CTF: blue team took the red flag, 1FCTF red team took the neutral flag
					// if this player picked up the flag then a sound is played in CG_CheckLocalSounds
					if(cgs.gametype == GT_TEAM || cgs.gametype == GT_HERO){
						return; //this should never happen
					}
					else if (cg.snap->ps.persistant[PERS_TEAM] == TEAM_BLUE)
					{
						//trap_S_StartSound (NULL, cg.snap->ps.clientNum, CHAN_ANNOUNCER, cgs.media.redTookFlagSound);
						CG_AddBufferedSound( cgs.media.redTookFlagSound );
						CG_EVBox_AddString("The blue team has stolen the flag!");
					}
					else if(cg.snap->ps.persistant[PERS_TEAM] == TEAM_RED){
						//trap_S_StartSound (NULL, cg.snap->ps.clientNum, CHAN_ANNOUNCER, cgs.media.blueTookFlagSound);
						CG_AddBufferedSound( cgs.media.blueTookFlagSound );
						CG_EVBox_AddString("The blue team has stolen the flag!");
					}
					else{
						//trap_S_StartSound (NULL, cg.snap->ps.clientNum, CHAN_ANNOUNCER, cgs.media.blueTookFlagSound);
						CG_AddBufferedSound( cgs.media.blueTookFlagSound );
						CG_EVBox_AddString("The blue team has stolen the flag!");
					}
					/*if (cgs.gametype == GT_CTY)
					{
						CG_AddBufferedSound( cgs.media.blueTookYsalSound );
					}
					else
					{
						CG_AddBufferedSound( cgs.media.blueTookFlagSound );
					}*/
					break;
				case GTS_REDTEAM_SCORED:
					if(cgs.gametype <= GT_TEAM || cgs.gametype == GT_HERO || cgs.gametype == GT_DOMINATION){
						return; //this should never happen
					}
					else{
						if (cg.snap->ps.persistant[PERS_TEAM] == TEAM_BLUE)
						{
							CG_AddBufferedSound( cgs.media.blueScoredSound );
							CG_EVBox_AddString("Red has scored!");
						}
						else if(cg.snap->ps.persistant[PERS_TEAM] == TEAM_RED){
							CG_AddBufferedSound( cgs.media.redScoredSound );
							CG_EVBox_AddString("Red has scored!");
						}
					}
					//CG_AddBufferedSound(cgs.media.redScoredSound);
					break;
				case GTS_BLUETEAM_SCORED:
					if(cgs.gametype <= GT_TEAM || cgs.gametype == GT_DOMINATION){
						return; //this should never happen
					}
					else{
						if (cg.snap->ps.persistant[PERS_TEAM] == TEAM_BLUE)
						{
							CG_AddBufferedSound( cgs.media.redScoredSound );
							CG_EVBox_AddString("Blue has scored!");
						}
						else if(cg.snap->ps.persistant[PERS_TEAM] == TEAM_RED){
							CG_AddBufferedSound( cgs.media.blueScoredSound );
							CG_EVBox_AddString("Blue has scored!");
						}
					}
					//CG_AddBufferedSound(cgs.media.blueScoredSound);
					break;
				case GTS_REDTEAM_TOOK_LEAD:
					if (cg.snap->ps.persistant[PERS_TEAM] == TEAM_BLUE)
					{
						CG_AddBufferedSound( cgs.media.blueLeadsSound );
					}
					else if(cg.snap->ps.persistant[PERS_TEAM] == TEAM_RED){
						CG_AddBufferedSound( cgs.media.redLeadsSound );

					}
					else{
						CG_AddBufferedSound( cgs.media.redLeadsSound );
					}
					CG_EVBox_AddString("Red has taken the lead.");
					//CG_AddBufferedSound(cgs.media.redLeadsSound);
					break;
				case GTS_BLUETEAM_TOOK_LEAD:
					//CG_AddBufferedSound(cgs.media.blueLeadsSound);
					if (cg.snap->ps.persistant[PERS_TEAM] == TEAM_BLUE)
					{
						CG_AddBufferedSound( cgs.media.redLeadsSound );
					}
					else if(cg.snap->ps.persistant[PERS_TEAM] == TEAM_RED){
						CG_AddBufferedSound( cgs.media.blueLeadsSound );
					}
					else{
						CG_AddBufferedSound( cgs.media.blueLeadsSound );
					}
					CG_EVBox_AddString("Blue has taken the lead.");
					break;
				case GTS_TEAMS_ARE_TIED:
					CG_AddBufferedSound( cgs.media.teamsTiedSound );
					CG_EVBox_AddString("The teams are tied.");
					break;
				case GTS_NEW_JEDIMASTER:
					trap_S_StartSound (NULL, cg.snap->ps.clientNum, CHAN_MENU1, cgs.media.redFlagReturnedSound );
					break;
				case GTS_JEDIMASTER_DIE:
					trap_S_StartSound (NULL, cg.snap->ps.clientNum, CHAN_MENU1, cgs.media.blueFlagReturnedSound );
					break;
				default:
					break;
			}
			break;
		}

	case EV_ENTITY_SOUND:
		DEBUGNAME("EV_ENTITY_SOUND");
		//somewhat of a hack - weapon is the caller entity's index, trickedentindex is the proper sound channel
		//[CoOp]
		//determine gender
		if(es->trickedentindex == CHAN_VOICE || es->trickedentindex == CHAN_VOICE_GLOBAL)
		{
			int gender;
			
			if(es->clientNum < MAX_CLIENTS && cg_entities[es->clientNum].currentValid)
			{//already a player
				gender = cgs.clientinfo[es->clientNum].gender;
			}
			else if(cg_entities[es->clientNum].currentState.eType == ET_NPC)
			{//NPC
				gender = cg_entities[es->clientNum].npcClient->gender;
			}
			else
			{//use the highest scorer's gender
				gender = cgs.clientinfo[cg.scores[0].client].gender;
			}

			if(gender == GENDER_FEMALE)
			{		
				char *r;
				s = CG_ConfigString( CS_SOUNDS + es->eventParm );
				r = strstr(s, "SOUND/CHARS/JADEN_MALE/");

				if(r)
				{
					char soundName[MAX_QPATH];
					char *nameSlash;
					char *fileName;

					if(!strstr(s, "SOUND/CHARS/JADEN_MALE/MISC/"))
					{//check for misc sounds
						strcpy(soundName, "SOUND/CHARS/JADEN_FMLE/");
					}
					else
					{
						strcpy(soundName, "SOUND/CHARS/JADEN_FMLE/MISC/");
					}

					//find the slash
					nameSlash = Q_strrchr( soundName, '/' );

					//find the filename
					fileName = Q_strrchr( r, '/' );
					strcpy(nameSlash, fileName);

					trap_S_StartSound (NULL, es->clientNum, es->trickedentindex, CG_CustomSound( es->clientNum, soundName ) );
					break;
				}
			}
		}
		//[/CoOp]
		if ( cgs.gameSounds[ es->eventParm ] ) {
			trap_S_StartSound (NULL, es->clientNum, es->trickedentindex, cgs.gameSounds[ es->eventParm ] );
		} else {
			s = CG_ConfigString( CS_SOUNDS + es->eventParm );
			trap_S_StartSound (NULL, es->clientNum, es->trickedentindex, CG_CustomSound( es->clientNum, s ) );
		}
		break;

	case EV_SWAP_FIRING_MODE:
		DEBUGNAME("EV_SWAP_FIRING_MODE");
		trap_S_StartSound(NULL, es->clientNum, es->trickedentindex, cgs.media.weaponModeSwapSound);
		break;

	case EV_PLAY_ROFF:
		DEBUGNAME("EV_PLAY_ROFF");
		trap_ROFF_Play(es->weapon, es->eventParm, es->trickedentindex);
		break;

	case EV_GLASS_SHATTER:
		DEBUGNAME("EV_GLASS_SHATTER");
		CG_GlassShatter(es->genericenemyindex, es->origin, es->angles, es->trickedentindex, es->pos.trTime);
		break;

	case EV_DEBRIS:
		DEBUGNAME("EV_DEBRIS");
		CG_Chunks(es->owner, es->origin, es->angles, es->origin2, es->angles2, es->speed,
			es->eventParm, es->trickedentindex, es->modelindex, es->apos.trBase[0]);
		break;

	case EV_MISC_MODEL_EXP:
		DEBUGNAME("EV_MISC_MODEL_EXP");
		CG_MiscModelExplosion(es->origin2, es->angles2, es->time, es->eventParm);
		break;

	case EV_PAIN:
		// local player sounds are triggered in CG_CheckLocalSounds,
		// so ignore events on the player
		DEBUGNAME("EV_PAIN");

		if ( !cg_oldPainSounds.integer || (cent->currentState.number != cg.snap->ps.clientNum) )
		{
			//Fuck that. Let's make it send the Means of Death too.
			CG_PainEvent( cent, es->eventParm );
		}
		break;

	case EV_BLIND:
		DEBUGNAME("EV_BLIND");
		{
			PlayClientSound(&cgs.clientinfo[cent->currentState.clientNum], CST_BLIND, cent->currentState.clientNum, NULL);
		}
		break;

	case EV_DEATH1:
	case EV_DEATH2:
	case EV_DEATH3:
		DEBUGNAME("EV_DEATHx");
		/*trap_S_StartSound( NULL, es->number, CHAN_VOICE, 
				CG_CustomSound( es->number, va("*death%i.wav", Q_irand(1,10)) ) );*/
		if(es->number < MAX_CLIENTS && cgs.clientinfo[es->number].infoValid)
		{
			PlayClientSound(&cgs.clientinfo[es->number], CST_DEATH, es->number, NULL);
		}
		else
		{
			CG_TryPlayCustomSound(NULL, es->number, CHAN_VOICE, va("*death%i", event - EV_DEATH1 + 1));
		}
		if (es->eventParm && es->number == cg.snap->ps.clientNum)
		{
			trap_S_StartLocalSound(cgs.media.dramaticFailure, CHAN_LOCAL);
			CGCam_SetMusicMult(0.3, 5000);
			CG_EVBox_AddString("The saber has been dropped.");
			es->trickedentindex = MAX_CLIENTS+1;
			es->eventParm = GTS_JEDIMASTER_DIE;
		}
		if(es->number == cg.snap->ps.clientNum)
		{
			int i;
			for(i = 0; i < WP_NUM_WEAPONS; i++)
			{
				cg.akimboGuns[i] = 0;
			}
		}
		break;


	case EV_OBITUARY:
		DEBUGNAME("EV_OBITUARY");
		CG_Obituary( es );
		break;

	//
	// powerup events
	//
	case EV_POWERUP_QUAD:
		DEBUGNAME("EV_POWERUP_QUAD");
		if ( es->number == cg.snap->ps.clientNum ) {
			cg.powerupActive = PW_QUAD;
			cg.powerupTime = cg.time;
		}
		//trap_S_StartSound (NULL, es->number, CHAN_ITEM, cgs.media.quadSound );
		break;
	case EV_POWERUP_BATTLESUIT:
		DEBUGNAME("EV_POWERUP_BATTLESUIT");
		if ( es->number == cg.snap->ps.clientNum ) {
			cg.powerupActive = PW_BATTLESUIT;
			cg.powerupTime = cg.time;
		}
		//trap_S_StartSound (NULL, es->number, CHAN_ITEM, cgs.media.protectSound );
		break;

	case EV_FORCE_DRAINED:
		DEBUGNAME("EV_FORCE_DRAINED");
		ByteToDir( es->eventParm, dir );
		//FX_ForceDrained(position, dir);
		trap_S_StartSound (NULL, es->owner, CHAN_AUTO, cgs.media.drainSound );
		cg_entities[es->owner].teamPowerEffectTime = cg.time + 1000;
		cg_entities[es->owner].teamPowerType = 2;
		break;

	case EV_GIB_PLAYER:
		DEBUGNAME("EV_GIB_PLAYER");
		//trap_S_StartSound( NULL, es->number, CHAN_BODY, cgs.media.gibSound );
		//CG_GibPlayer( cent->lerpOrigin );
		break;

	case EV_STARTLOOPINGSOUND:
		DEBUGNAME("EV_STARTLOOPINGSOUND");
		if ( cgs.gameSounds[ es->eventParm ] )
		{
			isnd = cgs.gameSounds[es->eventParm];
		}
		else
		{
			s = CG_ConfigString( CS_SOUNDS + es->eventParm );
			isnd = CG_CustomSound(es->number, s);
		}

		CG_S_AddRealLoopingSound( es->number, es->pos.trBase, vec3_origin, isnd );
		es->loopSound = isnd;
		break;

	case EV_STOPLOOPINGSOUND:
		DEBUGNAME("EV_STOPLOOPINGSOUND");
		CG_S_StopLoopingSound( es->number, -1 );
		es->loopSound = 0;
		break;

	case EV_WEAPON_CHARGE:
		DEBUGNAME("EV_WEAPON_CHARGE");
		assert(es->eventParm > WP_NONE && es->eventParm < WP_NUM_WEAPONS);
		if (cg_weapons[es->eventParm].chargeSound)
		{
			trap_S_StartSound(NULL, es->number, CHAN_WEAPON, cg_weapons[es->eventParm].chargeSound);
		}
		else if (es->eventParm == WP_DISRUPTOR)
		{
			trap_S_StartSound(NULL, es->number, CHAN_WEAPON, cgs.media.disruptorZoomLoop);
		}
		break;

	case EV_WEAPON_CHARGE_ALT:
		DEBUGNAME("EV_WEAPON_CHARGE_ALT");

		assert(es->eventParm > WP_NONE && es->eventParm < WP_NUM_WEAPONS);
		if (cg_weapons[es->eventParm].altChargeSound)
		{
			trap_S_StartSound(NULL, es->number, CHAN_WEAPON, cg_weapons[es->eventParm].altChargeSound);
		}
		break;

	case EV_SHIELD_HIT:
		DEBUGNAME("EV_SHIELD_HIT");
		ByteToDir(es->eventParm, dir);
		CG_PlayerShieldHit(es->otherEntityNum, dir, es->time2);
		break;

	case EV_DEBUG_LINE:
		DEBUGNAME("EV_DEBUG_LINE");
		CG_Beam( cent );
		break;

	case EV_TESTLINE:
		DEBUGNAME("EV_TESTLINE");
		CG_TestLine(es->origin, es->origin2, es->time2, es->weapon, 1);
		break;
#ifdef _RELOADING
	case EV_RELOADING:
		DEBUGNAME("EV_RELOADING");
		//Not used yet.
		break;
#endif

	//[VisualWeapons]
	//this event is sent whenever a player has lost/picked up a weapon.  This is used
	//to update the client side's weapon stats for the holstered weapons rendering.
	case EV_WEAPINVCHANGE:
		DEBUGNAME("EV_WEAPINVCHANGE");
		cg_entities[es->number].weapons = es->eventParm;
		break;
	//[/VisualWeapons]

	case EV_RADAR_SHOTPING:
		DEBUGNAME("EV_RADAR_SHOTPING");
 		CG_AddRadarEnt(cent);
		cent->trickAlphaTime = cg.time + 3000;
		break;


	default:
		DEBUGNAME("UNKNOWN");
		CG_Error( "Unknown event: %i", event );
		break;
	}

}


/*
==============
CG_CheckEvents

==============
*/
void CG_CheckEvents( centity_t *cent ) {
	// check for event-only entities
	if ( cent->currentState.eType > ET_EVENTS ) {
		if ( cent->previousEvent ) {
			return;	// already fired
		}
		// if this is a player event set the entity number of the client entity number
		if ( cent->currentState.eFlags & EF_PLAYER_EVENT ) {
			cent->currentState.number = cent->currentState.otherEntityNum;
		}

		cent->previousEvent = 1;

		cent->currentState.event = cent->currentState.eType - ET_EVENTS;
	} else {
		// check for events riding with another entity
		if ( cent->currentState.event == cent->previousEvent ) {
			return;
		}
		cent->previousEvent = cent->currentState.event;
		if ( ( cent->currentState.event & ~EV_EVENT_BITS ) == 0 ) {
			return;
		}
	}

	// calculate the position at exactly the frame time
	BG_EvaluateTrajectory( &cent->currentState.pos, cg.snap->serverTime, cent->lerpOrigin );
	CG_SetEntitySoundPosition( cent );

	CG_EntityEvent( cent, cent->lerpOrigin );
}
