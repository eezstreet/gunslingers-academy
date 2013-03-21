#include "q_shared.h"

#ifdef _WIN32
#define COLORCODE_ADDRESS	0x56DF48
#elif MAC_PORT
#define COLORCODE_ADDRESS 0x19B880
#endif

void SetPaletteColor(int colorcode, float r, float g, float b, float a)
{
	if (colorcode < 0 || colorcode > 7) {
		return;
	}
	*(float *)(COLORCODE_ADDRESS + (colorcode * 16)) = r;
	*(float *)(COLORCODE_ADDRESS + (colorcode * 16) + 4) = g;
	*(float *)(COLORCODE_ADDRESS + (colorcode * 16) + 8) = b;
	*(float *)(COLORCODE_ADDRESS + (colorcode * 16) + 12) = a;
}
void PaletteChange(int colorcode, char *color)
{
	if(colorcode < 0 || colorcode > 7)
		return;
	if(strcmp(color, "red") == 0){
		SetPaletteColor(colorcode, 1.0, 0.0, 0.0, 1.0);
	}
	else if(strcmp(color, "green") == 0){
		SetPaletteColor(colorcode, 0.0, 1.0, 0.0, 1.0);
	}
	else if(strcmp(color, "yellow") == 0){
		SetPaletteColor(colorcode, 1.0, 1.0, 0.0, 1.0);
	}
	else if(strcmp(color, "blue") == 0){
		SetPaletteColor(colorcode, 0.0, 0.0, 1.0, 1.0);
	}
	else if(strcmp(color, "cyan") == 0){
		SetPaletteColor(colorcode, 0.0, 1.0, 1.0, 1.0);
	}
	else if(strcmp(color, "purple") == 0){
		SetPaletteColor(colorcode, 1.0, 0.0, 1.0, 1.0);
	}
	else if(strcmp(color, "white") == 0){
		SetPaletteColor(colorcode, 1.0, 1.0, 1.0, 1.0);
	}
	else if(strcmp(color, "black") == 0){
		SetPaletteColor(colorcode, 0.0, 0.0, 0.0, 1.0);
	}
	else if(strcmp(color, "lgrey") == 0){
		SetPaletteColor(colorcode, 0.75, 0.75, 0.75, 1.0);
	}
	else if(strcmp(color, "mgrey") == 0){
		SetPaletteColor(colorcode, 0.5, 0.5, 0.5, 1.0);
	}
	else if(strcmp(color, "dgrey") == 0){
		SetPaletteColor(colorcode, 0.25, 0.25, 0.25, 1.0);
	}
	else if(strcmp(color, "lgray") == 0){
		SetPaletteColor(colorcode, 0.75, 0.75, 0.75, 1.0);
	}
	else if(strcmp(color, "mgray") == 0){
		SetPaletteColor(colorcode, 0.5, 0.5, 0.5, 1.0);
	}
	else if(strcmp(color, "dgray") == 0){
		SetPaletteColor(colorcode, 0.25, 0.25, 0.25, 1.0);
	}
	else if(strcmp(color, "navy") == 0){
		SetPaletteColor(colorcode, 0.1, 0.1, 0.43, 1.0);
	}
	else if(strcmp(color, "slate") == 0){
		SetPaletteColor(colorcode, 0.28, 0.24, 0.545f, 1.0);
	}
	else if(strcmp(color, "royalb") == 0){
		SetPaletteColor(colorcode, 0.25, 0.418f, 0.88, 1.0);
	}
	else if(strcmp(color, "royalblue") == 0){
		SetPaletteColor(colorcode, 0.25, 0.418f, 0.88, 1.0);
	}
	else if(strcmp(color, "sky") == 0){
		SetPaletteColor(colorcode, 0.53, 0.81, 0.98, 1.0);
	}
	else if(strcmp(color, "steel") == 0){
		SetPaletteColor(colorcode, 0.275f, 0.51, 0.706f, 1.0);
	}
	else if(strcmp(color, "fadedb") == 0){
		SetPaletteColor(colorcode, 0.7, 0.76, 0.87, 1.0);
	}
	else if(strcmp(color, "fadedblue") == 0){
		SetPaletteColor(colorcode, 0.7, 0.76, 0.87, 1.0);
	}
	else if(strcmp(color, "dgreen") == 0){
		SetPaletteColor(colorcode, 0.0, 0.39, 0.0, 1.0);
	}
	else if(strcmp(color, "olive") == 0){
		SetPaletteColor(colorcode, 0.33, 0.42, 0.184f, 1.0);
	}
	else if(strcmp(color, "dsea") == 0){
		SetPaletteColor(colorcode, 0.56, 0.737f, 0.56, 1.0);
	}
	else if(strcmp(color, "sea") == 0){
		SetPaletteColor(colorcode, 0.18, 0.545f, 0.3411f, 1.0);
	}
	else if(strcmp(color, "lime") == 0){
		SetPaletteColor(colorcode, 0.49f, 1.0, 0.0, 1.0);
	}
	else if(strcmp(color, "gold") == 0){
		SetPaletteColor(colorcode, 1.0, 0.843f, 0.0, 1.0);
	}
	else if(strcmp(color, "sand") == 0){
		SetPaletteColor(colorcode, 0.933f, 0.866f, 0.51, 1.0);
	}
	else if(strcmp(color, "orange") == 0){
		SetPaletteColor(colorcode, 1.0, 0.65, 0.0, 1.0);
	}
	else if(strcmp(color, "dorange") == 0){
		SetPaletteColor(colorcode, 1.0, 0.549f, 0.0, 1.0);
	}
	else if(strcmp(color, "hpink") == 0){
		SetPaletteColor(colorcode, 1.0, 0.412f, 0.706f, 1.0);
	}
	else if(strcmp(color, "dpink") == 0){
		SetPaletteColor(colorcode, 1.0, 0.08f, 0.576f, 1.0);
	}
	else if(strcmp(color, "lpink") == 0){
		SetPaletteColor(colorcode, 1.0, 0.753f, 0.796f, 1.0);
	}
	else if(strcmp(color, "violet") == 0){
		SetPaletteColor(colorcode, 0.58, 0.0, 0.8274f, 1.0);
	}
	else if(strcmp(color, "scarlet") == 0){
		SetPaletteColor(colorcode, 0.549f, 0.09f, 0.09f, 1.0);
	}
	else if(strcmp(color, "maroon") == 0){
		SetPaletteColor(colorcode, 0.5, 0.0, 0.0, 1.0);
	}
	else{
		SetPaletteColor(colorcode, 1.0, 1.0, 1.0, 1.0);
	}
}

//Not used-yet stuff
//#ifdef HOOK_CONNJACK
//	//--------------------------------
//	//	Name:	q3connjack patch
//	//	Desc:	A malicious server can flood clients with challengeResponse packets to hijack a pending connection
//	//	Hook:	CL_ServerInfoPacket
//	//	Retn:	CL_ServerInfoPacket
//	//--------------------------------
//	#ifdef _WIN32
//		#define CJ_HOOKPOS		0x41EAF7	//	.text		83 3D 08 CC B3 00 01	cmp Cmd_Argc, 1
//		#define CJ_RETSUCCESS	0x41EAFE	//	.text		B8 3C 5C 54 00			mov eax, offset NullString
//		#define CJ_RETFAIL		0x41EAEC	//	.text		5F						pop edi
//	#elif defined(MACOS_X)
//		#error HOOK_QUERYBOOM not yet available on Mac OSX
//	#endif
//
//	HOOK( ConnJack )
//	{//q3connjack
//		__StartHook( ConnJack )
//		{
//			static netadr_t *connJack_from;
//			static qboolean connJack_result;
//			static int connJack_argc;
//
//			__asm2__( lea eax, [esp+0x410+0x04] ); // Load local variable from into netadr_t *from
//			__asm2__( mov connJack_from, eax );
//			__asm1__( pushad );
//
//			connJack_result = ENG_NET_CompareAdr( *connJack_from, *clc_serverAddress );
//			connJack_argc = *cmd_argc;
//
//			__asm1__( popad );
//			__asm2__( mov eax, connJack_result );
//			__asm2__( test eax, eax );
//			__asm1__( jnz match );
//
//			__asm2__( mov eax, CJ_RETFAIL );
//			__asm1__( jmp eax );
//
//			// Matching:
//			__asmL__(match: );
//			__asm2__( cmp connJack_argc, 1 );
//			__asm2__( mov eax, CJ_RETSUCCESS );
//			__asm1__( jmp eax );
//		}
//		__EndHook( ConnJack )
//	}
//
//#endif //HOOK_CONNJACK
//
//#ifdef HOOK_QUERYBOOM
//	//--------------------------------
//	//	Name:	q3queryboom patch
//	//	Desc:	Upon recieving an oversized serverinfo response from the master server list, a buffer overflow will occur resulting in a crash
//	//	Hook:	CL_ServerInfoPacket
//	//	Retn:	CL_ServerInfoPacket
//	//--------------------------------
//	#ifdef _WIN32
//		#define QB_HOOKPOS		0x420083	//	.text		E8 38 49 02 00		call Info_ValueForKey
//		#define QB_RETSUCCESS	0x4449C0	//	.text		Info_ValueForKey	proc near
//		#define QB_RETFAIL		0x4200AE	//	.text		C3					retn
//	#elif defined(MACOS_X)
//		#error HOOK_QUERYBOOM not yet available on Mac OSX
//	#endif
//
//	static int USED QB_CheckPacket( const char *info )
//	{
//		char *value = NULL;
//		int length = 0;
//		
//		//Overall size > 384
//		length = strlen( info );
//		if ( length > 384 )
//		{
//			Com_Printf( "Ignoring server response due to oversized serverinfo packet (size: %i > 384)\n", length );
//			return 1;
//		}
//		
//		//hostname > 192
//		value = Info_ValueForKey( info, "hostname" );
//		length = strlen( value );
//		if ( length > 192 )
//		{
//			Com_Printf( "Ignoring server response due to oversized 'hostname' info key (size: %i > 192)\n", length );
//			return 1;
//		}
//
//		//mapname > 48
//		value = Info_ValueForKey( info, "mapname" );
//		length = strlen( value );
//		if ( length > 48 )
//		{
//			Com_Printf( "Ignoring server response due to oversized 'mapname' info key (size: %i > 48)\n", length );
//			return 1;
//		}
//
//		//fs_game > 32
//		value = Info_ValueForKey( info, "game" );
//		length = strlen( value );
//		if ( length > 32 )
//		{
//			Com_Printf( "Ignoring server response due to oversized 'game' info key (size: %i > 32)\n", length );
//			return 1;
//		}
//
//		return 0;
//	}
//
//	HOOK( QueryBoom )
//	{//QueryBoom
//		__StartHook( QueryBoom )
//		{
//			__asm1__( pushad );
//			__asm1__( push eax );
//			__asm1__( call QB_CheckPacket );
//			__asm2__( add esp, 4 );
//			__asm2__( test eax, eax );
//			__asm1__( popad );
//			__asm1__( jnz bad );
//			__asm1__( push QB_RETSUCCESS );
//			__asm1__( ret );
//
//			__asmL__(bad: );
//			__asm1__( push QB_RETFAIL );
//			__asm1__( ret );
//		}
//		__EndHook( QueryBoom )
//	}
//
//#endif //HOOK_QUERYBOOM
//
//PATCH( 0x454B5A, unsigned short, 0x9090 ); // Alt-enter
//PATCH( 0x436C83, byte, 0x77 ); // Fix Cmd_TokenizeString's ANSI support
//PATCH( 0x41DACB, byte, 0x03 ); // Avoid black screen upon connecting to invalid address from ingame
