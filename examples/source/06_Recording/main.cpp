
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
#include "./../midi.h"

#include "./../audio/audio.openal.h"
#include "OpenAL/mxsoundgenopenal.h"
#include "OpenAL/mxopenalsourcemanager.h"

#include <iostream>

using namespace std;

MCMIDIReceiver* mMIDIReceiver;

int main(int argc, char* argv[])
{
    // instrument
    MSRange mPianoRange = {21, 108};
    MCInstrument* mPiano = new MCInstrument(1, mPianoRange, mPianoRange.getSize());

    // header
    cout << "\n  Modus " << MODUS_VERSION;
    cout << "\n  C++ Music Library";
    cout << "\n  Sample Application";

    // global data
    SGlobal sGlobal;

    // list of MIDI in devices
    std::string sDevice;
    unsigned int iNumDevices;
    unsigned int iSelectedDevice;
    unsigned int i;
    
    RtMidiIn* mDevice = new RtMidiIn(rtMidiApi, "Modus Sample");
    iNumDevices = mDevice->getPortCount();
    
    if(iNumDevices == 0)
    {
        cout << "\n\n  ERROR: no MIDI in devices found\n\n";
        cin.get();
        return 0;
    }
    
    cout << "\n\n  Select a MIDI in device:\n";
    
    for(i = 0; i < iNumDevices; i++)
    {
        sDevice = mDevice->getPortName(i);
        cout << "\n  " << i + 1 << ") " << sDevice;
    }
    
    cout << "\n\n";

    do
    {
        cout << "  -> ";
        cin >> iSelectedDevice;

    } while(iSelectedDevice < 1 || iSelectedDevice > iNumDevices);

    iSelectedDevice--;

    // MIDI listener
    mDevice->openPort(iSelectedDevice);
    mDevice->setCallback(&MidiInProc);
    
    // Modus MIDI receiver
    mMIDIReceiver = new MCMIDIReceiver(mPiano);
    mMIDIReceiver->listenToAllMIDIChannels();
    mMIDIReceiver->attachAllPitchesToDifferentChannels();

    // sound generator
    CAudio::init();
    MCOpenALSourceManager* mALManager = new MCOpenALSourceManager(OPENAL_SOURCES);
    MCSoundGenOpenAL* mSoundGen = new MCSoundGenOpenAL(mPiano->getNumberOfChannels(), false, 1, mALManager);
    sprintf(sFilename, InstrumentsPath, "Piano.msp");
    mSoundGen->loadSamplePack(sFilename);
    mPiano->setSoundGen(mSoundGen);

    // score to write the notes
    sGlobal.mScore = new MCScore();

    // timer
    sGlobal.mTimer = new MCTimer();
    sGlobal.mTimer->setCallbackTick(TimerTick, mPiano);
    sGlobal.mTimer->start();

    // set piano callbacks
    mPiano->setCallbackPlay(PianoPlayNote, &sGlobal);
    mPiano->setCallbackRelease(PianoReleaseNote, &sGlobal);
    mPiano->setCallbackDamper(PianoDamper, &sGlobal);

    cout << "\n  Press any key to start recording. Press ESC to quit...";

    int iLastKey = 0;
    sGlobal.bRecordingMode = false;

    Input::init();

    while(iLastKey != 27)
    {
        // key pressed
        if(Input::keyPressed())
        {
            iLastKey = toupper(Input::getKey());

            // mode change
            if(iLastKey != 27)
            {
                sGlobal.bRecordingMode = !sGlobal.bRecordingMode;
                sGlobal.mTimer->reset();
                sGlobal.mTimer->start();

                if(sGlobal.bRecordingMode)
                {
                    sGlobal.mScore->clear();
                    mPiano->setScore(NULL);
                    mPiano->releaseAll();
                    cout << "\n\n  Recording! Press any key to hear. Press ESC to quit...";
                }
                else
                {
                    mPiano->setScore(sGlobal.mScore);
                    cout << "\n\n  Playing! Press any key to record again. Press ESC to quit...";
                }
            }
        }

        // timer update
        sGlobal.mTimer->update();
    }

    Input::close();

    delete sGlobal.mScore;
    delete sGlobal.mTimer;
    delete mPiano;
    delete mSoundGen;
    delete mALManager;
    delete mMIDIReceiver;

    delete mDevice;
    CAudio::release();

    cout << "\n\n";

    return 0;
}


//
//  timer tick callback
//
void TimerTick(const MSTimePosition& mTimePosition, void* pData)
{
    MCInstrument* mPiano = (MCInstrument*)pData;
    mPiano->update(mTimePosition);
}


//
//  MIDI in callback
//
void MidiInProc(double dTime, std::vector<unsigned char>* vMessage, void* pData)
{
    mMIDIReceiver->message(*vMessage);
}


//
//  piano callbacks
//
void PianoPlayNote(unsigned int iInstrID, const MSNote& mNote, void* pData)
{
    SGlobal* sGlobal = (SGlobal*)pData;
    
    // write to the score
    if(sGlobal->bRecordingMode)
    {
        MSScoreEntry se;
        se.Note = mNote;
        se.TimePosition = sGlobal->mTimer->getTimePosition();
        sGlobal->mScore->addEntry(se);
    }
}

void PianoReleaseNote(unsigned int iInstrID, const MSNote& mNote, void* pData)
{
    SGlobal* sGlobal = (SGlobal*)pData;
    
    // write to the score
    if(sGlobal->bRecordingMode)
    {
        MSScoreEntry se;
        se.Note = mNote;
        se.Note.Intensity = 0;  // note off
        se.TimePosition = sGlobal->mTimer->getTimePosition();
        sGlobal->mScore->addEntry(se);
    }
}

void PianoDamper(unsigned int iInstrID, bool bDamperState, void* pData)
{
    SGlobal* sGlobal = (SGlobal*)pData;
    
    // write to the score
    if(sGlobal->bRecordingMode)
    {
        MSScoreEntry se;        
        se.Note.Pitch = M_CTRL_DAMPER;
        se.Note.Intensity = (unsigned char)bDamperState;
        se.TimePosition = sGlobal->mTimer->getTimePosition();
        sGlobal->mScore->addEntry(se);
    }
}
