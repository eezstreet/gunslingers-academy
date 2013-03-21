#include "ui_loadout.h"
//==============================
// ui_loadout.c
// by eezstreet
//
// Contains stuff for the loadout
// and attachments menu
//==============================

//This is what happens when you click 'Apply'

char primaryGuns[WP_NUM_WEAPONS][64];
char sidearmGuns[WP_NUM_WEAPONS][64];
char explosiveGuns[WP_NUM_WEAPONS][64];
int numPrimary;
int numSidearms;
int numExplosives;

void GSA_UI_UpdateLoadout()
{
	//Grab our cvar values
	int primary, sidearm, explosives;
	primary = PrimaryWeapons[atoi(UI_Cvar_VariableString("ui_primaryGun"))];
	sidearm = SidearmWeapons[atoi(UI_Cvar_VariableString("ui_sidearm"))];
	explosives = Explosives[atoi(UI_Cvar_VariableString("ui_explosives"))];

	//Send stuff
	trap_Cmd_ExecuteText( EXEC_APPEND, va("oldfashionedlothing %i %i %i", primary, sidearm, explosives) );
}

//This is what happens upon opening
void GSA_UI_SetInitialLoadout()
{
	int i;
	int j = 1;
	int k = 1;
	int l = 1;

	numPrimary = 1;
	numSidearms = 1;
	numExplosives = 1;

	strcpy(primaryGuns[0], "Random");
	strcpy(sidearmGuns[0], "Random");
	strcpy(explosiveGuns[0], "Random");

	//Bah, humbug.
	trap_Cvar_Set("ui_primaryGun", "0");
	trap_Cvar_Set("ui_sidearm", "0");
	trap_Cvar_Set("ui_explosives", "0");

	for(i = 0; i < WP_NUM_WEAPONS; i++)
	{
		if(weaponData[i].weaponType == WT_PRIMARY)
		{
			strcpy(primaryGuns[j], weaponData[i].weaponName);
			j++;
			numPrimary++;
		}
		else if(weaponData[i].weaponType == WT_SIDEARM)
		{
			strcpy(sidearmGuns[k], weaponData[i].weaponName);
			numSidearms++;
			k++;
		}
		else if(weaponData[i].weaponType == WT_EXPLOSIVE)
		{
			strcpy(explosiveGuns[l], weaponData[i].weaponName);
			l++;
			numExplosives++;
		}
	}
}

int retrieveWeaponByName(const char *comparison)
{
	int i;
	for(i = 0; i < WP_NUM_WEAPONS; i++)
	{
		if(Q_stricmp(weaponData[i].weaponName, comparison) == 0)
			return i;
	}
	return -1;
}
int grabWTIndex(int weaponNum, int type)
{
	int i;
	switch(type)
	{
		case WT_PRIMARY:
			for(i = 0; i < numPrimary; i++)
			{
				if(PrimaryWeapons[i] == weaponNum)
					return i;
			}
			break;
		case WT_SIDEARM:
			for(i = 0; i < numSidearms; i++)
			{
				if(SidearmWeapons[i] == weaponNum)
					return i;
			}
			break;
		case WT_EXPLOSIVE:
			for(i = 0; i < numExplosives; i++)
			{
				if(Explosives[i] == weaponNum)
					return i;
			}
			break;
	}
	return 0;
}

extern weaponData_t weaponData[WP_NUM_WEAPONS];
extern displayContextDef_t *DC;
//This is what happens upon clicking a new option
void GSA_UI_LoadoutFeederAction(int index, int feederID)
{
	int newFeeder = feederID - FEEDER_LO_PRIMARY;
	int weaponSelections[3];
	int weaponNum;
	menuDef_t *menu;
	itemDef_t *item;
	switch(newFeeder)
	{
		case WT_PRIMARY:
			weaponNum = retrieveWeaponByName(primaryGuns[index]);
			trap_Cvar_Set("ui_primaryGun", va("%i", grabWTIndex(weaponNum, WT_PRIMARY)));
			break;
		case WT_SIDEARM:
			weaponNum = retrieveWeaponByName(sidearmGuns[index]);
			trap_Cvar_Set("ui_sidearm", va("%i", grabWTIndex(weaponNum, WT_SIDEARM)));
			break;
		case WT_EXPLOSIVE:
			weaponNum = retrieveWeaponByName(explosiveGuns[index]);
			trap_Cvar_Set("ui_explosives", va("%i", grabWTIndex(weaponNum, WT_EXPLOSIVE)));
			break;
		default:
			return;
	}
	weaponSelections[0] = (int)trap_Cvar_VariableValue("ui_primaryGun");
	weaponSelections[1] = (int)trap_Cvar_VariableValue("ui_sidearm");
	weaponSelections[2] = (int)trap_Cvar_VariableValue("ui_explosives");

	if(newFeeder == WT_PRIMARY)
	{
		menu = Menus_FindByName("ingame_loadout");
		item = Menu_FindItemByName(menu, "primWeapIcon");
		if(weaponSelections[0] != 0)
		{
			char sound[64];
			strcpy(sound, weaponData[retrieveWeaponByName(primaryGuns[index])].selectSound);
			item->window.background = DC->registerShaderNoMip(weaponData[retrieveWeaponByName(primaryGuns[index])].iconPath);
			trap_S_StartLocalSound(trap_S_RegisterSound((const char *)sound), CHAN_LOCAL_SOUND );
		}
		else
		{
			item->window.background = DC->registerShaderNoMip("gfx/weapon_icons/random");
		}
	}
	else if(newFeeder == WT_SIDEARM)
	{
		menu = Menus_FindByName("ingame_loadout");
		item = Menu_FindItemByName(menu, "sideWeapIcon");
		if(weaponSelections[0] != 0)
		{
			char sound[64];
			strcpy(sound, weaponData[retrieveWeaponByName(sidearmGuns[index])].selectSound);
			item->window.background = DC->registerShaderNoMip(weaponData[retrieveWeaponByName(sidearmGuns[index])].iconPath);
			trap_S_StartLocalSound(trap_S_RegisterSound((const char *)sound), CHAN_LOCAL_SOUND );
		}
		else
		{
			item->window.background = DC->registerShaderNoMip("gfx/weapon_icons/random");
		}
	}
	else if(newFeeder == WT_EXPLOSIVE)
	{
		menu = Menus_FindByName("ingame_loadout");
		item = Menu_FindItemByName(menu, "expWeapIcon");
		if(weaponSelections[0] != 0)
		{
			char sound[64];
			strcpy(sound, weaponData[retrieveWeaponByName(explosiveGuns[index])].selectSound);
			item->window.background = DC->registerShaderNoMip(weaponData[retrieveWeaponByName(explosiveGuns[index])].iconPath);
			trap_S_StartLocalSound(trap_S_RegisterSound((const char *)sound), CHAN_LOCAL_SOUND );
		}
		else
		{
			item->window.background = DC->registerShaderNoMip("gfx/weapon_icons/random");
		}
	}
}