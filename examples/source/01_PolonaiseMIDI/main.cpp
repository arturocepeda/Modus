
////////////////////////////////////////////////////////////////////////
//
//  Modus
//  C++ Music Library
//  (Examples)
//
//  Arturo Cepeda
//  July 2012
//
//  --- main.cpp ---
//
////////////////////////////////////////////////////////////////////////

#include "./../platforms.h"
#include "modus.h"

#include "./../midi.h"
#include "MIDI/mxsoundgenmidi.h"

#include <iostream>

using namespace std;

void TimerTick(const MSTimePosition& mTimePosition, void* pData);

int main(int argc, char* argv[])
{
    // instrument
    MSRange mPianoRange = {21, 108};
    MCInstrument* mPiano = new MCInstrument(1, mPianoRange, mPianoRange.getSize());

    // score
    MCScore* mScore = new MCScore();
    sprintf(sFilename, ScriptsPath, "score.piano.chopin.txt");
    mScore->loadScriptFromFile(sFilename);

    // header
    cout << "\n  Modus " << MODUS_VERSION;
    cout << "\n  C++ Music Library";
    cout << "\n  Sample Application";

    // list of MIDI out devices
    RtMidiOut* mDevice;
    unsigned int iNumDevices;
    unsigned int iSelectedDevice;
    unsigned int i;

    mDevice = new RtMidiOut(rtMidiApi, "Modus Sample");
    iNumDevices = mDevice->getPortCount();

    if(iNumDevices == 0)
    {
        cout << "\n\n  ERROR: no MIDI out devices found\n\n";
        cin.get();

        return 0;
    }

    cout << "\n\n  Select a MIDI out device:\n";

    for(i = 0; i < iNumDevices; i++)
        cout << "\n  " << i + 1 << ") " << mDevice->getPortName(i);

    cout << "\n\n";

    do
    {
        cout << "  -> ";
        cin >> iSelectedDevice;

    } while(iSelectedDevice < 1 || iSelectedDevice > iNumDevices);

    iSelectedDevice--;

    // sound generator
    mDevice->openPort(iSelectedDevice);
    MCSoundGenMIDI* mSoundGen = new MCSoundGenMIDI(mPiano->getNumberOfChannels(), mDevice, 1, 1);

    // piano settings
    mPiano->setScore(mScore);
    mPiano->setSoundGen(mSoundGen);

    // timer
    MCTimer* mTimer = new MCTimer();
    mTimer->setCallbackTick(TimerTick, mPiano);
    mTimer->start();

    cout << "\n  Playing! Press any key to quit...";
    fflush(stdout);

    Input::init();

    while(!Input::keyPressed())
        mTimer->update();

    Input::close();

    delete mPiano;
    delete mScore;
    delete mSoundGen;
    delete mTimer;

    mDevice->closePort();
    delete mDevice;

    cout << "\n\n";

    return 0;
}

void TimerTick(const MSTimePosition& mTimePosition, void* pData)
{
    MCInstrument* mPiano = (MCInstrument*)pData;
    mPiano->update(mTimePosition);
}
