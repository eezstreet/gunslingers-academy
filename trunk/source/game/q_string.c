//q_string.c
#include "g_local.h"
#include "q_helper.h"
//Quake 3 string helper file
//Q_bSA - breaks array into a space-delimited string
char *Q_bSA(char arrayToBreak[32][64], int maximum){
	int i;
	static char returnValue[384];
	memset(returnValue, 0, 384);
	for(i=0; i<maximum; i++){
		strcat(returnValue, arrayToBreak[i]);
		strcat(returnValue, va(" "));
	}
	return returnValue;
}