
////////////////////////////////////////////////////////////////////////
//
//  Modus
//  C++ Music Library
//  [Tools]
//
//  Arturo Cepeda
//  July 2012
//
//  --------------------------------------------------------------------
//
//  midi2mscore
//
//  Tool to generate scores for Modus from MIDI files
//
////////////////////////////////////////////////////////////////////////

#include "midi2mscore.h"
#include "midifile.h"
#include "mconstants.h"

#include <iostream>
#include <fstream>

using namespace std;

int main(int argc, char* argv[])
{
    SMIDIConfig sConfig;

    unsigned int iAux;
    unsigned int i;

    // header
    cout << "\n  Modus " << MODUS_VERSION;
    cout << "\n  C++ Music Library";
    cout << "\n  MIDI-to-MScore tool\n";

    if(argc < 3)
    {
        cout << "\n  Usage: midi2mscore <MIDI_filename> <Script_filename> [options]\n";
        cout << "\n  Options:";
        cout << "\n            /i -> ignore note off messages\n" << endl;
        return -1;
    }

    sConfig.MIDIFile = argv[1];
    sConfig.ScriptFile = argv[2];
    sConfig.FirstMeasure = 1;
    sConfig.LastMeasure = 65536;
    sConfig.ScriptChannels = 0;

    bool bIgnoreNoteOff = false;

    // command line options
    for(i = 3; i < (unsigned int)argc; i++)
    {
        // ignore note off
        bIgnoreNoteOff = (strstr(argv[i], "/i") != NULL);
    }

    // object to read the MIDI file
    CMIDIFile cMIDIFile;

    cout << "\n  Reading MIDI data from \"" << sConfig.MIDIFile << "\"...";
    fflush(stdout);

    if(!cMIDIFile.openFile(sConfig.MIDIFile))
    {
        cerr << "\n  Cannot read MIDI data from \"" << sConfig.MIDIFile << "\"" << endl;
        return -1;
    }
    else
    {
        cout << "\n  Data readed successfully!" << endl;
    }

    // MIDI file: channels
    cout << "\n  MIDI channels: -\n  -> Add channel (0 to finish): ";
    cin >> iAux;

    while(iAux == 0)
    {
        cout << "  -> You must select at least one channel: ";
        cin >> iAux;
    }

    while(iAux != 0)
    {
        while(iAux > 16)
        {
            cout << "  -> Only values between 1 and 16: ";
            cin >> iAux;
        }

        sConfig.MIDIChannels.push_back(iAux - 1);

        cout << "\n  MIDI channels: ";

        for(i = 0; i < sConfig.MIDIChannels.size(); i++)
            cout << (int)(sConfig.MIDIChannels[i] + 1) << " ";

        cout << "\n  -> Add channel (0 to finish): ";
        cin >> iAux;
    }

    // script: number of channels
    while(sConfig.ScriptChannels == 0)
    {
        cout << "\n  Number of channels for the script (1-255): ";
        cin >> sConfig.ScriptChannels;
    }

    // new script file
    std::ofstream fScript(sConfig.ScriptFile);
    char sLineBuffer[1024];

    unsigned int iDuration;
    unsigned int iCurrentChannel = 0;
    unsigned char* iLastNoteOnChannel = new unsigned char[sConfig.ScriptChannels];

    for(i = 0; i < sConfig.ScriptChannels; i++)
        iLastNoteOnChannel[i] = 0;

    if(!fScript.is_open())
    {
        cerr << "\n  Cannot create script file \"" << sConfig.ScriptFile << "\"" << endl;
        return -1;
    }

    cout << "\n  Writing script..." << endl;

    // script header
    sprintf(sLineBuffer, 
            "\n;-------------------------------------------------------------------------\n;"
            "\n; %s (generated from %s)\n;"
            "\n;-------------------------------------------------------------------------\n"
            "\nmode:0", sConfig.ScriptFile, sConfig.MIDIFile);

    fScript.write(sLineBuffer, strlen(sLineBuffer));

    // MIDI file contents
    SMIDITimePosition sTimePosition;
    SMIDITimePosition sAuxTimePosition;
    SMIDITimePosition sLastTimePosition;

    SMIDITimeSignature sTimeSignatureChange;
    vector<SMIDITimeSignature> sTimeSignature;

    bool bNotePlayedBefore;
    bool bLastDamperOn = false;

    int iClocksPerBeat = cMIDIFile.getTimeDivision();

    SMIDIEvent* sCurrentEvent;
    unsigned int iCurrentEvent;

    SMIDIEvent* sNoteOffEvent;
    unsigned int iNoteOffEvent;

    // default time signature
    sTimeSignatureChange.Measure = 0;
    sTimeSignatureChange.BeatsPerMeasure = 4;
    sTimeSignature.push_back(sTimeSignatureChange);

    // go to the beginning
    sLastTimePosition.Measure = 0;
    sLastTimePosition.Beat = 0;
    sLastTimePosition.Tick = 0;

    iCurrentEvent = 0;
    
    while(iCurrentEvent < cMIDIFile.getNumberOfEvents())
    {
        sCurrentEvent = cMIDIFile.getEvent(iCurrentEvent);
        iCurrentEvent++;

        // current time position
        GetTimePosition(sCurrentEvent->Time, iClocksPerBeat, sTimeSignature, &sTimePosition);

        // new time signature change
        if(sCurrentEvent->Type == 0xF && sCurrentEvent->MIDIChannel == 0xF && sCurrentEvent->Parameter1 == 0x58)
        {
            sTimeSignatureChange.Measure = sTimePosition.Measure;
            sTimeSignatureChange.BeatsPerMeasure = sCurrentEvent->Parameter2;
            sTimeSignature.push_back(sTimeSignatureChange);

            sprintf(sLineBuffer, "\n\nbpm:%d", sTimeSignatureChange.BeatsPerMeasure);
            fScript.write(sLineBuffer, strlen(sLineBuffer));
        }

        // only inside the selected set of measures
        if(sTimePosition.Measure < sConfig.FirstMeasure)
            continue;

        if(sTimePosition.Measure > sConfig.LastMeasure)
            break;

        // damper control
        if(sCurrentEvent->Type == 0xB && sCurrentEvent->Parameter1 == 0x40)
        {
            if(sCurrentEvent->Parameter2 > 0)
            {
                if(!bLastDamperOn)
                {
                    bLastDamperOn = true;

                    // time position mark
                    if(sTimePosition.Measure != sLastTimePosition.Measure ||
                        sTimePosition.Beat != sLastTimePosition.Beat ||
                        sTimePosition.Tick != sLastTimePosition.Tick)
                    {
                        sprintf(sLineBuffer, "\n\n[m:%d b:%d t:%d]", sTimePosition.Measure, 
                                                                     sTimePosition.Beat, 
                                                                     sTimePosition.Tick);
                        fScript.write(sLineBuffer, strlen(sLineBuffer));

                        sLastTimePosition.Measure = sTimePosition.Measure;
                        sLastTimePosition.Beat = sTimePosition.Beat;
                        sLastTimePosition.Tick = sTimePosition.Tick;
                    }

                    // damper
                    sprintf(sLineBuffer, "\ni:1 p:129 >  ; damper on");
                    fScript.write(sLineBuffer, strlen(sLineBuffer));
                }
            }
            else
            {
                bLastDamperOn = false;

                // time position mark
                if(sTimePosition.Measure != sLastTimePosition.Measure ||
                    sTimePosition.Beat != sLastTimePosition.Beat ||
                    sTimePosition.Tick != sLastTimePosition.Tick)
                {
                    sprintf(sLineBuffer, "\n\n[m:%d b:%d t:%d]", sTimePosition.Measure, 
                                                                 sTimePosition.Beat, 
                                                                 sTimePosition.Tick);
                    fScript.write(sLineBuffer, strlen(sLineBuffer));

                    sLastTimePosition.Measure = sTimePosition.Measure;
                    sLastTimePosition.Beat = sTimePosition.Beat;
                    sLastTimePosition.Tick = sTimePosition.Tick;
                }

                // damper
                sprintf(sLineBuffer, "\ni:0 p:129 >  ; damper off");
                fScript.write(sLineBuffer, strlen(sLineBuffer));
            }

            continue;
        }

        // only note messages
        if(sCurrentEvent->Type != 0x8 && sCurrentEvent->Type != 0x9)
            continue;

        // only in the list of selected channels
        if(!ChannelIncluded(sCurrentEvent->MIDIChannel, sConfig.MIDIChannels))
            continue;

        // NOTE ON: parameter 1 -> note, parameter 2 -> velocity
        if(sCurrentEvent->Type == 0x9 && sCurrentEvent->Parameter2 > 0)
        {
            // time position mark
            if(sTimePosition.Measure != sLastTimePosition.Measure ||
               sTimePosition.Beat != sLastTimePosition.Beat ||
               sTimePosition.Tick != sLastTimePosition.Tick)
            {
                sprintf(sLineBuffer, "\n\n[m:%d b:%d t:%d]", sTimePosition.Measure, sTimePosition.Beat, sTimePosition.Tick);
                fScript.write(sLineBuffer, strlen(sLineBuffer));

                sLastTimePosition.Measure = sTimePosition.Measure;
                sLastTimePosition.Beat = sTimePosition.Beat;
                sLastTimePosition.Tick = sTimePosition.Tick;
            }

            // duration
            iDuration = 0;

            if(!bIgnoreNoteOff)
            {
                iNoteOffEvent = 0;

                // set the cursor in the same position than the current event
                do
                {
                    iNoteOffEvent++;
                    sNoteOffEvent = cMIDIFile.getEvent(iNoteOffEvent);

                } while(sNoteOffEvent->Time < sCurrentEvent->Time);

                // look for the corresponding note off event for the current event
                while(iNoteOffEvent < cMIDIFile.getNumberOfEvents())
                {
                    sNoteOffEvent = cMIDIFile.getEvent(iNoteOffEvent++);

                    // note off or note on with velocity 0
                    if(sNoteOffEvent->Type == 0x8 || (sNoteOffEvent->Type == 0x9 && sNoteOffEvent->Parameter2 == 0))
                    {
                        // the pitch is the one we are looking for
                        if(sNoteOffEvent->Parameter1 == sCurrentEvent->Parameter1)
                        {
                            GetTimePosition(sNoteOffEvent->Time, iClocksPerBeat, sTimeSignature, &sAuxTimePosition);

                            unsigned int iBeatsPerMeasure = sTimeSignature[sTimeSignature.size() - 1].BeatsPerMeasure;
                            unsigned int iPositionNoteOn = GetInTicks(sTimePosition, iBeatsPerMeasure);
                            unsigned int iPositionNoteOff = GetInTicks(sAuxTimePosition, iBeatsPerMeasure);

                            if((iPositionNoteOff - iPositionNoteOn) < 256)
                                iDuration = iPositionNoteOff - iPositionNoteOn;

                            break;
                        }
                    }
                }            
            }

            bNotePlayedBefore = false;

            for(i = 0; i < sConfig.ScriptChannels; i++)
            {
                if(iLastNoteOnChannel[i] == sCurrentEvent->Parameter1)
                {
                    bNotePlayedBefore = true;
                    sprintf(sLineBuffer, "\nch:%d i:%d d:%d p:%d >", i, sCurrentEvent->Parameter2, 
                                                                     iDuration, sCurrentEvent->Parameter1);
                    break;
                }
            }

            if(!bNotePlayedBefore)
            {
                sprintf(sLineBuffer, "\nch:%d i:%d d:%d p:%d >", iCurrentChannel, sCurrentEvent->Parameter2, 
                                                                 iDuration, sCurrentEvent->Parameter1);

                iLastNoteOnChannel[iCurrentChannel] = sCurrentEvent->Parameter1;
                iCurrentChannel++;

                if(iCurrentChannel == sConfig.ScriptChannels)
                    iCurrentChannel = 0;
            }

            fScript.write(sLineBuffer, strlen(sLineBuffer));
        }

    }
    
    fScript.write("\n", 1);
    fScript.close();

    delete[] iLastNoteOnChannel;

    cout << "  Script generated successfully!\n\n";
    
    return 0;
}

bool ChannelIncluded(unsigned char iChannel, const vector<unsigned char>& iListOfChannels)
{
    for(unsigned int i = 0; i < iListOfChannels.size(); i++)
    {
        if(iListOfChannels[i] == iChannel)
            return true;
    }

    return false;
}

void GetTimePosition(unsigned int iMIDITime, unsigned int iMIDIClocksPerBeat, 
                     const vector<SMIDITimeSignature>& sTimeSignature, SMIDITimePosition* sTimePosition)
{
    unsigned int iCurrentClock = 0;
    unsigned int iCurrentTimeSignature = 0;
    unsigned int iTicksRatio = iMIDIClocksPerBeat / M_TICKS_PER_BEAT;
    unsigned int i;
    
    sTimePosition->Measure = 1;
    sTimePosition->Beat = 1;
    sTimePosition->Tick = 0;

    if(sTimeSignature.size() == 0)
        return;

    while((iCurrentClock + iMIDIClocksPerBeat) < iMIDITime)
    {
        iCurrentClock += iMIDIClocksPerBeat;
        sTimePosition->Beat++;

        if(sTimePosition->Beat > sTimeSignature[iCurrentTimeSignature].BeatsPerMeasure)
        {
            sTimePosition->Beat = 1;
            sTimePosition->Measure++;

            // look for a time signature change
            for(i = (iCurrentTimeSignature + 1); i < sTimeSignature.size(); i++)
            {
                if(sTimeSignature[i].Measure == sTimePosition->Measure)
                {
                    iCurrentTimeSignature = i;
                    break;
                }
                else if(sTimeSignature[i].Measure > sTimePosition->Measure)
                    break;
            }
        }
    }

    sTimePosition->Tick = iMIDITime - iCurrentClock;
    sTimePosition->Tick /= iTicksRatio;

    if(sTimePosition->Tick == M_TICKS_PER_BEAT)
    {
        sTimePosition->Tick = 0;
        sTimePosition->Beat++;

        if(sTimePosition->Beat > sTimeSignature[iCurrentTimeSignature].BeatsPerMeasure)
        {
            sTimePosition->Beat = 1;
            sTimePosition->Measure++;
        }
    }
}

unsigned int GetInTicks(const SMIDITimePosition& sTimePosition, unsigned int iBeatsPerMeasure)
{
    return ((sTimePosition.Measure - 1) * iBeatsPerMeasure * M_TICKS_PER_BEAT) + 
           ((sTimePosition.Beat - 1) * M_TICKS_PER_BEAT) + 
           sTimePosition.Tick;
}
