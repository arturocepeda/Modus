
////////////////////////////////////////////////////////////////////////
//
//  Modus
//  C++ Music Library
//  (Examples)
//
//  Arturo Cepeda Pérez
//  July 2012
//
////////////////////////////////////////////////////////////////////////

#ifndef _MAIN_H_
#define _MAIN_H_

#include <Windows.h>
#include "modus.h"
#include "./../moduslib.win32.h"

#define FPS 60

#define TEMPO           120
#define LOWEST_NOTE     21
#define SCORES          2

struct SGlobal
{
    Modus::MCTimer* mMusicTimer;
    Modus::MCInstrument* mPiano;
    Modus::MCScore* mScores;
    unsigned int iCurrentScore;

    bool bKeyPressed[88];
    unsigned char iKeyIntensity[88];
    bool bDamper;

    CRITICAL_SECTION cSection;
};

struct SGDI
{
    HWND hWnd;
    HDC hdcmem;
    RECT rArea;
    HBITMAP bmp;
    HBITMAP bmpOld;

    HFONT hFontText;
    HFONT hFontBold;
    HBRUSH hKeyBrush[128];

    HBITMAP bmpWhiteKey;
    HBITMAP bmpBlackKey;
    HBITMAP bmpPedalOn;
    HBITMAP bmpPedalOff;

    HDC hdcWhiteKey;
    HDC hdcBlackKey;
    HDC hdcPedalOn;
    HDC hdcPedalOff;

    int WindowSizeX;
    int WindowSizeY;

    int ClientX;
    int ClientY;
};

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam);
DWORD WINAPI MusicTimerThread(LPVOID lp);

void TimerTick(const Modus::MSTimePosition& mTimePosition, void* pData);
void PianoPlayNote(unsigned int iInstrID, const Modus::MSNote& mNote, void* pData);
void PianoReleaseNote(unsigned int iInstrID, const Modus::MSNote& mNote, void* pData);
void PianoDamper(unsigned int iInstrID, bool bDamperState, void* pData);

#endif
