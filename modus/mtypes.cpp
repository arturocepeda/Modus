
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

#include "mtypes.h"
#include <ctype.h>
#include <algorithm>


//
//  MSTimePosition
//
MSTimePosition::MSTimePosition()
    : Measure(1)
    , Beat(1)
    , Tick(0)
{
}

MSTimePosition::MSTimePosition(int Measure)
    : Measure(Measure)
    , Beat(1)
    , Tick(0)
{
}

MSTimePosition::MSTimePosition(int Measure, unsigned int Beat)
    : Measure(Measure)
    , Beat(Beat)
    , Tick(0)
{
}

MSTimePosition::MSTimePosition(int Measure, unsigned int Beat, unsigned int Tick)
    : Measure(Measure)
    , Beat(Beat)
    , Tick(Tick)
{
}

bool MSTimePosition::operator == (const MSTimePosition& TimePosition) const
{
    return (Measure == TimePosition.Measure && Beat == TimePosition.Beat && Tick == TimePosition.Tick);
}

bool MSTimePosition::operator < (const MSTimePosition& TimePosition) const
{
    if(Measure < TimePosition.Measure)
    {
        return true;
    }
    else if(Measure == TimePosition.Measure)
    {
        if(Beat < TimePosition.Beat)
        {
            return true;
        }
        else if(Beat == TimePosition.Beat)
        {
            if(Tick < TimePosition.Tick)
                return true;
        }
    }

    return false;
}

bool MSTimePosition::operator > (const MSTimePosition& TimePosition) const
{
    if(Measure > TimePosition.Measure)
    {
        return true;
    }
    else if(Measure == TimePosition.Measure)
    {
        if(Beat > TimePosition.Beat)
        {
            return true;
        }
        else if(Beat == TimePosition.Beat)
        {
            if(Tick > TimePosition.Tick)
                return true;
        }
    }

    return false;
}

bool MSTimePosition::operator <= (const MSTimePosition& TimePosition) const
{
    if(Measure > TimePosition.Measure)
    {
        return false;
    }
    else if(Measure == TimePosition.Measure)
    {
        if(Beat > TimePosition.Beat)
        {
            return false;
        }
        else if(Beat == TimePosition.Beat)
        {
            if(Tick > TimePosition.Tick)
                return false;
        }
    }

    return true;
}

bool MSTimePosition::operator >= (const MSTimePosition& TimePosition) const
{
    if(Measure < TimePosition.Measure)
    {
        return false;
    }
    else if(Measure == TimePosition.Measure)
    {
        if(Beat < TimePosition.Beat)
        {
            return false;
        }
        else if(Beat == TimePosition.Beat)
        {
            if(Tick < TimePosition.Tick)
                return false;
        }
    }

    return true;
}

void MSTimePosition::add(int Ticks, unsigned int BeatsPerMeasure)
{
    if(Ticks >= 0)
    {
        Tick += Ticks;

        while(Tick >= M_TICKS_PER_BEAT)
        {
            Tick -= M_TICKS_PER_BEAT;
            Beat++;

            if(Beat > BeatsPerMeasure)
            {
                Beat = 1;
                Measure++;
            }
        }
    }
    else
    {
        unsigned int iTicks = abs(Ticks);

        while(iTicks >= M_TICKS_PER_BEAT)
        {
            iTicks -= M_TICKS_PER_BEAT;
            Beat--;

            if(Beat == 0)
            {
                Beat = BeatsPerMeasure;
                Measure--;
            }
        }

        if(Tick >= iTicks)
        {
            Tick -= iTicks;
        }
        else
        {
            Tick += M_TICKS_PER_BEAT;
            Tick -= iTicks;
            Beat--;

            if(Beat == 0)
            {
                Beat = BeatsPerMeasure;
                Measure--;
            }
        }
    }
}

int MSTimePosition::getInTicks(unsigned int BeatsPerMeasure) const
{
    return ((Measure - 1) * BeatsPerMeasure * M_TICKS_PER_BEAT) + ((Beat - 1) * M_TICKS_PER_BEAT) + Tick;
}

MSTimePosition MSTimePosition::getWithIncrement(int Ticks, unsigned int BeatsPerMeasure) const
{
    MSTimePosition mIncTimePosition;

    mIncTimePosition.Measure = Measure;
    mIncTimePosition.Beat = Beat;
    mIncTimePosition.Tick = Tick;
    mIncTimePosition.add(Ticks, BeatsPerMeasure);

    return mIncTimePosition;
}

unsigned int MSTimePosition::getDifferenceWith(const MSTimePosition& TimePosition, unsigned int BeatsPerMeasure) const
{
    unsigned int iTicks1 = getInTicks(BeatsPerMeasure);
    unsigned int iTicks2 = TimePosition.getInTicks(BeatsPerMeasure);

    if(iTicks2 > iTicks1)
        return (iTicks2 - iTicks1);
    else
        return (iTicks1 - iTicks2);
}


//
//  MSNote
//
MSNote::MSNote()
{
    memset(this, 0, sizeof(MSNote));
}

MSNote::MSNote(MTNote Pitch, unsigned char Intensity, unsigned char Mode, unsigned char Channel, unsigned char Duration)
    : Pitch(Pitch)
    , Intensity(Intensity)
    , Mode(Mode)
    , Channel(Channel)
    , Duration(Duration)
    , MDA(0)
    , MDB(0)
    , MDC(0)
{
}

MSNote::MSNote(MTNote Pitch, unsigned char Intensity, unsigned char Mode, unsigned char Channel, unsigned char Duration,
               unsigned char MDA, unsigned char MDB, unsigned char MDC)
    : Pitch(Pitch)
    , Intensity(Intensity)
    , Mode(Mode)
    , Channel(Channel)
    , Duration(Duration)
    , MDA(MDA)
    , MDB(MDB)
    , MDC(MDC)
{
}


//
// MSRange
//
MSRange::MSRange()
    : LowestNote(0)
    , HighestNote(127)
{
}

MSRange::MSRange(MTNote LowestNote, MTNote HighestNote)
    : LowestNote(LowestNote)
    , HighestNote(HighestNote)
{
}

unsigned char MSRange::getSize() const
{
    return (HighestNote - LowestNote + 1);
}


//
// MSSampleSet
//
MSSampleSet::MSSampleSet()
{
    Path[0] = '\0';
    Format[0] = '\0';
    ID = 0;
    Mode = 0;
    MinimumIntensity = 0;
    MaximumIntensity = 127;
}


//
//  MCNotes
//
bool MCNotes::isNatural(MTNote Note)
{
    switch(whichNote(Note))
    {
    case C:
    case D:
    case E:
    case F:
    case G:
    case A:
    case B:
        return true;

    default:
        return false;
    }
}

MTNote MCNotes::whichNote(MTNote Note)
{
    return (Note % 12);
}

void MCNotes::whichNote(MTNote Note, char* String)
{
    switch(Note % 12)
    {
    case 0:
        strcpy(String, "C");
        break;
    case 1:
        strcpy(String, "C#/Db");
        break;
    case 2:
        strcpy(String, "D");
        break;
    case 3:
        strcpy(String, "D#/Eb");
        break;
    case 4:
        strcpy(String, "E");
        break;
    case 5:
        strcpy(String, "F");
        break;
    case 6:
        strcpy(String, "F#/Gb");
        break;
    case 7:
        strcpy(String, "G");
        break;
    case 8:
        strcpy(String, "G#/Ab");
        break;
    case 9:
        strcpy(String, "A");
        break;
    case 10:
        strcpy(String, "A#/Bb");
        break;
    case 11:
        strcpy(String, "B");
        break;
    }
}

void MCNotes::whichNoteFlat(MTNote Note, char* String)
{
    switch(Note % 12)
    {
    case 0:
        strcpy(String, "C");
        break;
    case 1:
        strcpy(String, "Db");
        break;
    case 2:
        strcpy(String, "D");
        break;
    case 3:
        strcpy(String, "Eb");
        break;
    case 4:
        strcpy(String, "E");
        break;
    case 5:
        strcpy(String, "F");
        break;
    case 6:
        strcpy(String, "Gb");
        break;
    case 7:
        strcpy(String, "G");
        break;
    case 8:
        strcpy(String, "Ab");
        break;
    case 9:
        strcpy(String, "A");
        break;
    case 10:
        strcpy(String, "Bb");
        break;
    case 11:
        strcpy(String, "B");
        break;
    }
}

void MCNotes::whichNoteSharp(MTNote Note, char* String)
{
    switch(Note % 12)
    {
    case 0:
        strcpy(String, "C");
        break;
    case 1:
        strcpy(String, "C#");
        break;
    case 2:
        strcpy(String, "D");
        break;
    case 3:
        strcpy(String, "D#");
        break;
    case 4:
        strcpy(String, "E");
        break;
    case 5:
        strcpy(String, "F");
        break;
    case 6:
        strcpy(String, "F#");
        break;
    case 7:
        strcpy(String, "G");
        break;
    case 8:
        strcpy(String, "G#");
        break;
    case 9:
        strcpy(String, "A");
        break;
    case 10:
        strcpy(String, "A#");
        break;
    case 11:
        strcpy(String, "B");
        break;
    }
}

MTNote MCNotes::fromString(const char* String)
{
    if(strlen(String) < 1)
        return 0;

    MTNote mNote;

    switch(toupper(String[0]))
    {
    case 'C':
        mNote = C;
        break;

    case 'D':
        mNote = D;
        break;

    case 'E':
        mNote = E;
        break;

    case 'F':
        mNote = F;
        break;

    case 'G':
        mNote = G;
        break;

    case 'A':
        mNote = A;
        break;

    case 'B':
        mNote = B;
        break;
    }

    // check accidental
    switch(toupper(String[1]))
    {
    // half step down
    case 'B':
    case 'F':

        if(mNote == C)
            mNote = B;
        else
            mNote--;

        break;   

    // half step up
    case '#':
    case 'S':

        if(mNote == B)
            mNote = C;
        else
            mNote++;

        break;
    }

    return mNote;
}

inline void MCNotes::octaveUp(MTNote* Note)
{
    if(*Note <= 115)
        *Note += 12;
}

inline void MCNotes::octaveDown(MTNote* Note)
{
    if(*Note >= 12)
        *Note -= 12;
}


//
//  MCNoteMaps
//
MTNoteMap MCNoteMaps::createNoteMap(MTNote RootNote, const MTScale& Scale, const MSRange& Range)
{
    MTNoteMap mNoteMap;
    MTNote mNote = RootNote % 12;

    unsigned int iCurrentOctave = Range.LowestNote / 12;
    unsigned int iOctaveIncrement = 1 + (Scale[Scale.size() - 1] / 12);

    unsigned int iCurrentScalePosition;
    unsigned int iCurrentNote;

    do
    {
        for(iCurrentScalePosition = 0; iCurrentScalePosition < Scale.size(); iCurrentScalePosition++)
        {
            iCurrentNote = mNote + Scale[iCurrentScalePosition] + (iCurrentOctave * 12);

            if(iCurrentNote > Range.HighestNote)
                break;

            if(iCurrentNote >= Range.LowestNote)
                mNoteMap.push_back(iCurrentNote);
        }

        iCurrentOctave += iOctaveIncrement;

    } while(iCurrentNote <= Range.HighestNote);

    return mNoteMap;
}

MTNoteMap MCNoteMaps::extractNoteEntries(MTNote Note, const MTNoteMap& Notemap)
{
    MTNoteMap mNotemap;

    for(unsigned int i = 0; i < Notemap.size(); i++)
    {
        if(Notemap[i] % 12 == Note)
            mNotemap.push_back(Notemap[i]);
    }

    return mNotemap;
}

MTNoteMap MCNoteMaps::combineNoteMaps(const MTNoteMap& Notemap1, const MTNoteMap& Notemap2)
{
    MTNoteMap mNewNotemap;
    unsigned int iPosition1 = 0;
    unsigned int iPosition2 = 0;

    while(iPosition1 < Notemap1.size() || iPosition2 < Notemap2.size())
    {
        if(iPosition1 == Notemap1.size())
        {
            mNewNotemap.push_back(Notemap2[iPosition2]);
            iPosition2++;
        }
        else if(iPosition2 == Notemap2.size())
        {
            mNewNotemap.push_back(Notemap1[iPosition1]);
            iPosition1++;
        }
        else
        {
            if(Notemap1[iPosition1] < Notemap2[iPosition2])
            {
                mNewNotemap.push_back(Notemap1[iPosition1]);
                iPosition1++;
            }
            else if(Notemap1[iPosition1] > Notemap2[iPosition2])
            {
                mNewNotemap.push_back(Notemap2[iPosition2]);
                iPosition2++;
            }
            else
            {
                mNewNotemap.push_back(Notemap1[iPosition1]);
                iPosition1++;
                iPosition2++;
            }
        }
    }

    return mNewNotemap;
}

void MCNoteMaps::sortNoteMap(MTNoteMap* Notemap)
{
    std::sort(Notemap->begin(), Notemap->end());
}

void MCNoteMaps::transposeNoteMap(MTNoteMap* Notemap, char Semitones)
{
    for(unsigned int i = 0; i < Notemap->size(); i++)
        (*Notemap)[i] += Semitones;
}

bool MCNoteMaps::contains(const MTNoteMap& Notemap, MTNote Note)
{
    for(unsigned int i = 0; i < Notemap.size(); i++)
    {
        if(Notemap[i] == Note)
            return true;
    }

    return false;
}

bool MCNoteMaps::contains(const MTNoteMap& Notemap, const MTNoteMap& Notes)
{
    for(unsigned int i = 0; i < Notes.size(); i++)
    {
        if(!contains(Notemap, Notes[i]))
            return false;
    }

    return true;
}

int MCNoteMaps::getPositionNearestNote(MTNote Note, const MTNoteMap& NoteMap)
{
    unsigned int iNoteMapSize = NoteMap.size();

    // lower than the first note
    if(iNoteMapSize == 0 || Note < NoteMap[0])
        return 0;

    // higher than the last note
    if(Note > NoteMap[iNoteMapSize - 1])
        return (iNoteMapSize - 1);

    // in the middle
    for(unsigned int i = 0; i < iNoteMapSize; i++)
    {
        if(Note == NoteMap[i])
            return i;

        if(i > 0 && Note > NoteMap[i - 1] && Note < NoteMap[i])
        {
            if((Note - NoteMap[i - 1]) > (NoteMap[i] - Note))
                return i;
            else
                return (i - 1);
        }
    }

    return -1;
}
