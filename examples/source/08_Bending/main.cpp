
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

#include "./../audio/audio.openal.h"
#include "OpenAL/mxsoundgenopenal.h"
#include "OpenAL/mxopenalsourcemanager.h"

#include <iostream>

using namespace std;

bool bThreadEnd = false;

void TimerTick(const MSTimePosition& mTimePosition, void* pData);

int main(int argc, char* argv[])
{
    // header
    cout << "\n  Modus " << MODUS_VERSION;
    cout << "\n  C++ Music Library";
    cout << "\n  Sample Application\n\n";

    // instrument
    MSRange mDoubleBassRange(28, 60);
    MCInstrument mDoubleBass(1, mDoubleBassRange, 1);

    // sound generator
    CAudio::init();
    MCOpenALSourceManager* mManager = new MCOpenALSourceManager(OPENAL_SOURCES);
    MCSoundGenAudio* mSoundGen = new MCSoundGenOpenAL(mDoubleBass.getNumberOfChannels(), false, 1, mManager);
    sprintf(sFilename, InstrumentsPath, "DoubleBass.msp");
    mSoundGen->loadSamplePack(sFilename);

    // double bass settings
    mDoubleBass.setSoundGen(mSoundGen);

    // timer
    MCTimer mTimer;
    mTimer.setCallbackTick(TimerTick, &mDoubleBass);
    mTimer.start();

    // create music timer thread
#ifdef _WIN32
    HANDLE hMusicTimerThread = CreateThread(NULL, 0, MusicTimerThread, &mTimer, 0, NULL);
#else
    pthread_t hMusicTimerThread;
    pthread_create(&hMusicTimerThread, NULL, MusicTimerThread, &mTimer);
#endif

    // bending parameters
    MSNote mNote;
    int iBendingCents;
    unsigned int iBendingTicks;

    while(1)
    {
        do
        {
            cout << "  Bending cents [-1200, 1200]. Enter 0 to quit: ";
            cin >> iBendingCents;

        } while(iBendingCents < -1200 || iBendingCents > 1200);

        // quit
        if(iBendingCents == 0)
            break;

        do
        {
            cout << "  Bending duration in ticks [1, 96]: ";
            cin >> iBendingTicks;

        } while(iBendingTicks < 1 || iBendingTicks > 96);

        cout << "  Playing note and bending...\n\n";

        mNote.Pitch = 40;
        mNote.Intensity = 127;

        mDoubleBass.playNote(mNote);
        mDoubleBass.bend(0, iBendingCents, iBendingTicks);
    }

    // wait until the music timer thread finishes
    bThreadEnd = true;

#ifdef _WIN32
    WaitForSingleObject(hMusicTimerThread, INFINITE);
    CloseHandle(hMusicTimerThread);
#else
    pthread_join(hMusicTimerThread, NULL);
#endif

    delete mSoundGen;
    delete mManager;

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
    MCInstrument* mDoubleBass = (MCInstrument*)pData;
    mDoubleBass->update(mTimePosition);
}
