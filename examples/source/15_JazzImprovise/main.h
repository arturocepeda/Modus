
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

#include <windows.h>
#include "modus.h"
#include "./../moduslib.win32.h"

#include "mxhumanizer.h"
#include "mximproviser.h"

#define FPS             60
#define LOWEST_NOTE     21

#define TEMPO           132
#define MIN_TEMPO       80
#define MAX_TEMPO       210

#define LOOP            24
#define SCALE_PATTERNS  3
#define BASS_ENDING     2

#define PAD_BUTTONS     10

struct SGlobal
{
    bool bEnd;
    CRITICAL_SECTION cSection;

    Modus::MSNote mNote;
    int iCursor;
    int iLastPosition;

    char** sMIDIDevices;
    HMIDIIN hMIDIDevice;
    unsigned int iMIDINumDevices;
    unsigned int iMIDISelectedDevice;

    Modus::MCTimer* mMusicTimer;
    Modus::MCSongStructure* mStructure;

    Modus::MCHarmonyPattern mHarmonyPattern;
    unsigned int iHarmonyPatternPosition;
    
    Modus::MCScalePattern mImpScalePatternList[SCALE_PATTERNS];
    unsigned int iCurrentImpScalePattern;
    Modus::MCScalePattern& mImpScalePattern;
    unsigned int iImpScalePatternPosition;

    Modus::MCScalePattern mBassScalePattern;
    unsigned int iBassScalePatternPosition;

    Modus::MTNoteMap mNoteMap;
    Modus::MCBand mBand;

    Modus::MCInstrument* mTrombone;
    Modus::MCScore mTromboneScore;

    Modus::MCInstrument* mTenorSax;
    Modus::MCScore mTenorSaxScore;

    Modus::MCInstrument* mPiano;
    Modus::MSRange mPianoRange;
    Modus::MCSoundGenAudio* mPianoSoundGen;
    bool bKeyPressed[88];
    unsigned char iKeyIntensity[88];
    bool bDamper;

    Modus::MCInstrument* mBass;
    Modus::MCScore mBassScore;

    Modus::MCInstrument* mDrums;
    Modus::MCScore mDrumsScore;

    Modus::MCInstrument* mCount;
    Modus::MCScore mCountScore;

    int iWalkingMeasureFrom;
    int iWalkingMeasureTo;
    Modus::MCWalkingBassImproviser mBassImproviser;

    bool bEndingTheme;
    int iMeasureEndingTheme;

    SGlobal() : mImpScalePattern(mImpScalePatternList[0]) {}
};

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void CALLBACK MidiInProc(HMIDIIN mHandle, UINT iMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2);
DWORD WINAPI MusicTimerThread(LPVOID lp);

void ResetCursor(SGlobal* sGlobal);
void LoadScripts(SGlobal* sGlobal);
void SelectMIDIDevice(unsigned int iNumDevice, SGlobal* sGlobal);

void PlayPianoNote(int iPosition, SGlobal* sGlobal);
void PlayPianoVoicing(SGlobal* sGlobal);
void GoToEndingTheme(SGlobal* sGlobal);

void CallbackTick(const Modus::MSTimePosition& mPos, void* pData);
void CallbackEnd(void* pData);
void PianoPlayNote(unsigned int iInstrID, const Modus::MSNote& mNote, void* pData);
void PianoReleaseNote(unsigned int iInstrID, const Modus::MSNote& mNote, void* pData);
void PianoDamper(unsigned int iInstrID, bool bDamperState, void* pData);

#endif
