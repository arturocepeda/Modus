
////////////////////////////////////////////////////////////////////////
//
//  Modus v0.50
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

#include <vector>

#define M_TICKS_PER_BEAT 24

struct SMIDIConfig
{
    char* MIDIFile;
    char* ScriptFile;

    std::vector<unsigned char> MIDIChannels;
    unsigned char ScriptChannels;

    unsigned int FirstMeasure;
    unsigned int LastMeasure;
};

struct SMIDITimeSignature
{
    unsigned int Measure;
    unsigned int BeatsPerMeasure;
};

struct SMIDITimePosition
{
    unsigned int Measure;
    unsigned int Beat;
    unsigned int Tick;
};

bool ChannelIncluded(unsigned char iChannel, const std::vector<unsigned char>& iListOfChannels);
void GetTimePosition(unsigned int iMIDITime, unsigned int iMIDIClocksPerBeat, 
                     const std::vector<SMIDITimeSignature>& sTimeSignature, SMIDITimePosition* sTimePosition);
unsigned int GetInTicks(const SMIDITimePosition& sTimePosition, unsigned int iTicksPerBeat);
