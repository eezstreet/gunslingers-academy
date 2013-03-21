#pragma once
//#ifdef _DEBUG
//	#define BREAK __asm1__( int 3 )
//#else
	#define BREAK Com_Printf( "\n^3================================\n^5Something bad has happened.\nPlease file a bug report explaining how you achieved this.\n^3================================\n" )
//#endif