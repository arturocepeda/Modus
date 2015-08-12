
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
#include <fstream>

using namespace std;
using namespace Modus;

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
    MCSoundGenAudio* mSoundGen = CAudio::createSoundGen(1, mPiano.getNumberOfChannels(), false);
    sprintf(sFilename, InstrumentsPath, "Piano.msp");
    mSoundGen->loadSamplePack(sFilename);

    // piano settings
    mPiano.setScore(&mScore);
    mPiano.setSoundGen(mSoundGen);

    // timer
    MCTimer mTimer;
    mTimer.setCallbackTick(TimerTick, &mPiano);
    mTimer.start();

    cout << "\n\n  Playing! Press any key to quit...";
    fflush(stdout);

    Input::init();

    while(!Input::keyPressed())
    {
        if(mTimer.update())
            CAudio::update();
    }

    Input::close();

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
