
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
    sGDI.WindowSizeY = 460;

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

    // class registration
    RegisterClassEx(&wndClass);

    // create the main window
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

    memset(sGlobal.bKeyPressed, 0, 88);
    memset(sGlobal.iKeyIntensity, 0, 88);

    sGlobal.bEnd = false;
    sGlobal.bDamper = false;

    // make global data accesible to WndProc()
    SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG)&sGlobal);

    // check for MIDI in devices
    MIDIINCAPS mCaps;
    unsigned int i;

    sGlobal.iMIDINumDevices = midiInGetNumDevs();

    if(sGlobal.iMIDINumDevices > 0)
    {
        sGlobal.sMIDIDevices = new char*[sGlobal.iMIDINumDevices];

        for(i = 0; i < sGlobal.iMIDINumDevices; i++)
        {
            sGlobal.sMIDIDevices[i] = new char[128];
            midiInGetDevCaps(i, &mCaps, sizeof(MIDIINCAPS));
            strcpy(sGlobal.sMIDIDevices[i], mCaps.szPname);
        }

        // MIDI listener
        SelectMIDIDevice(0, &sGlobal);
    }

    // sound system
    CAudio::init();

    // instruments
    MSRange mTromboneRange(40, 77);
    sGlobal.mTrombone = new MCInstrument(1, mTromboneRange, 1);
    sGlobal.mTrombone->setTranspose(-12);
    MCSoundGenAudio* mTromboneSoundGen = CAudio::createSoundGen(1, sGlobal.mTrombone->getNumberOfChannels(), true);
    mTromboneSoundGen->set3DPosition(2.0f, 0.0f, 0.1f);
    sGlobal.mTrombone->setSoundGen(mTromboneSoundGen);
    
    MSRange mTenorSaxRange(44, 75);
    sGlobal.mTenorSax = new MCInstrument(2, mTenorSaxRange, 1);
    sGlobal.mTenorSax->setTranspose(-12);
    MCSoundGenAudio* mTenorSaxSoundGen = CAudio::createSoundGen(2, sGlobal.mTenorSax->getNumberOfChannels(), true);
    mTenorSaxSoundGen->set3DPosition(-1.0f, 0.0f, 0.1f);
    sGlobal.mTenorSax->setSoundGen(mTenorSaxSoundGen);

    sGlobal.mPianoRange.LowestNote = 21;
    sGlobal.mPianoRange.HighestNote = 108;
    sGlobal.mPiano = new MCInstrument(3, sGlobal.mPianoRange, sGlobal.mPianoRange.getSize());
    MCSoundGenAudio* mPianoSoundGen = CAudio::createSoundGen(3, sGlobal.mPiano->getNumberOfChannels(), true);
    mPianoSoundGen->set3DPosition(0.0f, 0.0f, 0.0f);
    sGlobal.mPiano->setSoundGen(mPianoSoundGen);
    
    MSRange mBassRange(28, 60);
    sGlobal.mBass = new MCInstrument(4, mBassRange, 4);
    MCSoundGenAudio* mBassSoundGen = CAudio::createSoundGen(4, sGlobal.mBass->getNumberOfChannels(), true);
    mBassSoundGen->set3DPosition(0.0f, 0.0f, 0.0f);
    sGlobal.mBass->setSoundGen(mBassSoundGen);

    MSRange mDrumsRange(35, 59);
    sGlobal.mDrums = new MCInstrument(5, mDrumsRange, 8);
    MCSoundGenAudio* mDrumsSoundGen = CAudio::createSoundGen(5, sGlobal.mDrums->getNumberOfChannels(), true);
    mDrumsSoundGen->set3DPosition(0.0f, 0.0f, 0.0f);
    sGlobal.mDrums->setSoundGen(mDrumsSoundGen);

    MSRange mCountRange(1, 7);
    sGlobal.mCount = new MCInstrument(6, mCountRange, 2);
    MCSoundGenAudio* mCountSoundGen = CAudio::createSoundGen(6, sGlobal.mCount->getNumberOfChannels(), true);
    mCountSoundGen->set3DPosition(0.0f, 0.0f, 1.0f);
    sGlobal.mCount->setSoundGen(mCountSoundGen);

    sGlobal.mBand.addInstrument(sGlobal.mTrombone);
    sGlobal.mBand.addInstrument(sGlobal.mTenorSax);
    sGlobal.mBand.addInstrument(sGlobal.mPiano);
    sGlobal.mBand.addInstrument(sGlobal.mBass);
    sGlobal.mBand.addInstrument(sGlobal.mDrums);
    sGlobal.mBand.addInstrument(sGlobal.mCount);
    
    // MIDI receiver
    mMIDIReceiver = new MCMIDIReceiver(sGlobal.mPiano);
    mMIDIReceiver->listenToAllMIDIChannels();
    mMIDIReceiver->attachAllPitchesToDifferentChannels();
    
    // set range for the bass improviser
    sGlobal.mBassImproviser.setRange(mBassRange);

    // load all the scripts
    LoadScripts(&sGlobal);

    // set piano callbacks
    sGlobal.mPiano->setCallbackPlay(PianoPlayNote, &sGlobal);
    sGlobal.mPiano->setCallbackRelease(PianoReleaseNote, &sGlobal);
    sGlobal.mPiano->setCallbackDamper(PianoDamper, &sGlobal);

    // cursor initial position
    ResetCursor(&sGlobal);

    // main loop timer
    Timer* mLoopTimer = new Timer();
    mLoopTimer->start();
    sGlobal.mStructure = NULL;

    double dTimeInterval = 1000000.0 / FPS;
    double dTimeDelta = 0;
    double dTimeBefore = 0;
    double dTimeNow;

    // music timer
    sGlobal.mMusicTimer = new MCTimer(TEMPO, 4);

    // set timer callbacks
    sGlobal.mMusicTimer->setCallbackTick(CallbackTick, &sGlobal);
    sGlobal.mMusicTimer->setCallbackEnd(CallbackEnd, &sGlobal);

    // create music timer thread
    InitializeCriticalSection(&sGlobal.cSection);
    HANDLE hMusicTimerThread = CreateThread(NULL, 0, MusicTimerThread, &sGlobal, 0, NULL);

    // rendering system
    CRendering::init(&sGDI);

    // show window
    ShowWindow(hWnd, iCmdShow);

    // samples
    SetWindowText(hWnd, "Loading trombone samples...");
    mTromboneSoundGen->loadSamplePack(".\\..\\..\\common\\instruments\\Trombone.msp");
    mTromboneSoundGen->setVolume(0.3f);

    SetWindowText(hWnd, "Loading tenor sax samples...");
    mTenorSaxSoundGen->loadSamplePack(".\\..\\..\\common\\instruments\\TenorSax.msp");
    mTenorSaxSoundGen->setVolume(0.2f);

    SetWindowText(hWnd, "Loading piano samples...");
    mPianoSoundGen->loadSamplePack(".\\..\\..\\common\\instruments\\Piano.msp");
    mPianoSoundGen->setVolume(1.0f);

    SetWindowText(hWnd, "Loading double bass samples...");
    mBassSoundGen->loadSamplePack(".\\..\\..\\common\\instruments\\DoubleBass.msp");
    mBassSoundGen->setVolume(1.0f);

    SetWindowText(hWnd, "Loading drums samples...");
    mDrumsSoundGen->loadSamplePack(".\\..\\..\\common\\instruments\\Drums.msp");
    mDrumsSoundGen->setVolume(1.0f);

    SetWindowText(hWnd, "Loading count samples...");
    mCountSoundGen->loadSamplePack(".\\..\\..\\common\\instruments\\Count.msp");
    mCountSoundGen->setVolume(1.0f);

    SetWindowText(hWnd, "Modus - C++ Music Library");

    // main loop
    while(!sGlobal.bEnd)
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

    // close MIDI handler
    if(sGlobal.iMIDINumDevices > 0)
    {
        midiInStop(sGlobal.hMIDIDevice);
        midiInClose(sGlobal.hMIDIDevice); 
    }

    // release objects
    delete mMIDIReceiver;

    delete sGlobal.mTrombone;
    delete mTromboneSoundGen;

    delete sGlobal.mTenorSax;
    delete mTenorSaxSoundGen;

    delete sGlobal.mPiano;
    delete mPianoSoundGen;

    delete sGlobal.mBass;
    delete mBassSoundGen;

    delete sGlobal.mDrums;
    delete mDrumsSoundGen;

    delete sGlobal.mCount;
    delete mCountSoundGen;
    
    delete mLoopTimer;
    delete sGlobal.mMusicTimer;

    if(sGlobal.mStructure)
        delete sGlobal.mStructure;

    CAudio::release();
    CRendering::release();

    return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    PAINTSTRUCT ps;

    SGlobal* sGlobal = (SGlobal*)GetWindowLongPtr(hWnd, GWLP_USERDATA);

    switch(iMsg)
    {
    case WM_KEYDOWN:

        switch(wParam)
        {
        // quit
        case VK_ESCAPE:    
            sGlobal->bEnd = true;
            break;

        // play voicing
        case ' ':            
            PlayPianoVoicing(sGlobal);
            break;

        // play solo note
        case 'Q':
            PlayPianoNote(0, sGlobal);
            break;
        case 'W':
            PlayPianoNote(1, sGlobal);
            break;
        case 'E':
            PlayPianoNote(2, sGlobal);
            break;
        case 'R':
            PlayPianoNote(3, sGlobal);
            break;
        case 'T':
            PlayPianoNote(4, sGlobal);
            break;
        case 'Y':
            PlayPianoNote(5, sGlobal);
            break;
        case 'U':
            PlayPianoNote(6, sGlobal);
            break;
        case 'I':
            PlayPianoNote(7, sGlobal);
            break;
        case 'O':
            PlayPianoNote(8, sGlobal);
            break;
        case 'P':
            PlayPianoNote(9, sGlobal);
            break;

        // cursor
        case VK_RIGHT:
            if(sGlobal->mNoteMap[sGlobal->iCursor + PAD_BUTTONS - 1] < sGlobal->mPianoRange.HighestNote)
                sGlobal->iCursor++;
            break;
        case VK_LEFT:
            if(sGlobal->iCursor > 0)
                sGlobal->iCursor--;
            break;

        // scales pattern
        case '1':
        case '2':
        case '3':            
            sGlobal->iCurrentImpScalePattern = wParam - 49;     // 49: ASCII value for '1'
            sGlobal->mImpScalePattern = sGlobal->mImpScalePatternList[sGlobal->iCurrentImpScalePattern];
            break;

        // tempo
        case VK_UP:
            if(sGlobal->mMusicTimer->getTempo() < MAX_TEMPO)
                sGlobal->mMusicTimer->setTempo(sGlobal->mMusicTimer->getTempo() + 1.0f);
            break;
        case VK_DOWN:
            if(sGlobal->mMusicTimer->getTempo() > MIN_TEMPO)
                sGlobal->mMusicTimer->setTempo(sGlobal->mMusicTimer->getTempo() - 1.0f);
            break;

        // start!
        case 'S':
            if(!sGlobal->mMusicTimer->isRunning())
            {
                ResetCursor(sGlobal);
                EnterCriticalSection(&sGlobal->cSection);
                sGlobal->mMusicTimer->reset();
                LoadScripts(sGlobal);

                if(sGlobal->mStructure)
                {
                    delete sGlobal->mStructure;
                    sGlobal->mStructure = NULL;
                    sGlobal->mMusicTimer->setSongStructure(NULL);
                }

                sGlobal->mMusicTimer->setCountoff(2);
                sGlobal->mMusicTimer->start();
                LeaveCriticalSection(&sGlobal->cSection);
            }
            break;

        // go to the ending theme and finish
        case 'M':
            GoToEndingTheme(sGlobal);
            break;

        // change MIDI device
        case 'D':
            if(sGlobal->iMIDINumDevices > 0)
            {
                sGlobal->iMIDISelectedDevice++;

                if(sGlobal->iMIDISelectedDevice == sGlobal->iMIDINumDevices)
                    sGlobal->iMIDISelectedDevice = 0;

                SelectMIDIDevice(sGlobal->iMIDISelectedDevice, sGlobal);
            }
            break;
        }

        return 0;

    case WM_PAINT:

        hdc = BeginPaint(hWnd, &ps);
        CRendering::render(sGlobal);
        BitBlt(ps.hdc, 0, 0, sGDI.ClientX, sGDI.ClientY, sGDI.hdcmem, 0, 0, SRCCOPY); 
        EndPaint(hWnd, &ps);
        
        return 0;

    case WM_CLOSE:

        sGlobal->bEnd = true;

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
//  thread function for the music timer
//
DWORD WINAPI MusicTimerThread(LPVOID lp)
{
    SGlobal* sGlobal = (SGlobal*)lp;

    while(!sGlobal->bEnd)
    {
        EnterCriticalSection(&sGlobal->cSection);
        sGlobal->mMusicTimer->update();
        LeaveCriticalSection(&sGlobal->cSection);
    }

    return 0;
}


//
//  reset keyboard scope cursor
//
void ResetCursor(SGlobal* sGlobal)
{
    MTNote mTonic = sGlobal->mImpScalePattern[0]->RootNote;
    sGlobal->mNoteMap = MCNoteMaps::createNoteMap(mTonic, sGlobal->mImpScalePattern[0]->Scale, sGlobal->mPianoRange);
    sGlobal->iCursor = MCNoteMaps::getPositionNearestNote(60 + mTonic, sGlobal->mNoteMap);
    sGlobal->iLastPosition = sGlobal->iCursor;
    sGlobal->mNote.Pitch = 60 + mTonic;
}


//
//  load scripts
//
void LoadScripts(SGlobal* sGlobal)
{
    sGlobal->bEndingTheme = false;

    // harmony pattern definition
    sGlobal->mHarmonyPattern.loadScriptFromFile(".\\..\\..\\common\\scripts\\bags\\harmony.txt");
    sGlobal->iHarmonyPatternPosition = 0;

    // scale pattern definition 
    for(int i = 0; i < SCALE_PATTERNS; i++)
    {
        sprintf(sGDI.sBuffer, ".\\..\\..\\common\\scripts\\bags\\scales.improvisation.%d.txt", i);
        sGlobal->mImpScalePatternList[i].loadScriptFromFile(sGDI.sBuffer);
    }

    sGlobal->mImpScalePattern = sGlobal->mImpScalePatternList[0];
    sGlobal->iCurrentImpScalePattern = 0;
    sGlobal->iImpScalePatternPosition = 0;

    sGlobal->mBassScalePattern.loadScriptFromFile(".\\..\\..\\common\\scripts\\bags\\scales.bass.txt");
    sGlobal->iBassScalePatternPosition = 0;

    // trombone score definition
    sGlobal->mTromboneScore.loadScriptFromFile(".\\..\\..\\common\\scripts\\bags\\score.theme.trombone.txt");
    MCHumanizer::humanizeRandom(&sGlobal->mTromboneScore, 1, 16, 0, 1, 12, 4);
    sGlobal->mTrombone->setScore(&sGlobal->mTromboneScore); 

    // tenor sax score definition
    sGlobal->mTenorSaxScore.loadScriptFromFile(".\\..\\..\\common\\scripts\\bags\\score.theme.sax.txt");
    MCHumanizer::humanizeRandom(&sGlobal->mTenorSaxScore, 1, 16, 0, 1, 12, 4);
    sGlobal->mTenorSax->setScore(&sGlobal->mTenorSaxScore);

    // drums score definition
    sGlobal->mDrumsScore.loadScriptFromFile(".\\..\\..\\common\\scripts\\bags\\score.drums.txt");
    MCHumanizer::humanizeRandom(&sGlobal->mDrumsScore, 0, 8, 0, 1, 12, 4);
    sGlobal->mDrums->setScore(&sGlobal->mDrumsScore);

    // count score definition
    sGlobal->mCountScore.loadScriptFromFile(".\\..\\..\\common\\scripts\\bags\\score.count.txt");
    sGlobal->mCountScore.displace(-2);
    sGlobal->mCount->setScore(&sGlobal->mCountScore);

    // bass improvisation definition
    sGlobal->iWalkingMeasureFrom = 1;
    sGlobal->iWalkingMeasureTo = LOOP;
        
    sGlobal->mBassImproviser.setHarmonyPattern(&sGlobal->mHarmonyPattern);
    sGlobal->mBassImproviser.setScalePattern(&sGlobal->mBassScalePattern);

    sGlobal->mBassScore.clear();
    sGlobal->mBassImproviser.improvise(&sGlobal->mBassScore, sGlobal->iWalkingMeasureFrom, sGlobal->iWalkingMeasureTo);
    sGlobal->mBass->setScore(&sGlobal->mBassScore);
}


//
//  piano solo notes
//
void PlayPianoNote(int iPosition, SGlobal* sGlobal)
{
    if(!sGlobal->mMusicTimer->isRunning())
        return;
    
    // store last position used in the note map
    sGlobal->iLastPosition = iPosition + sGlobal->iCursor;

    // set note values
    sGlobal->mNote.Pitch = sGlobal->mNoteMap[iPosition + sGlobal->iCursor];
    sGlobal->mNote.Channel = sGlobal->mNote.Pitch - sGlobal->mPianoRange.LowestNote;
    sGlobal->mNote.Mode = 0;
    sGlobal->mNote.Duration = 8;

    // swing!
    sGlobal->mNote.Intensity = (sGlobal->mMusicTimer->getTimePosition().Tick >= 12 && 
                                sGlobal->mMusicTimer->getTimePosition().Tick <= 20)? 104: 64;
    sGlobal->mPiano->playNote(sGlobal->mNote);
}


//
//  piano voicings
//
void PlayPianoVoicing(SGlobal* sGlobal)
{
    if(!sGlobal->mMusicTimer->isRunning())
        return;

    // create voicing
    MSHarmonyPatternEntry* mCurrentChord = sGlobal->mHarmonyPattern[sGlobal->iHarmonyPatternPosition];
    MTNoteMap mVoicing = MCChords::createClosedVoicing(mCurrentChord->RootNote, 
                                                       mCurrentChord->Chord,
                                                       mCurrentChord->BassNote,
                                                       mCurrentChord->Tensions,
                                                       52 + rand() % 8, true, (rand() % 2) == 1);

    // set the channels
    unsigned char* iVoicingChannels = new unsigned char[mVoicing.size()];

    for(unsigned int i = 0; i < mVoicing.size(); i++)
        iVoicingChannels[i] = mVoicing[i] - sGlobal->mPianoRange.LowestNote;

    // set duration
    unsigned char iDuration;

    if(sGlobal->bEndingTheme && sGlobal->mMusicTimer->getTimePosition().Measure >= (sGlobal->iMeasureEndingTheme + LOOP))
        iDuration = 0;
    else
        iDuration = (sGlobal->mMusicTimer->getTimePosition().Beat % 2 == 0)? 54: 12;

    // play the voicing
    sGlobal->mPiano->playChord(mVoicing, 32, 0, iVoicingChannels, iDuration);

    delete[] iVoicingChannels;
}


//
//  go to ending theme
//
void GoToEndingTheme(SGlobal* sGlobal)
{
    if(sGlobal->bEndingTheme || sGlobal->mMusicTimer->getTimePosition().Measure <= LOOP)
        return;

    sGlobal->iMeasureEndingTheme = sGlobal->mMusicTimer->getTimePosition().Measure;
    sGlobal->iMeasureEndingTheme += LOOP - (sGlobal->iMeasureEndingTheme % LOOP);

    sGlobal->mTromboneScore.loadScriptFromFile(".\\..\\..\\common\\scripts\\bags\\score.theme.trombone.end.txt");
    sGlobal->mTromboneScore.displace(sGlobal->iMeasureEndingTheme);
    MCHumanizer::humanizeRandom(&sGlobal->mTromboneScore, 1, 16, 0, 1, 12, 4);

    sGlobal->mTenorSaxScore.loadScriptFromFile(".\\..\\..\\common\\scripts\\bags\\score.theme.sax.end.txt");
    sGlobal->mTenorSaxScore.displace(sGlobal->iMeasureEndingTheme);
    MCHumanizer::humanizeRandom(&sGlobal->mTenorSaxScore, 1, 16, 0, 1, 12, 4);

    // ending theme flag
    sGlobal->bEndingTheme = true;
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
//  timer callbacks
//
void CallbackTick(const MSTimePosition& mPos, void* pData)
{
    SGlobal* sGlobal = (SGlobal*)pData;

    // loop management: drums, harmony, scales and bass
    if(mPos.Measure % LOOP == 1 && mPos.Measure > LOOP && mPos.Beat == 1 && mPos.Tick == 0)
    {
        if(!sGlobal->bEndingTheme)
        {
            sGlobal->mDrumsScore.displace(LOOP);
        }
        else
        {
            sGlobal->mDrumsScore.loadScriptFromFile(".\\..\\..\\common\\scripts\\bags\\score.drums.end.txt");
            sGlobal->mDrumsScore.displace(sGlobal->iMeasureEndingTheme);

            // song structure
            sGlobal->mStructure = new MCSongStructure();
            MSSongSection mSection;

            mSection.FirstMeasure = sGlobal->iMeasureEndingTheme + 1;
            mSection.LastMeasure = sGlobal->iMeasureEndingTheme + LOOP + 1;
            mSection.Tempo = mSection.TargetTempo = sGlobal->mMusicTimer->getTempo();
            mSection.BeatsPerMeasure = 4;
            sGlobal->mStructure->addEntry(mSection);

            sGlobal->mMusicTimer->setSongStructure(sGlobal->mStructure);
        }

        sGlobal->mHarmonyPattern.displace(LOOP);
        sGlobal->mBassScalePattern.displace(LOOP);

        for(unsigned int i = 0; i < SCALE_PATTERNS; i++)
            sGlobal->mImpScalePatternList[i].displace(LOOP);

        sGlobal->iBassScalePatternPosition = 0;
        sGlobal->iHarmonyPatternPosition = 0;
        sGlobal->iImpScalePatternPosition = 0;

        // bass improvisation
        if(!sGlobal->bEndingTheme)
        {
            sGlobal->iWalkingMeasureFrom += LOOP;
            sGlobal->iWalkingMeasureTo += LOOP;

            sGlobal->mBassImproviser.setPreviousNote(sGlobal->mBassScore.getLastEntry()->Note.Pitch);
            sGlobal->mBassImproviser.improvise(&sGlobal->mBassScore, sGlobal->iWalkingMeasureFrom, 
                                               sGlobal->iWalkingMeasureTo);
            MCHumanizer::humanizeRandom(&sGlobal->mBassScore, 4, 32, 0, 1, LOOP, 4);
            sGlobal->mBass->setScore(&sGlobal->mBassScore);
        }
        else if(mPos.Measure == (sGlobal->iMeasureEndingTheme + 1))
        {
            sGlobal->iWalkingMeasureFrom += LOOP;
            sGlobal->iWalkingMeasureTo += LOOP - BASS_ENDING;

            sGlobal->mBassImproviser.setPreviousNote(sGlobal->mBassScore.getLastEntry()->Note.Pitch);
            sGlobal->mBassImproviser.improvise(&sGlobal->mBassScore, sGlobal->iWalkingMeasureFrom, 
                                               sGlobal->iWalkingMeasureTo);
            MCHumanizer::humanizeRandom(&sGlobal->mBassScore, 4, 32, 0, 1, LOOP, 4);

            MCScore mAuxScore;
            mAuxScore.loadScriptFromFile(".\\..\\..\\common\\scripts\\bags\\score.bass.end.txt");
            mAuxScore.displace(sGlobal->iMeasureEndingTheme);
            sGlobal->mBassScore.addScore(mAuxScore);
            sGlobal->mBass->setScore(&sGlobal->mBassScore);
        }
    }

    // update scale pattern
    if(mPos.Tick == 0)
    {
        // check for a new scale
        MSScalePatternEntry* spe = sGlobal->mImpScalePattern[sGlobal->iImpScalePatternPosition];
        
        if(spe && spe->TimePosition.Measure == mPos.Measure && spe->TimePosition.Beat == mPos.Beat)
        {
            sGlobal->mNoteMap = MCNoteMaps::createNoteMap(spe->RootNote, spe->Scale, sGlobal->mPianoRange);
            sGlobal->iImpScalePatternPosition++;

            // set the cursor position so that the same key generates the same note
            int iNewPosition = MCNoteMaps::getPositionNearestNote(sGlobal->mNote.Pitch, sGlobal->mNoteMap);
            sGlobal->iCursor = sGlobal->iCursor + iNewPosition - sGlobal->iLastPosition;

            // make sure that the value is inside the note map
            if(sGlobal->iCursor < 0)
                sGlobal->iCursor = 0;
            else if((sGlobal->iCursor + PAD_BUTTONS) >= (int)sGlobal->mNoteMap.size())
                sGlobal->iCursor = sGlobal->mNoteMap.size() - PAD_BUTTONS;
        }

        // update harmony pattern (one quarter note before)
        if(mPos.Beat % 2 == 0)
        {
            // check for a new chord
            MSHarmonyPatternEntry* hpe = sGlobal->mHarmonyPattern[sGlobal->iHarmonyPatternPosition + 1];

            if(hpe && hpe->TimePosition == mPos.getWithIncrement(MCSettings::getTicksPerBeat(), 4))
                sGlobal->iHarmonyPatternPosition++;
        }
    }

    // instruments update
    sGlobal->mBand.update(mPos);
}

void CallbackEnd(void* pData)
{
    SGlobal* sGlobal = (SGlobal*)pData;

    EnterCriticalSection(&sGlobal->cSection);
    sGlobal->mMusicTimer->stop();
    LeaveCriticalSection(&sGlobal->cSection);

    memset(sGlobal->bKeyPressed, 0, 88);
}


//
//  piano callbacks
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
