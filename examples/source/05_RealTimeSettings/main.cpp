
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

#define LOOP 4

#define TEMPO       120.0f
#define MIN_TEMPO   60.0f
#define MAX_TEMPO   180.0f

using namespace std;

struct SGlobal
{
    MCInstrument* mDrums;
    MCScore mScore;
};

void TimerTick(const MSTimePosition& mTimePosition, void* pData);

int main(int argc, char* argv[])
{
    // header
    cout << "\n  Modus " << MODUS_VERSION;
    cout << "\n  C++ Music Library";
    cout << "\n  Sample Application\n\n";

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

    // real time settings
    float fTempo = TEMPO;
    char iIntensityChange = 0;

    // timer
    MCTimer mTimer(fTempo, 4);
    mTimer.setCallbackTick(TimerTick, &sGlobal);
    mTimer.start();

    cout << "\n  Playing! Options:\n";
    cout << "\n    W -> tempo +";
    cout << "\n    Q -> tempo -\n";
    cout << "\n    P -> intensity +";
    cout << "\n    O -> intensity -\n";
    cout << "\n    S -> play with sticks (default)";
    cout << "\n    B -> play with brushes\n";
    cout << "\n    ESC -> quit\n\n";
    fflush(stdout);

    int iLastKey = 0;
    unsigned int i;

    Input::init();

    while(iLastKey != 27)
    {
        // key pressed
        if(Input::keyPressed())
        {
            iLastKey = toupper(Input::getKey());

            switch(iLastKey)
            {
            // tempo +
            case 'W':

                if(fTempo < MAX_TEMPO)
                {
                    fTempo += 1.0f;
                    mTimer.setTempo(fTempo);
                }

                break;

            // tempo -
            case 'Q':

                if(fTempo > MIN_TEMPO)
                {
                    fTempo -= 1.0f;
                    mTimer.setTempo(fTempo);
                }

                break;

            // intensity +
            case 'P':

                if(iIntensityChange < 127)
                {
                    iIntensityChange++;
                    sGlobal.mDrums->setIntensityVariation(iIntensityChange);
                }

                break;

            // intensity -
            case 'O':

                if(iIntensityChange > -127)
                {
                    iIntensityChange--;
                    sGlobal.mDrums->setIntensityVariation(iIntensityChange);
                }

                break;

            // play with sticks
            case 'S':

                for(i = 0; i < sGlobal.mScore.getNumberOfEntries(); i++)
                    sGlobal.mScore[i]->Note.Mode = 0;

                break;

            // play with brushes
            case 'B':

                for(i = 0; i < sGlobal.mScore.getNumberOfEntries(); i++)
                    sGlobal.mScore[i]->Note.Mode = 1;

                break;
            }
        }

        // timer update
        if(mTimer.update())
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
    SGlobal* sGlobal = (SGlobal*)pData;

    // loop management
    if(mTimePosition.Measure % LOOP == 1 && mTimePosition.Measure > LOOP && 
       mTimePosition.Beat == 1 && mTimePosition.Tick == 0)
    {
        sGlobal->mScore.displace(LOOP);
    }

    // update
    sGlobal->mDrums->update(mTimePosition);
}
