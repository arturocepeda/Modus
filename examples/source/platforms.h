
////////////////////////////////////////////////////////////////////////
//
//  Modus
//  C++ Music Library
//  (Examples)
//
//  Arturo Cepeda Pérez
//
//  --- platforms.h ---
//
////////////////////////////////////////////////////////////////////////


char sFilename[128];


//
//  Win32
//
#ifdef _WIN32

#include "input.win32.h"
#include "moduslib.win32.h"
#include <Windows.h>

const char ScriptsPath[] = ".\\..\\..\\common\\scripts\\%s";
const char InstrumentsPath[] = ".\\..\\..\\common\\instruments\\%s";

void wait(int milliseconds) { Sleep(milliseconds); }

CRITICAL_SECTION cSection;
DWORD WINAPI MusicTimerThread(LPVOID lp);


//
//  MacOS X / Linux
//
#else

#include "input.gcc.h"
#include <pthread.h>

const char ScriptsPath[] = "./../../common/scripts/%s";
const char InstrumentsPath[] = "./../../common/instruments/%s";

void wait(int milliseconds) { usleep(milliseconds * 1000); }

pthread_mutex_t pMutex;
void* MusicTimerThread(void* lp);

#endif
