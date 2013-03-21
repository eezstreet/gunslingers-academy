#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <dbghelp.h>
#include <tlhelp32.h>
#include <psapi.h>
#include <time.h>

#include "q_shared.h"

// Windows version of the crash handler
LPTOP_LEVEL_EXCEPTION_FILTER oldHandler = 0;
// Used in case of a stack overflow
char StackBackup[0x18000];
unsigned int StackBackupStart;

const char *GSA_GetCrashlogName(void)
{
	char buffer[1024];
	time_t rawtime;
	struct tm * timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);
#ifdef CGAME
	strftime(buffer, 1024,"GSA-CLCrashlog_%Y-%m-%d_%H-%M-%S.log",timeinfo);
#elif defined QAGAME
	strftime(buffer, 1024,"GSA-GCrashlog_%Y-%m-%d_%H-%M-%S.log",timeinfo);
#elif defined UI
	strftime(buffer, 1024,"GSA-UICrashlog_%Y-%m-%d_%H-%M-%S.log",timeinfo);
#else
	strftime(buffer, 1024,"GSA-UCrashlog_%Y-%m-%d_%H-%M-%S.log",timeinfo);
	return buffer;
}

static LONG WINAPI UnhandledExceptionHandler(struct _EXCEPTION_POINTERS *EI)
{
	int bTerminate = 0;
	const char *fileName = GSA_GetCrashlogName();
	fileHandle_t f;

	if(bCrashing)
		bTerminate = 1;
	bCrashing = 1;

}

static LONG WINAPI UnhandledExceptionHandler_Failsafe(struct _EXCEPTION_POINTERS *EI)
{
	if(EI->ExceptionRecord->ExceptionCode == EXCEPTION_STACK_OVERFLOW) {
		// bf: Alright, we got a VERY serious issue here..
		// In this state the exception handler itself will run outta stack too
		// So we'll just use a nice hack here to roll up esp by 16k
		_asm {
			mov eax, EI
			mov StackBackupStart, esp
			mov esi, esp
			mov edi, offset StackBackup
			mov ecx, 0x6000
			rep stosd
			add esp, 0x18000
			push eax
			call UnhandledExceptionHandler
			jmp skip
		}
	}
	StackBackupStart=0;
	return UnhandledExceptionHandler(EI);
skip:
	;
}

void ActivateCrashHandler()
{
}

void DeactivateCrashHandler()
{
	if(!oldHandler) return;

}