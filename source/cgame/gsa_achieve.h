#pragma once
#include "q_shared.h"
#include "cg_local.h"

#ifdef ACHIEVEMENTS
#define MAX_ACHIEVEMENT_VARS	64
#define MAX_ACHIEVEMENTS		256

iAchieve_t *thisAchieve;

achieve_t GSA_achieveList[A_MAX];

void GSA_BasicAchieveFunc(qboolean load);
void GSA_BasicAchieveFunc_NS(qboolean load);
#endif