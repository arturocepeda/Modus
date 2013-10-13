
////////////////////////////////////////////////////////////////////////
//
//  Modus v0.53
//  C++ Music Library
//
//  Copyright (c) 2012-2013 Arturo Cepeda Pérez
//
//  --------------------------------------------------------------------
//
//  This file is part of Modus. Permission is hereby granted, free 
//  of charge, to any person obtaining a copy of this software and 
//  associated documentation files (the "Software"), to deal in the
//  Software without restriction, including without limitation the 
//  rights to use, copy, modify, merge, publish, distribute, 
//  sublicense, and/or sell copies of the Software, and to permit 
//  persons to whom the Software is furnished to do so, subject to 
//  the following conditions:
//
//  The above copyright notice and this permission notice shall be 
//  included in all copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY 
//  KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE 
//  WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR 
//  PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS 
//  OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR 
//  OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR 
//  OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
//  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
////////////////////////////////////////////////////////////////////////

#include "mscript.h"

void MCScript::writeBMP(char* String, unsigned int BeatsPerMeasure)
{
    sprintf(String, "bpm:%d\n", BeatsPerMeasure);
}

void MCScript::writeTimePosition(char* String, MSTimePosition& TimePosition)
{
    sprintf(String, "[m:%d b:%d t:%d]\n", TimePosition.Measure, TimePosition.Beat, TimePosition.Tick);
}

void MCScript::writeTimePosition(char* String, unsigned int Measure, unsigned int Beat, unsigned int Tick)
{
    sprintf(String, "[m:%d b:%d t:%d]\n", Measure, Beat, Tick);
}

void MCScript::writeCurrentBeat(char* String, unsigned int Beat)
{
    sprintf(String, "[b:%d]\n", Beat);
}

void MCScript::writeCurrentTick(char* String, unsigned int Tick)
{
    sprintf(String, "[t:%d]\n", Tick);
}

void MCScript::writeCurrentBeatAndTick(char* String, unsigned int Beat, unsigned int Tick)
{
    sprintf(String, "[b:%d t:%d]\n", Beat, Tick);
}

void MCScript::writeNote(char* String, MSNote& Note, bool AddDuration)
{
    sprintf(String, "p:%d i:%d mode:%d ch:%d d:%d mda:%d mdb:%d mdc:%d > %c\n", Note.Pitch, Note.Intensity, Note.Mode,
                                                                                Note.Channel, Note.Duration, 
                                                                                Note.MDA, Note.MDB, Note.MDC,
                                                                                (AddDuration)? '+': ' ');
}

void MCScript::writeNote(char* String, MTNote Pitch, unsigned char Intensity, unsigned char Mode,
                         unsigned char Channel, unsigned char Duration, bool AddDuration)
{
    sprintf(String, "p:%d i:%d mode:%d ch:%d d:%d > %c\n", Pitch, Intensity, Mode, Channel, Duration, (AddDuration)? '+': ' ');
}

void MCScript::writeRest(char* String, unsigned char Channel, unsigned char Duration, bool AddDuration)
{
    sprintf(String, "p:0 ch:%d d:%d > %c\n", Channel, Duration, (AddDuration)? '+': ' ');
}

void MCScript::writeChord(char* String, MTNoteMap& PitchList, unsigned char Intensity, unsigned char Mode,
                          unsigned char FirstChannel, unsigned char Duration, unsigned char TicksBetweenNotes,
                          MSTimePosition& TimePosition, unsigned char BeatsPerMeasure)
{
    MSTimePosition mTimePos = TimePosition;
    unsigned char iChannel = FirstChannel;

    sprintf(String, "b:%d\ni:%d mode:%d\n", BeatsPerMeasure, Intensity, Mode);

    for(unsigned int i = 0; i < PitchList.size(); i++)
    {
        // time position
        writeTimePosition(String + strlen(String), mTimePos);

        // note to play
        sprintf(String + strlen(String), "p:%d ch:%d d:%d >\n", PitchList[i], iChannel, Duration);

        // update values
        mTimePos.add(TicksBetweenNotes, BeatsPerMeasure);
        iChannel++;

        if(Duration > 0)
            Duration -= TicksBetweenNotes;
    }
}

void MCScript::writeScale(char* String, MTNoteMap& PitchList, unsigned char Intensity, unsigned char Mode,
                          unsigned char Channel, unsigned char DurationOfEachNote, 
                          MSTimePosition& TimePosition, unsigned char BeatsPerMeasure)
{
    MSTimePosition mTimePos = TimePosition;

    sprintf(String, "b:%d\ni:%d mode:%d\n", BeatsPerMeasure, Intensity, Mode);

    // time position
    writeTimePosition(String + strlen(String), mTimePos);
    
    for(unsigned int i = 0; i < PitchList.size(); i++)
    {
        // note to play
        sprintf(String + strlen(String), "p:%d ch:%d d:%d > +\n", PitchList[i], Channel, DurationOfEachNote);

        // update values
        mTimePos.add(DurationOfEachNote, BeatsPerMeasure);
    }
}
