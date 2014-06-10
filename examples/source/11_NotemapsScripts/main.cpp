
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

#define NOTE_INTENSITY  96
#define NOTE_DURATION   6

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
    MSRange mTenorSaxRange(44, 75);
    MCInstrument mTenorSax(1, mTenorSaxRange, 1);

    // sound generator
    CAudio::init();
    MCSoundGenAudio* mSoundGen = CAudio::createSoundGen(1, mTenorSax.getNumberOfChannels(), false);
    sprintf(sFilename, InstrumentsPath, "TenorSax.msp");
    mSoundGen->loadSamplePack(sFilename);

    // score
    MCScore* mScore = new MCScore();

    // sax settings
    mTenorSax.setSoundGen(mSoundGen);
    mTenorSax.setScore(mScore);

    // timer
    MCTimer mTimer;
    mTimer.setCallbackTick(TimerTick, &mTenorSax);
    mTimer.start();

    // create music timer thread
#ifdef _WIN32
    InitializeCriticalSection(&cSection);
    HANDLE hMusicTimerThread = CreateThread(NULL, 0, MusicTimerThread, &mTimer, 0, NULL);
#else
    pthread_mutex_init(&pMutex, NULL);
    pthread_t hMusicTimerThread;
    pthread_create(&hMusicTimerThread, NULL, MusicTimerThread, &mTimer);
#endif

    // data
    char sRootNote[3];
    char sScript[1024];

    MTNote mRootNote;
    MTNoteMap mChord;
    MTNoteMap mNotemap;

    MSTimePosition mTimePosition;
    int iSelected;

    sRootNote[2] = '\0';

    while(1)
    {
        do
        {
            cout << "\n\n  Root note (C, D, E, F, G, A, B). Enter 0 to quit: ";
            cin >> sRootNote[0];

            if(sRootNote[0] == '0')
                break;

        } while(toupper(sRootNote[0]) < 'A' || toupper(sRootNote[0]) > 'G');

        // quit
        if(sRootNote[0] == '0')
            break;

        do
        {
            cout << "  Accidental (-, #, b): ";
            cin >> sRootNote[1];

        } while(sRootNote[1] != '-' && sRootNote[1] != '#' && sRootNote[1] != 'b');

        mRootNote = MCNotes::fromString(sRootNote);

        // arpeggio selection
        cout << "  Select a chord:";
        cout << "\n    1) M";
        cout << "\n    2) m7";
        cout << "\n    3) 7";
        cout << "\n    4) dis7";
        cout << "\n    5) M+5";
        cout << "\n    6) 7sus4\n";

        do
        {
            cout << "    -> ";
            cin >> iSelected;

        } while(iSelected < 1 || iSelected > 6);

        cout << "  Playing arpeggio...";

        switch(iSelected)
        {
        case 1:
            mChord = MCChords::cM();
            break;
        case 2:
            mChord = MCChords::cm7();
            break;
        case 3:
            mChord = MCChords::c7();
            break;
        case 4:
            mChord = MCChords::cdis7();
            break;
        case 5:
            mChord = MCChords::cMaug5();
            break;
        case 6:
            mChord = MCChords::c7sus4();
            break;
        }

        // create a note map with all the notes which belong to the selected chord
        // inside the instrument's range
        mNotemap = MCNoteMaps::createNoteMap(mRootNote, mChord, mTenorSaxRange);

        // write a script
        MCScript::writeScale(sScript, mNotemap, NOTE_INTENSITY, 0, 0, NOTE_DURATION, mTimePosition, 4);

        // load score data and play
        mScore->loadScriptFromString(sScript);

#ifdef _WIN32
        EnterCriticalSection(&cSection);
        mTimer.reset();
        mTimer.start();
        LeaveCriticalSection(&cSection);
#else
        pthread_mutex_lock(&pMutex);
        mTimer.reset();
        mTimer.start();
        pthread_mutex_unlock(&pMutex);
#endif
    }

    // wait until the music timer thread finishes
    bThreadEnd = true;

#ifdef _WIN32
    WaitForSingleObject(hMusicTimerThread, INFINITE);
    CloseHandle(hMusicTimerThread);
    DeleteCriticalSection(&cSection);
#else
    pthread_join(hMusicTimerThread, NULL);
    pthread_mutex_destroy(&pMutex);
#endif

    delete mScore;
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
{
    MCTimer* mTimer = (MCTimer*)lp;

    while(!bThreadEnd)
    {
        EnterCriticalSection(&cSection);
        mTimer->update();
        LeaveCriticalSection(&cSection);
    }

    return 0;
}
#else
void* MusicTimerThread(void* lp)
{
    MCTimer* mTimer = (MCTimer*)lp;

    while(!bThreadEnd)
    {
        pthread_mutex_lock(&pMutex);
        mTimer->update();
        pthread_mutex_unlock(&pMutex);
    }

    return 0;
}
#endif

//
//  Timer tick callback
//
void TimerTick(const MSTimePosition& mTimePosition, void* pData)
{
    MCInstrument* mTenorSax = (MCInstrument*)pData;
    mTenorSax->update(mTimePosition);
}
