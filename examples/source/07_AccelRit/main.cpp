
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

#define LOOP        4
#define TEMPO_A     80.0f
#define TEMPO_B     140.0f

using namespace std;

struct SGlobal
{
    MCInstrument* mDrums;
    MCScore mScore;
    MCTimer mTimer;
    MCSongStructure mStructure;
};

void TimerTick(const MSTimePosition& mTimePosition, void* pData);

int main(int argc, char* argv[])
{
    // header
    cout << "\n  Modus " << MODUS_VERSION;
    cout << "\n  C++ Music Library";
    cout << "\n  Sample Application\n";

    // global data
    SGlobal sGlobal;

    // instrument
    MSRange mDrumsRange(35, 59);
    sGlobal.mDrums = new MCInstrument(1, mDrumsRange, 16);

    // score
    sprintf(sFilename, ScriptsPath, "score.drums.sample.txt");
    sGlobal.mScore.loadScriptFromFile(sFilename);

    // sound generator
    CAudio::init();
    MCSoundGenAudio* mSoundGen = CAudio::createSoundGen(1, sGlobal.mDrums->getNumberOfChannels(), false);
    sprintf(sFilename, InstrumentsPath, "Drums.msp");
    mSoundGen->loadSamplePack(sFilename);

    // drums settings
    sGlobal.mDrums->setScore(&sGlobal.mScore);
    sGlobal.mDrums->setSoundGen(mSoundGen);

    // song structure object
    MSSongSection mFirstSection;
    mFirstSection.FirstMeasure = 1;
    mFirstSection.LastMeasure = LOOP;
    mFirstSection.Tempo = TEMPO_A;
    mFirstSection.TargetTempo = TEMPO_B;
    mFirstSection.BeatsPerMeasure = 4;

    sGlobal.mStructure.addEntry(mFirstSection);

    // timer
    sGlobal.mTimer.setSongStructure(&sGlobal.mStructure);
    sGlobal.mTimer.setCallbackTick(TimerTick, &sGlobal);
    sGlobal.mTimer.start();

    cout << "\n  Playing! Press any key to quit\n";
    cout << "\n  Accelerando...";
    fflush(stdout);

    Input::init();

    while(!Input::keyPressed())
    {
        if(sGlobal.mTimer.update())
            CAudio::update();
    }

    Input::close();

    delete sGlobal.mDrums;
    delete mSoundGen;

    CAudio::release();

    cout << "\n\n";

    return 0;
}

void TimerTick(const MSTimePosition& mTimePosition, void* pData)
{
    static bool bAccelRit = false;

    SGlobal* sGlobal = (SGlobal*)pData;

    // loop management
    if(mTimePosition.Measure % LOOP == 0 && mTimePosition.Beat == 4 && mTimePosition.Tick == M_LAST_TICK)
    {
        sGlobal->mScore.displace(LOOP);

        MSSongSection mSection;
        mSection.FirstMeasure = mTimePosition.Measure + 1;
        mSection.LastMeasure = mTimePosition.Measure + LOOP;
        mSection.BeatsPerMeasure = 4;
        
        // accelerando
        if(bAccelRit)
        {
            mSection.Tempo = TEMPO_A;
            mSection.TargetTempo = TEMPO_B;

            cout << "\n  Accelerando...";
            fflush(stdout);
        }

        // ritardando
        else
        {
            mSection.Tempo = TEMPO_B;
            mSection.TargetTempo = TEMPO_A;

            cout << "\n  Ritardando...";
            fflush(stdout);
        }

        sGlobal->mStructure.addEntry(mSection);
        bAccelRit = !bAccelRit;
    }

    // update
    sGlobal->mDrums->update(mTimePosition);
}
