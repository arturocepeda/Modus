
////////////////////////////////////////////////////////////////////////
//
//  Modus
//  C++ Music Library
//  (Examples)
//
//  Arturo Cepeda Pérez
//
////////////////////////////////////////////////////////////////////////

#include "./../platforms.h"
#include "modus.h"

#include "./../audio/audio.fmod.h"
#include "FMOD/mxsoundgenfmod.h"

#include <iostream>

using namespace std;

bool bThreadEnd = false;

void TimerTick(const MSTimePosition& mTimePosition, void* pData);

int main(int argc, char* argv[])
{
    // header
    cout << "\n  Modus " << MODUS_VERSION;
    cout << "\n  C++ Music Library";
    cout << "\n  Sample Application";

    // instrument
    MSRange mPianoRange(21, 108);
    MCInstrument mPiano(1, mPianoRange, mPianoRange.getSize());

    // score
    MCScore mScore;
    sprintf(sFilename, ScriptsPath, "score.piano.chopin.txt");
    mScore.loadScriptFromFile(sFilename);

    // sound generator
    CAudio::init();
    MCSoundGenAudio* mSoundGen = new MCSoundGenFMOD(mPiano.getNumberOfChannels(), false, CAudio::getSoundSystem());
    sprintf(sFilename, InstrumentsPath, "Piano.msp");
    mSoundGen->loadSamplePack(sFilename);

    // piano settings
    mPiano.setScore(&mScore);
    mPiano.setSoundGen(mSoundGen);

    // timer
    MCTimer mTimer;
    mTimer.setCallbackTick(TimerTick, &mPiano);
    mTimer.start();

    cout << "\n\n  Playing! Press any key to quit...\n\n";
    fflush(stdout);

    // message
    char sMessage[] = {"  The main thread is performing this simple task while the music thread\n"
                       "  plays the music!\n\n"};
    int iCurrentChar = 0;

    // create music timer thread
#ifdef _WIN32
    HANDLE hMusicTimerThread = CreateThread(NULL, 0, MusicTimerThread, &mTimer, 0, NULL);
#else
    pthread_t hMusicTimerThread;
    pthread_create(&hMusicTimerThread, NULL, MusicTimerThread, &mTimer);
#endif

    Input::init();

    while(!Input::keyPressed())
    {
        cout << sMessage[iCurrentChar];
        fflush(stdout);
        iCurrentChar++;

        if(iCurrentChar == strlen(sMessage))
            iCurrentChar = 0;

        wait(100);
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
    {
        if(mTimer->update())
            CAudio::update();
    }

    return 0;
}

//
//  Timer tick callback
//
void TimerTick(const MSTimePosition& mTimePosition, void* pData)
{
    MCInstrument* mPiano = (MCInstrument*)pData;
    mPiano->update(mTimePosition);
}
