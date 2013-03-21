#include "cg_local.h"

int tipMaximum;

void GSA_DrawTip(void)
{
	int x = ( SCREEN_WIDTH ) / 2;
	int y = 465;
	trap_R_SetColor( colorWhite );

	tipMaximum = 20;
	if(!cg.tipInit)
	{
		srand(time(NULL));
		cg.currentTip = rand() % 25 + 1;
		cg.tipInit = qtrue;
	}

	//TODO: Dynamic maximum/string file based on map
	CG_Text_Paint ( x - CG_Text_Width( CG_GetStringEdString("MP_TIPS", va("TIP_%i", cg.currentTip)), 0.5f, FONT_SMALL ) / 2, y, 0.5f, colorWhite,
		CG_GetStringEdString("MP_TIPS", va("TIP_%i", cg.currentTip)), 0, 0, 4, FONT_SMALL );

}