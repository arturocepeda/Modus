
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

#include "./../platforms.h"
#include "modus.h"

#include "./../midi.h"

#include "./../audio/audio.h"

#include <iostream>

using namespace std;
using namespace Modus;

MCMIDIReceiver* mMIDIReceiver;

void TimerTick(const MSTimePosition& mTimePosition, void* pData);
void MidiInProc(double dTime, std::vector<unsigned char>* vMessage, void* pData);

int main(int argc, char* argv[])
{
    // instrument
    MSRange mPianoRange(21, 108);
    MCInstrument mPiano(1, mPianoRange, mPianoRange.getSize());

    // header
    cout << "\n  Modus " << MODUS_VERSION;
    cout << "\n  C++ Music Library";
    cout << "\n  Sample Application";

    // list of MIDI in devices
    std::string sDevice;
    unsigned int iNumDevices;
    unsigned int iSelectedDevice = 0;

    RtMidiIn mDevice(rtMidiApi, "Modus Sample");
    iNumDevices = mDevice.getPortCount();

    cout << endl;

    if(iNumDevices == 0)
    {
        cout << "\n  ERROR: no MIDI in devices found\n\n";
        cin.get();

        return 0;
    }    
    else if(iNumDevices > 1)
    {
        cout << "\n  Select a MIDI in device:\n";

        for(unsigned int i = 0; i < iNumDevices; i++)
        {
            sDevice = mDevice.getPortName(i);
            cout << "\n  " << i + 1 << ") " << sDevice;
        }

        cout << "\n\n";

        do
        {
            cout << "  -> ";
            cin >> iSelectedDevice;

        } while(iSelectedDevice < 1 || iSelectedDevice > iNumDevices);

        iSelectedDevice--;
    }

    // MIDI listener
    mDevice.openPort(iSelectedDevice);
    mDevice.setCallback(&MidiInProc);
    
    // Modus MIDI receiver
    mMIDIReceiver = new MCMIDIReceiver(&mPiano);
    mMIDIReceiver->listenToAllMIDIChannels();
    mMIDIReceiver->attachAllPitchesToDifferentChannels();

    // sound generator
    CAudio::init();
    MCSoundGenAudio* mSoundGen = CAudio::createSoundGen(1, mPiano.getNumberOfChannels(), false);
    sprintf(sFilename, InstrumentsPath, "Piano.msp");
    mSoundGen->loadSamplePack(sFilename);
    mPiano.setSoundGen(mSoundGen);

    // timer
    MCTimer mTimer;
    mTimer.setCallbackTick(TimerTick, &mPiano);
    mTimer.start();

    cout << "\n  Listening, you can play now! Press any key to quit...";
    fflush(stdout);

    Input::init();

    while(!Input::keyPressed())
        mTimer.update();

    Input::close();

    delete mMIDIReceiver;
    delete mSoundGen;
    CAudio::release();

    cout << "\n\n";

    return 0;
}

void TimerTick(const MSTimePosition& mTimePosition, void* pData)
{
    MCInstrument* mPiano = (MCInstrument*)pData;
    mPiano->update(mTimePosition);
}

void MidiInProc(double dTime, std::vector<unsigned char>* vMessage, void* pData)
{
    mMIDIReceiver->message(*vMessage);
}
