//TrueView surface files
#include "q_shared.h"
#include "bg_public.h"
#include "bg_local.h"

#define READVALUE(a,b,c,d,e)	if(DB_RetrieveValue(database, ##a##, ##b##, buffer, ##c##)) ##d##[##b##].##e## = atoi(buffer);
#define READSTRING(a,b,c,d,e)	if(DB_RetrieveValue(database, ##a##, ##b##, buffer, ##c##)) strncpy(##d##[##b##].##e##, buffer, ##c##);
#define READCSTRING(a,b,c,d,e)	if(DB_RetrieveValue(database, ##a##, ##b##, buffer, ##c##)) strncpy((char *)##d##[##b##].##e##, buffer, ##c##);
#define READSHORT(a,b,c,d,e)	if(DB_RetrieveValue(database, ##a##, ##b##, buffer, ##c##)) ##d##[##b##].##e## = (short)atoi(buffer);
#define READUSHORT(a,b,c,d,e)	if(DB_RetrieveValue(database, ##a##, ##b##, buffer, ##c##)) ##d##[##b##].##e## = (unsigned short)atoi(buffer);
#define READFLOAT(a,b,c,d,e)	if(DB_RetrieveValue(database, ##a##, ##b##, buffer, ##c##)) ##d##[##b##].##e## = atof(buffer);
#define READUINT(a,b,c,d,e)		if(DB_RetrieveValue(database, ##a##, ##b##, buffer, ##c##)) ##d##[##b##].##e## = (unsigned int)atoi(buffer);
#define READQBOOL(a,b,c,d,e)	if(DB_RetrieveValue(database, ##a##, ##b##, buffer, ##c##)) ##d##[##b##].##e## = (qboolean)atoi(buffer);

#define TRUEVIEW_HEAD_SIZE	262142
#define TRUEVIEW_ARM_SIZE	262142
