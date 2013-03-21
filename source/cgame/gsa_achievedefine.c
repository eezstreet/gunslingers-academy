#ifndef UI
#include "gsa_achieve.h"
#else
#include "ui_shared.h"
#endif
#ifdef ACHIEVEMENTS

//Achievements, must match menudef.h
#define FEEDER_FEATS						0x2d			// Feats of strength earned
#define FEEDER_EARNED						0x2e			// Achievements earned
#define FEEDER_BASE							0x2f			// Base Training achievements
#define FEEDER_PRIMARY						0x30			// Primary Weapon achievements
#define FEEDER_SIDEARM						0x31			// Sidearm achievements
#define FEEDER_EQUIPMENT					0x32			// Equipment achievements
#define FEEDER_OPERATIONS					0x33			// Operations achievements
#define FEEDER_FEATEARNED					0x34			// Earned feat of strength

achieve_t GSA_achieveList[] =
{
	{
		"Sidearm",
		"Kill 10 people using the M&P 40",
		10,
		"sound/interface/secret_area",
		"",
		FEEDER_SIDEARM,
		GSA_BasicAchieveFunc,
		NULL
	},
	{
		"Sidearm II",
		"Kill 25 people using the M&P 40",
		25,
		"sound/interface/secret_area",
		"",
		FEEDER_SIDEARM,
		GSA_BasicAchieveFunc,
		NULL
	},
	{
		"Sidearm III",
		"Kill 50 people using the M&P 40",
		50,
		"sound/interface/secret_area",
		"",
		FEEDER_SIDEARM,
		GSA_BasicAchieveFunc,
		NULL
	},
	{
		"Suicidal",
		"Die by falling.",
		1,
		"sound/interface/secret_area",
		"",
		FEEDER_BASE,
		GSA_BasicAchieveFunc,
		NULL
	},
	{
		"Six-Shooter",
		"Kill 10 people using the Magnum",
		10,
		"sound/interface/secret_area",
		"",
		FEEDER_SIDEARM,
		GSA_BasicAchieveFunc,
		NULL
	},
	{
		"Six-Shooter II",
		"Kill 25 people using the Magnum",
		25,
		"sound/interface/secret_area",
		"",
		FEEDER_SIDEARM,
		GSA_BasicAchieveFunc,
		NULL
	},
	{
		"Six-Shooter III",
		"Kill 50 people using the Magnum",
		50,
		"sound/interface/secret_area",
		"",
		FEEDER_SIDEARM,
		GSA_BasicAchieveFunc,
		NULL
	},
	{
		"Soldier",
		"Kill 10 people using the M16",
		10,
		"sound/interface/secret_area",
		"",
		FEEDER_PRIMARY,
		GSA_BasicAchieveFunc,
		NULL
	},
	{
		"Soldier II",
		"Kill 25 people using the M16",
		25,
		"sound/interface/secret_area",
		"",
		FEEDER_PRIMARY,
		GSA_BasicAchieveFunc,
		NULL
	},
	{
		"Soldier III",
		"Kill 50 people using the M16",
		50,
		"sound/interface/secret_area",
		"",
		FEEDER_PRIMARY,
		GSA_BasicAchieveFunc,
		NULL
	},
	{
		"Reconnaissance",
		"Call in a Recon Team",
		1,
		"sound/interface/secret_area",
		"",
		FEEDER_EQUIPMENT,
		GSA_BasicAchieveFunc,
		NULL
	},
	{
		"Sentry",
		"Get 5 kills using a Sentry Gun",
		5,
		"sound/interface/secret_area",
		"",
		FEEDER_EQUIPMENT,
		GSA_BasicAchieveFunc,
		NULL
	},
	{
		"TFFA Winner",
		"Win a game of Team Deathmatch",
		1,
		"sound/interface/secret_area",
		"",
		FEEDER_OPERATIONS,
		GSA_BasicAchieveFunc,
		NULL
	},
	{
		"TFFA Master",
		"Win 5 games of Team Deathmatch",
		5,
		"sound/interface/secret_area",
		"",
		FEEDER_OPERATIONS,
		GSA_BasicAchieveFunc,
		NULL
	},
	{
		"TFFA Master II",
		"Win 10 games of Team Deathmatch",
		10,
		"sound/interface/secret_area",
		"",
		FEEDER_OPERATIONS,
		GSA_BasicAchieveFunc,
		NULL
	},
	{
		"TFFA Master III",
		"Win 25 games of Team Deathmatch",
		25,
		"sound/interface/secret_area",
		"",
		FEEDER_OPERATIONS,
		GSA_BasicAchieveFunc,
		NULL
	},
	{
		"TFFA Master IV",
		"Win 50 games of Team Deathmatch",
		50,
		"sound/interface/secret_area",
		"",
		FEEDER_OPERATIONS,
		GSA_BasicAchieveFunc,
		NULL
	},
	{
		"Impossible Odds",
		"Feat of Valor: Played online against the Gunslinger's Academy developers.",
		1,
		"sound/interface/secret_area",
		"",
		FEEDER_FEATS,
		GSA_BasicAchieveFunc,
		NULL
	},
	{
		"Point Guard I",
		"Get 5 assists.",
		5,
		"sound/interface/secret_area",
		"",
		FEEDER_BASE,
		GSA_BasicAchieveFunc,
		NULL
	},
	{
		"Point Guard II",
		"Get 10 assists.",
		10,
		"sound/interface/secret_area",
		"",
		FEEDER_BASE,
		GSA_BasicAchieveFunc,
		NULL
	},
	{
		"Point Guard III",
		"Get 20 assists.",
		20,
		"sound/interface/secret_area",
		"",
		FEEDER_BASE,
		GSA_BasicAchieveFunc,
		NULL
	},
	{
		"White Belt",
		"Kill 1 person using your fists.",
		1,
		"sound/interface/secret_area",
		"",
		FEEDER_BASE,
		GSA_BasicAchieveFunc,
		NULL
	},
	{
		"Blue Belt",
		"Kill 5 people using your fists.",
		5,
		"sound/interface/secret_area",
		"",
		FEEDER_BASE,
		GSA_BasicAchieveFunc,
		NULL
	},
	{
		"Black Belt",
		"Kill 10 people using your fists.",
		10,
		"sound/interface/secret_area",
		"",
		FEEDER_BASE,
		GSA_BasicAchieveFunc,
		NULL
	},
	{
		"Sleight of Hand",
		"Manually reload 50 times.",
		50,
		"sound/interface/secret_area",
		"",
		FEEDER_FEATS,
		GSA_BasicAchieveFunc,
		NULL
	},
	{
		"On-Duty",
		"Kill 10 people using the TASER",
		10,
		"sound/interface/secret_area",
		"",
		FEEDER_SIDEARM,
		GSA_BasicAchieveFunc,
		NULL
	},
	{
		"On-Duty II",
		"Kill 25 people using the TASER",
		25,
		"sound/interface/secret_area",
		"",
		FEEDER_SIDEARM,
		GSA_BasicAchieveFunc,
		NULL
	},
	{
		"On-Duty III",
		"Kill 50 people using the TASER",
		50,
		"sound/interface/secret_area",
		"",
		FEEDER_SIDEARM,
		GSA_BasicAchieveFunc,
		NULL
	},
	{
		"Master Blaster",
		"Kill 10 people using the Glock 18c",
		10,
		"sound/interface/secret_area",
		"",
		FEEDER_SIDEARM,
		GSA_BasicAchieveFunc,
		NULL
	},
	{
		"Master Blaster II",
		"Kill 25 people using the Glock 18c",
		25,
		"sound/interface/secret_area",
		"",
		FEEDER_SIDEARM,
		GSA_BasicAchieveFunc,
		NULL
	},
	{
		"Master Blaster III",
		"Kill 50 people using the Glock 18c",
		50,
		"sound/interface/secret_area",
		"",
		FEEDER_SIDEARM,
		GSA_BasicAchieveFunc,
		NULL
	},
	{
		"AK-1337",
		"Kill 10 people using the AK-104",
		10,
		"sound/interface/secret_area",
		"",
		FEEDER_PRIMARY,
		GSA_BasicAchieveFunc,
		NULL
	},
	{
		"AK-1337 II",
		"Kill 25 people using the AK-104",
		25,
		"sound/interface/secret_area",
		"",
		FEEDER_PRIMARY,
		GSA_BasicAchieveFunc,
		NULL
	},
	{
		"AK-1337 III",
		"Kill 50 people using the AK-104",
		50,
		"sound/interface/secret_area",
		"",
		FEEDER_PRIMARY,
		GSA_BasicAchieveFunc,
		NULL
	},
	{
		"Frag Master",
		"Kill 10 people using frag grenades.",
		10,
		"sound/interface/secret_area",
		"",
		FEEDER_EQUIPMENT,
		GSA_BasicAchieveFunc,
		NULL
	},
	{
		"Frag Master II",
		"Kill 25 people using frag grenades.",
		25,
		"sound/interface/secret_area",
		"",
		FEEDER_EQUIPMENT,
		GSA_BasicAchieveFunc,
		NULL
	},
	{
		"Frag Master III",
		"Kill 50 people using frag grenades.",
		50,
		"sound/interface/secret_area",
		"",
		FEEDER_EQUIPMENT,
		GSA_BasicAchieveFunc,
		NULL
	},
	{
		"Frag Master IV",
		"Kill 75 people using frag grenades.",
		75,
		"sound/interface/secret_area",
		"",
		FEEDER_EQUIPMENT,
		GSA_BasicAchieveFunc,
		NULL
	},
	{
		"Frag Master V",
		"Kill 100 people using frag grenades.",
		100,
		"sound/interface/secret_area",
		"",
		FEEDER_EQUIPMENT,
		GSA_BasicAchieveFunc,
		NULL
	},
	{
		"Magnum Headhunter",
		"Get 5 headshots using the Magnum",
		5,
		"sound/interface/secret_area",
		"",
		FEEDER_SIDEARM,
		GSA_BasicAchieveFunc,
		NULL
	},
	{
		"Magnum Headhunter II",
		"Get 10 headshots using the Magnum.",
		10,
		"sound/interface/secret_area",
		"",
		FEEDER_SIDEARM,
		GSA_BasicAchieveFunc,
		NULL
	},
	{
		"Magnum Headhunter III",
		"Get 25 headshots using the Magnum.",
		25,
		"sound/interface/secret_area",
		"",
		FEEDER_SIDEARM,
		GSA_BasicAchieveFunc,
		NULL
	},
	{
		"M14 Headhunter",
		"Get 5 headshots using the M14",
		5,
		"sound/interface/secret_area",
		"",
		FEEDER_PRIMARY,
		GSA_BasicAchieveFunc,
		NULL
	},
	{
		"M14 Headhunter II",
		"Get 10 headshots using the M14.",
		10,
		"sound/interface/secret_area",
		"",
		FEEDER_PRIMARY,
		GSA_BasicAchieveFunc,
		NULL
	},
	{
		"M14 Headhunter III",
		"Get 25 headshots using the M14.",
		25,
		"sound/interface/secret_area",
		"",
		FEEDER_PRIMARY,
		GSA_BasicAchieveFunc,
		NULL
	},
	{
		"M98C Headhunter",
		"Get 5 headshots using the M98C",
		5,
		"sound/interface/secret_area",
		"",
		FEEDER_PRIMARY,
		GSA_BasicAchieveFunc,
		NULL
	},
	{
		"M98C Headhunter II",
		"Get 10 headshots using the M98C.",
		10,
		"sound/interface/secret_area",
		"",
		FEEDER_PRIMARY,
		GSA_BasicAchieveFunc,
		NULL
	},
	{
		"M98C Headhunter III",
		"Get 25 headshots using the M98C.",
		25,
		"sound/interface/secret_area",
		"",
		FEEDER_PRIMARY,
		GSA_BasicAchieveFunc,
		NULL
	},
	{
		"Bullet For My Valentine",
		"Kill someone on Valentine's Day",
		1,
		"sound/interface/secret_area",
		"",
		FEEDER_FEATS,
		GSA_BasicAchieveFunc,
		NULL
	},
	{
		"Soothsayer",
		"Kill someone on the Ides of March",
		1,
		"sound/interface/secret_area",
		"",
		FEEDER_FEATS,
		GSA_BasicAchieveFunc,
		NULL
	},
	{
		"In Memoriam",
		"Kill someone on Memorial Day",
		1,
		"sound/interface/secret_area",
		"",
		FEEDER_FEATS,
		GSA_BasicAchieveFunc,
		NULL
	},
	{
		"Fight for Independence",
		"Kill someone on July 4th",
		1,
		"sound/interface/secret_area",
		"",
		FEEDER_FEATS,
		GSA_BasicAchieveFunc,
		NULL
	},
	{
		"Creepy Crawl",
		"Kill someone on Halloween",
		1,
		"sound/interface/secret_area",
		"",
		FEEDER_FEATS,
		GSA_BasicAchieveFunc,
		NULL
	},
	{
		"Veteran vrs. Noob",
		"Kill someone on Veteran's Day",
		1,
		"sound/interface/secret_area",
		"",
		FEEDER_FEATS,
		GSA_BasicAchieveFunc,
		NULL
	},
	{
		"...a Date in Infamy",
		"Kill someone on Pearl Harbor Remembrance Day",
		1,
		"sound/interface/secret_area",
		"",
		FEEDER_FEATS,
		GSA_BasicAchieveFunc,
		NULL
	},
	{
		"Anthrax",
		"Kill 10 people using gas",
		10,
		"sound/interface/secret_area",
		"",
		FEEDER_EQUIPMENT,
		GSA_BasicAchieveFunc,
		NULL
	},
	{
		"Anthrax II",
		"Kill 25 people using gas",
		25,
		"sound/interface/secret_area",
		"",
		FEEDER_EQUIPMENT,
		GSA_BasicAchieveFunc,
		NULL
	},
	{
		"Anthrax III",
		"Kill 50 people using gas",
		50,
		"sound/interface/secret_area",
		"",
		FEEDER_EQUIPMENT,
		GSA_BasicAchieveFunc,
		NULL
	},
	{
		"Full House",
		"Feat of Valor: Attended an Open House",
		1,
		"sound/interface/secret_area",
		"",
		FEEDER_FEATS,
		GSA_BasicAchieveFunc,
		NULL
	},
	{
		"Any Means Necessary",
		"Kicked down a locked door.",
		1,
		"sound/interface/secret_area",
		"",
		FEEDER_BASE,
		GSA_BasicAchieveFunc,
		NULL
	},

};
#endif