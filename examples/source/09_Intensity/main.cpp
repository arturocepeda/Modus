
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
    MSRange mTenorSaxRange(44, 75);
    MCInstrument mTenorSax(1, mTenorSaxRange, 1);

    // sound generator
    CAudio::init();
    MCSoundGenAudio* mSoundGen = CAudio::createSoundGen(1, mTenorSax.getNumberOfChannels(), false);
    sprintf(sFilename, InstrumentsPath, "TenorSax.msp");
    mSoundGen->loadSamplePack(sFilename);

    // sax settings
    mTenorSax.setSoundGen(mSoundGen);

    // timer
    MCTimer mTimer(120, 4);
    mTimer.setCallbackTick(TimerTick, &mTenorSax);
    mTimer.start();

    // create music timer thread
#ifdef _WIN32
    HANDLE hMusicTimerThread = CreateThread(NULL, 0, MusicTimerThread, &mTimer, 0, NULL);
#else
    pthread_t hMusicTimerThread;
    pthread_create(&hMusicTimerThread, NULL, MusicTimerThread, &mTimer);
#endif

    // intensity change parameters
    MSNote mNote;
    unsigned int iIntensityA;
    unsigned int iIntensityB;
    unsigned int iChangeTicks;

    cout << endl;

    while(1)
    {
        do
        {
            cout << "  Intensity [1, 127]. Enter 0 to quit: ";
            cin >> iIntensityA;

        } while(iIntensityA > 127);

        // quit
        if(iIntensityA == 0)
            break;

        do
        {
            cout << "  Final intensity [1, 127]: ";
            cin >> iIntensityB;

        } while(iIntensityB < 1 || iIntensityB > 127);

        do
        {
            cout << "  Intensity change duration in ticks [1, 96]: ";
            cin >> iChangeTicks;

        } while(iChangeTicks < 1 || iChangeTicks > 96);

        cout << "  Playing note...\n\n";

        memset(&mNote, 0, sizeof(MSNote));
        mNote.Pitch = 48;
        mNote.Intensity = iIntensityA;

        mTenorSax.playNote(mNote);
        mTenorSax.changeIntensity(0, iIntensityB, iChangeTicks);
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
    MCInstrument* mTenorSax = (MCInstrument*)pData;
    mTenorSax->update(mTimePosition);
}
