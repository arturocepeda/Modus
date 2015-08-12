
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

#include "./../platforms.h"
#include "modus.h"

#include "./../audio/audio.h"

#include <iostream>

#define LOOP 24

using namespace std;
using namespace Modus;

struct SGlobal
{
    MCInstrument* mTrumpet;
    MCScore mScore;
};

bool bThreadEnd = false;

void TimerTick(const MSTimePosition& mTimePosition, void* pData);

int main(int argc, char* argv[])
{
    // header
    cout << "\n  Modus " << MODUS_VERSION;
    cout << "\n  C++ Music Library";
    cout << "\n  Sample Application\n\n";

    // global data
    SGlobal sGlobal;

    // instrument
    MSRange mTrumpetRange(52, 84);
    sGlobal.mTrumpet = new MCInstrument(1, mTrumpetRange, 1);

    // sound generator
    CAudio::init();
    MCSoundGenAudio* mSoundGen = CAudio::createSoundGen(1, sGlobal.mTrumpet->getNumberOfChannels(), false);
    sprintf(sFilename, InstrumentsPath, "Trumpet.msp");
    mSoundGen->loadSamplePack(sFilename);

    // score
    sprintf(sFilename, ScriptsPath, "score.trumpet.sample.txt");
    sGlobal.mScore.loadScriptFromFile(sFilename);

    // instrument settings
    sGlobal.mTrumpet->setScore(&sGlobal.mScore);
    sGlobal.mTrumpet->setSoundGen(mSoundGen);

    // timer
    MCTimer mTimer(132, 4);
    mTimer.setCallbackTick(TimerTick, &sGlobal);
    mTimer.start();

    // create music timer thread
#ifdef _WIN32
    HANDLE hMusicTimerThread = CreateThread(NULL, 0, MusicTimerThread, &mTimer, 0, NULL);
#else
    pthread_t hMusicTimerThread;
    pthread_create(&hMusicTimerThread, NULL, MusicTimerThread, &mTimer);
#endif

    cout << "\n  Playing! Options:\n";
    cout << "\n    M -> change mode";
    cout << "\n    ESC -> quit\n\n";

    int iKey = 0;
    bool bMode = false;
    unsigned int i;

    Input::init();

    while(iKey != 27)
    {
        iKey = toupper(Input::getKey());

        if(iKey == 'M')
        {
            for(i = 0; i < sGlobal.mScore.getNumberOfEntries(); i++)
                sGlobal.mScore[i]->Note.Mode = (bMode)? 1: 0;

            bMode = !bMode;
        }

        wait(1);
    }

    Input::close();

    // wait until the music timer thread finishes
    bThreadEnd = true;

#ifdef _WIN32
    WaitForSingleObject(hMusicTimerThread, INFINITE);
    CloseHandle(hMusicTimerThread);
#else
    pthread_join(hMusicTimerThread, NULL);
#endif

    delete sGlobal.mTrumpet;
    delete mSoundGen;

    CAudio::release();

    cout << "\n\n";

    return 0;
}

//
//  Thread function for the music timer
//
#ifdef _WIN32
DWORD WINAPI MusicTimerThread(LPVOID lp)
#else
void* MusicTimerThread(void* lp)
#endif
{
    MCTimer* mTimer = (MCTimer*)lp;

    while(!bThreadEnd)
        mTimer->update();

    return 0;
}

//
//  Timer tick callback
//
void TimerTick(const MSTimePosition& mTimePosition, void* pData)
{
    SGlobal* sGlobal = (SGlobal*)pData;
    
    // loop management
    if(mTimePosition.Measure % LOOP == 1 && mTimePosition.Measure > LOOP && 
       mTimePosition.Beat == 1 && mTimePosition.Tick == 0)
    {    
        sGlobal->mScore.displace(LOOP);
    }

    // update instrument
    sGlobal->mTrumpet->update(mTimePosition);
}
