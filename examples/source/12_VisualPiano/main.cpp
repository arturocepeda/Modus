
////////////////////////////////////////////////////////////////////////
//
//  Modus
//  C++ Music Library
//  (Examples)
//
//  Arturo Cepeda P�rez
//  July 2012
//
////////////////////////////////////////////////////////////////////////

#include "main.h"
#include "rendering.h"

#include "./../audio/audio.h"

using namespace Modus;

// rendering data
SGDI sGDI;

// loop management
bool bEnd = false;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR sCmdLine, int iCmdShow)
{
    static char sAppName[] = "Modus Sample";
    HWND hWnd;
    MSG iMsg;
    WNDCLASSEX wndClass;

    // window size
    sGDI.WindowSizeX = 924;
    sGDI.WindowSizeY = 280;

    // class properties
    wndClass.cbSize = sizeof(wndClass);
    wndClass.style = CS_HREDRAW | CS_VREDRAW;
    wndClass.lpfnWndProc = WndProc;
    wndClass.cbClsExtra = 0;
    wndClass.cbWndExtra = 0;
    wndClass.hInstance = hInstance;
    wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndClass.hbrBackground = (HBRUSH)COLOR_BACKGROUND;
    wndClass.lpszMenuName = NULL;
    wndClass.lpszClassName = sAppName;
    wndClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    // register the class
    RegisterClassEx(&wndClass);

    // create main window
    hWnd = CreateWindow(sAppName, "Modus - C++ Music Library",
                        WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
                        CW_USEDEFAULT, CW_USEDEFAULT, sGDI.WindowSizeX, sGDI.WindowSizeY,
                        NULL, NULL, hInstance, NULL);

    // store window's handle
    sGDI.hWnd = hWnd;

    // initialize global data
    SGlobal sGlobal;

    sGlobal.mPiano = NULL;
    memset(sGlobal.bKeyPressed, 0, 88);
    memset(sGlobal.iKeyIntensity, 0, 88);
    sGlobal.bDamper = false;

    // make global data accesible to WndProc()
    SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG)&sGlobal);

    // instrument
    MSRange mPianoRange(21, 108);
    sGlobal.mPiano = new MCInstrument(1, mPianoRange, mPianoRange.getSize());

    // scores
    sGlobal.mScores = new MCScore[SCORES];

    sGlobal.mScores[0].loadScriptFromFile(".\\..\\..\\common\\scripts\\score.piano.chopin.txt");
    sGlobal.mScores[1].loadScriptFromFile(".\\..\\..\\common\\scripts\\score.piano.chopin2.txt");
    sGlobal.mScores[1].displace(1);

    sGlobal.iCurrentScore = 0;

    // sound generator
    CAudio::init();
    MCSoundGenAudio* mSoundGen = CAudio::createSoundGen(1, sGlobal.mPiano->getNumberOfChannels(), false);
    mSoundGen->loadSamplePack(".\\..\\..\\common\\instruments\\Piano.msp");

    // instrument settings
    sGlobal.mPiano->setScore(&sGlobal.mScores[sGlobal.iCurrentScore]);
    sGlobal.mPiano->setSoundGen(mSoundGen);

    sGlobal.mPiano->setCallbackPlay(PianoPlayNote, &sGlobal);
    sGlobal.mPiano->setCallbackRelease(PianoReleaseNote, &sGlobal);
    sGlobal.mPiano->setCallbackDamper(PianoDamper, &sGlobal);

    // initalize critical section
    InitializeCriticalSection(&sGlobal.cSection);

    // music timer
    sGlobal.mMusicTimer = new MCTimer(TEMPO, 4);
    sGlobal.mMusicTimer->setCallbackTick(TimerTick, &sGlobal);
    sGlobal.mMusicTimer->start();

    // create music timer thread
    HANDLE hMusicTimerThread = CreateThread(NULL, 0, MusicTimerThread, &sGlobal, 0, NULL);

    // loop timer
    Timer* mLoopTimer = new Timer();
    mLoopTimer->start();

    double dTimeInterval = 1000000.0 / FPS;
    double dTimeDelta = 0;
    double dTimeBefore = 0;
    double dTimeNow;

    // initialize rendering system
    CRendering::init(&sGDI);

    // show window
    ShowWindow(hWnd, iCmdShow);

    // main loop
    while(!bEnd)
    {
        // input
        if(PeekMessage(&iMsg, NULL, 0, 0, PM_REMOVE)) 
        {
            TranslateMessage(&iMsg);
            DispatchMessage(&iMsg);
        }
        
        // render
        dTimeNow = mLoopTimer->getTime();
        dTimeDelta = dTimeNow - dTimeBefore;

        if(dTimeDelta >= dTimeInterval)
        {
            dTimeBefore = dTimeNow;
            RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE);
        }
    }

    // wait until the music timer thread finishes
    WaitForSingleObject(hMusicTimerThread, INFINITE);
    CloseHandle(hMusicTimerThread);

    DeleteCriticalSection(&sGlobal.cSection);

    delete sGlobal.mPiano;
    delete[] sGlobal.mScores;
    delete mSoundGen;
    delete sGlobal.mMusicTimer;
    delete mLoopTimer;

    CRendering::release();
    CAudio::release();

    return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    PAINTSTRUCT ps;

    SGlobal* sGlobal = (SGlobal*)GetWindowLongPtr(hWnd, GWLP_USERDATA);

    switch(iMsg)
    {
    case WM_PAINT:

        hdc = BeginPaint(hWnd, &ps);
        CRendering::render(sGlobal);
        BitBlt(ps.hdc, 0, 0, sGDI.ClientX, sGDI.ClientY, sGDI.hdcmem, 0, 0, SRCCOPY); 
        EndPaint(hWnd, &ps);
        
        return 0;

    case WM_KEYDOWN:

        switch(wParam)
        {
        // ESC
        case VK_ESCAPE:
            bEnd = true;
            break;

        // 'P'
        case 'P':

            sGlobal->iCurrentScore++;

            if(sGlobal->iCurrentScore == SCORES)
                sGlobal->iCurrentScore = 0;

            sGlobal->mPiano->releaseAll();
            sGlobal->mPiano->setScore(&sGlobal->mScores[sGlobal->iCurrentScore]);
            sGlobal->bDamper = false;

            // synchronized section
            EnterCriticalSection(&sGlobal->cSection);

            sGlobal->mMusicTimer->reset();
            sGlobal->mMusicTimer->start();

            LeaveCriticalSection(&sGlobal->cSection);

            break;
        }

        return 0;

    case WM_CLOSE:

        bEnd = true;

        return 0;
    }

    return DefWindowProc(hWnd, iMsg, wParam, lParam);
}


//
//  Thread function for the music timer
//
DWORD WINAPI MusicTimerThread(LPVOID lp)
{
    SGlobal* sGlobal = (SGlobal*)lp;

    while(!bEnd)
    {
        // synchronized section
        EnterCriticalSection(&sGlobal->cSection);

        if(sGlobal->mMusicTimer->update())
            CAudio::update();

        LeaveCriticalSection(&sGlobal->cSection);
    }

    return 0;
}


//
//  Timer tick callback
//
void TimerTick(const MSTimePosition& mTimePosition, void* pData)
{
    SGlobal* sGlobal = (SGlobal*)pData;
    sGlobal->mPiano->update(mTimePosition);
}


//
//  Piano callbacks
//
void PianoPlayNote(unsigned int iInstrID, const MSNote& mNote, void* pData)
{
    SGlobal* sGlobal = (SGlobal*)pData;
    sGlobal->bKeyPressed[mNote.Pitch - LOWEST_NOTE] = true;
    sGlobal->iKeyIntensity[mNote.Pitch - LOWEST_NOTE] = mNote.Intensity;
}

void PianoReleaseNote(unsigned int iInstrID, const MSNote& mNote, void* pData)
{
    SGlobal* sGlobal = (SGlobal*)pData;
    sGlobal->bKeyPressed[mNote.Pitch - LOWEST_NOTE] = false;
}

void PianoDamper(unsigned int iInstrID, bool bDamperState, void* pData)
{
    SGlobal* sGlobal = (SGlobal*)pData;
    sGlobal->bDamper = bDamperState;
}
