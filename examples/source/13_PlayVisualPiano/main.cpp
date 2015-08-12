
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

#include "main.h"
#include "rendering.h"

#include "./../audio/audio.h"

#ifdef _MSC_VER
#pragma comment(lib, "winmm.lib")
#endif

using namespace Modus;

// rendering data
SGDI sGDI;

// main loop management
bool bEnd = false;

// MIDI receiver object
MCMIDIReceiver* mMIDIReceiver;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR sCmdLine, int iCmdShow)
{
    static char sAppName[] = "Modus Sample";
    HWND hWnd;
    MSG iMsg;
    WNDCLASSEX wndClass;

    // window size
    sGDI.WindowSizeX = 924;
    sGDI.WindowSizeY = 320;

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

    sGlobal.hMIDIDevice = NULL;
    sGlobal.iMIDINumDevices = 0;
    sGlobal.iMIDISelectedDevice = 0;

    sGlobal.mPiano = NULL;
    memset(sGlobal.bKeyPressed, 0, 88);
    memset(sGlobal.iKeyIntensity, 0, 88);
    sGlobal.bDamper = false;

    // make global data accesible to WndProc()
    SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG)&sGlobal);

    // check for MIDI in devices
    MIDIINCAPS mCaps;
    unsigned int i;

    sGlobal.iMIDINumDevices = midiInGetNumDevs();

    if(sGlobal.iMIDINumDevices == 0)
    {
        MessageBox(hWnd, "No MIDI in device found!", "Modus Sample Application", 0);
        exit(1);
    }

    sGlobal.sMIDIDevices = new char*[sGlobal.iMIDINumDevices];

    for(i = 0; i < sGlobal.iMIDINumDevices; i++)
    {
        sGlobal.sMIDIDevices[i] = new char[128];
        midiInGetDevCaps(i, &mCaps, sizeof(MIDIINCAPS));
        strcpy(sGlobal.sMIDIDevices[i], mCaps.szPname);
    }

    // MIDI listener
    SelectMIDIDevice(0, &sGlobal);

    // instrument
    MSRange mPianoRange(21, 108);
    sGlobal.mPiano = new MCInstrument(1, mPianoRange, mPianoRange.getSize());

    // MIDI receiver
    mMIDIReceiver = new MCMIDIReceiver(sGlobal.mPiano);
    mMIDIReceiver->listenToAllMIDIChannels();
    mMIDIReceiver->attachAllPitchesToDifferentChannels();

    // sound generator
    CAudio::init();
    MCSoundGenAudio* mSoundGen = CAudio::createSoundGen(1, sGlobal.mPiano->getNumberOfChannels(), false);
    mSoundGen->loadSamplePack(".\\..\\..\\common\\instruments\\Piano.msp");

    // instrument settings
    sGlobal.mPiano->setSoundGen(mSoundGen);
    sGlobal.mPiano->setCallbackPlay(PianoPlayNote, &sGlobal);
    sGlobal.mPiano->setCallbackRelease(PianoReleaseNote, &sGlobal);
    sGlobal.mPiano->setCallbackDamper(PianoDamper, &sGlobal);

    // music timer
    MCTimer* mMusicTimer = new MCTimer();
    mMusicTimer->setCallbackTick(TimerTick, &sGlobal);
    mMusicTimer->start();

    // create music timer thread
    HANDLE hMusicTimerThread = CreateThread(NULL, 0, MusicTimerThread, mMusicTimer, 0, NULL);

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
        
        // update and render
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

    // close MIDI handler
    midiInStop(sGlobal.hMIDIDevice);
    midiInClose(sGlobal.hMIDIDevice);    

    // release objects
    for(i = 0; i < sGlobal.iMIDINumDevices; i++)
        delete[] sGlobal.sMIDIDevices[i];

    delete[] sGlobal.sMIDIDevices;

    delete mMIDIReceiver;
    delete sGlobal.mPiano;
    delete mSoundGen;
    delete mMusicTimer;
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
        // ESC key
        case VK_ESCAPE:

            bEnd = true;

            break;

        // 'D' key
        case 'D':

            sGlobal->iMIDISelectedDevice++;

            if(sGlobal->iMIDISelectedDevice == sGlobal->iMIDINumDevices)
                sGlobal->iMIDISelectedDevice = 0;

            SelectMIDIDevice(sGlobal->iMIDISelectedDevice, sGlobal);

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
//  MIDI in callback
//
void CALLBACK MidiInProc(HMIDIIN mHandle, UINT iMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
{
    if(iMsg == MIM_DATA)
        mMIDIReceiver->message32inv(dwParam1);
}


//
//  Thread function for the music timer
//
DWORD WINAPI MusicTimerThread(LPVOID lp)
{
    MCTimer* mMusicTimer = (MCTimer*)lp;

    while(!bEnd)
        mMusicTimer->update();

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
//  MIDI in device selection
//
void SelectMIDIDevice(unsigned int iNumDevice, SGlobal* sGlobal)
{
    if(sGlobal->hMIDIDevice)
    {
        midiInStop(sGlobal->hMIDIDevice);
        midiInClose(sGlobal->hMIDIDevice);
    }

    sGlobal->iMIDISelectedDevice = iNumDevice;

    midiInOpen(&sGlobal->hMIDIDevice, iNumDevice, (DWORD)MidiInProc, 0, CALLBACK_FUNCTION);
    midiInStart(sGlobal->hMIDIDevice);
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
