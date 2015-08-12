
////////////////////////////////////////////////////////////////////////
//
//  Modus v0.60
//  C++ Music Library
//
//  Copyright (c) 2012-2015 Arturo Cepeda Pérez
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

#include "mchords.h"

using namespace Modus;

const char* MCChords::ChordsDefinition = "0: 0,4,7 \n"          // M
                                         "1: 0,4,7,9 \n"        // M6
                                         "2: 0,4,7,11 \n"       // M7
                                         "3: 0,4,8 \n"          // Maug5
                                         "4: 0,3,7 \n"          // m
                                         "5: 0,3,7,10 \n"       // m7
                                         "6: 0,3,6,10 \n"       // m7b5
                                         "7: 0,3,7,9 \n"        // m6
                                         "8: 0,3,7,11 \n"       // mM7
                                         "9: 0,4,7,10 \n"       // 7
                                         "10: 0,4,7,10,14 \n"   // 9
                                         "11: 0,4,7,10,13 \n"   // 7m9
                                         "12: 0,3,6 \n"         // dis
                                         "13: 0,3,6,9 \n"       // dis7
                                         "14: 0,5,7 \n"         // sus4
                                         "15: 0,5,7,10 \n"      // 7sus4
                                         "16: 0,4,6,10 \n"      // 7b5
                                         "17: 0,4,8,10 \n";     // 7aug5


bool MCChords::equal(const MTChord& Chord1, const MTChord& Chord2)
{
    return std::equal(Chord1.begin(), Chord1.end(), Chord2.begin());
}

MTChord MCChords::cM()
{
    MTChord mChord;
    MTNote mNoteSet[] = {0, 4, 7};
    mChord.assign(mNoteSet, mNoteSet + 3);
    return mChord;
}

MTChord MCChords::cM6()
{
    MTChord mChord;
    MTNote mNoteSet[] = {0, 4, 7, 9};
    mChord.assign(mNoteSet, mNoteSet + 4);
    return mChord;
}

MTChord MCChords::cM7()
{
    MTChord mChord;
    MTNote mNoteSet[] = {0, 4, 7, 11};
    mChord.assign(mNoteSet, mNoteSet + 4);
    return mChord;
}

MTChord MCChords::cMaug5()
{
    MTChord mChord;
    MTNote mNoteSet[] = {0, 4, 8};
    mChord.assign(mNoteSet, mNoteSet + 3);
    return mChord;
}

MTChord MCChords::cm()
{
    MTChord mChord;
    MTNote mNoteSet[] = {0, 3, 7};
    mChord.assign(mNoteSet, mNoteSet + 3);
    return mChord;
}

MTChord MCChords::cm7()
{
    MTChord mChord;
    MTNote mNoteSet[] = {0, 3, 7, 10};
    mChord.assign(mNoteSet, mNoteSet + 4);
    return mChord;
}

MTChord MCChords::cm7b5()
{
    MTChord mChord;
    MTNote mNoteSet[] = {0, 3, 6, 10};
    mChord.assign(mNoteSet, mNoteSet + 4);
    return mChord;
}

MTChord MCChords::cm6()
{
    MTChord mChord;
    MTNote mNoteSet[] = {0, 3, 7, 9};
    mChord.assign(mNoteSet, mNoteSet + 4);
    return mChord;
}

MTChord MCChords::cmM7()
{
    MTChord mChord;
    MTNote mNoteSet[] = {0, 3, 7, 11};
    mChord.assign(mNoteSet, mNoteSet + 4);
    return mChord;
}

MTChord MCChords::c7()
{
    MTChord mChord;
    MTNote mNoteSet[] = {0, 4, 7, 10};
    mChord.assign(mNoteSet, mNoteSet + 4);
    return mChord;
}

MTChord MCChords::c9()
{
    MTChord mChord;
    MTNote mNoteSet[] = {0, 4, 7, 10, 14};
    mChord.assign(mNoteSet, mNoteSet + 5);
    return mChord;
}

MTChord MCChords::c7m9()
{
    MTChord mChord;
    MTNote mNoteSet[] = {0, 4, 7, 10, 13};
    mChord.assign(mNoteSet, mNoteSet + 5);
    return mChord;
}

MTChord MCChords::cdis()
{
    MTChord mChord;
    MTNote mNoteSet[] = {0, 3, 6};
    mChord.assign(mNoteSet, mNoteSet + 3);
    return mChord;
}

MTChord MCChords::cdis7()
{
    MTChord mChord;
    MTNote mNoteSet[] = {0, 3, 6, 9};
    mChord.assign(mNoteSet, mNoteSet + 4);
    return mChord;
}

MTChord MCChords::csus4()
{
    MTChord mChord;
    MTNote mNoteSet[] = {0, 5, 7};
    mChord.assign(mNoteSet, mNoteSet + 3);
    return mChord;
}

MTChord MCChords::c7sus4()
{
    MTChord mChord;
    MTNote mNoteSet[] = {0, 5, 7, 10};
    mChord.assign(mNoteSet, mNoteSet + 4);
    return mChord;
}

MTChord MCChords::c7b5()
{
    MTChord mChord;
    MTNote mNoteSet[] = {0, 4, 6, 10};
    mChord.assign(mNoteSet, mNoteSet + 4);
    return mChord;
}

MTNoteMap MCChords::createClosedChord(MTNote RootNote, MTChord& Chord, MTNote LowestNote, 
                                      unsigned char NumberOfNotes)
{
    MSRange mRange(LowestNote, 127);
    MTNoteMap mNoteMap = MCNoteMaps::createNoteMap(RootNote, Chord, mRange);
    MTNoteMap mClosedChord;

    unsigned int i;
    int iPosition = 0;

    // find the first note
    for(i = 0; i < mNoteMap.size(); i++)
    {
        if(mNoteMap[i] >= LowestNote)
        {
            iPosition = i;
            break;
        }
    }

    // fill the rest of the notes
    for(i = 0; (iPosition + i) < mNoteMap.size() && i < NumberOfNotes; i++)
        mClosedChord.push_back(mNoteMap[iPosition + i]);

    return mClosedChord;
}

MTNoteMap MCChords::createOpenChord(MTNote RootNote, MTChord& Chord, MTNote LowestNote, 
                                    unsigned char NumberOfNotes)
{
    MSRange mRange(LowestNote, 127);
    MTNoteMap mNoteMap = MCNoteMaps::createNoteMap(RootNote, Chord, mRange);
    MTNoteMap mOpenChord;

    unsigned int i;
    unsigned int iPosition = 0;

    // find the first note
    for(i = 0; i < mNoteMap.size(); i++)
    {
        if(mNoteMap[i] >= LowestNote)
        {
            iPosition = i;
            break;
        }
    }

    NumberOfNotes *= 2;

    // fill the rest of the notes
    for(i = 0; (iPosition + i) < mNoteMap.size() && i < NumberOfNotes; i += 2)
        mOpenChord.push_back(mNoteMap[iPosition + i]);

    return mOpenChord;
}

MTNoteMap MCChords::createClosedVoicing(MTNote RootNote, MTChord& Chord, MTNote BassNote, MTChord& Tensions, 
                                        MTNote LowestNote, bool RemoveBassNote, bool RemovePerfectFifth)
{
    MTNoteMap mCompleteChord = MCNoteMaps::combineNoteMaps(Chord, Tensions);

    unsigned int i;
    unsigned int iOctave = (LowestNote / 12) - 1;

    // remove bass note from the pattern of the complete chord
    if(RemoveBassNote)
    {
        for(i = 0; i < mCompleteChord.size(); i++)
        {
            if(mCompleteChord[i] == (BassNote - RootNote))
            {
                mCompleteChord.erase(mCompleteChord.begin() + i);
                break;
            }
        }
    }

    // remove perfect fifth from the pattern of the complete chord
    if(RemovePerfectFifth)
    {
        for(i = 0; i < mCompleteChord.size(); i++)
        {
            if(mCompleteChord[i] == 7)
            {
                mCompleteChord.erase(mCompleteChord.begin() + i);
                break;
            }
        }
    }

    // move the notes in the pattern with the complete chord to the real pitches
    for(i = 0; i < mCompleteChord.size(); i++)
    {
        mCompleteChord[i] += RootNote + (iOctave * 12);

        while(mCompleteChord[i] < LowestNote)
            mCompleteChord[i] += 12;
    }

    // sort the notes from low to high
    MCNoteMaps::sortNoteMap(&mCompleteChord);

    return mCompleteChord;
}

MTNoteMap MCChords::createOpenVoicing(MTNote RootNote, MTChord& Chord, MTNote BassNote, MTChord& Tensions, 
                                      MTNote LowestNote, bool RemoveBassNote, bool RemovePerfectFifth)
{
    MTNoteMap mOpenVoicing = createClosedVoicing(RootNote, Chord, BassNote, Tensions, LowestNote, 
                                                 RemoveBassNote, RemovePerfectFifth);
    unsigned int i;

    for(i = 1; i < mOpenVoicing.size(); i++)
    {
        if((i % 2) == 1)
            mOpenVoicing[i] += 12;
    }

    // sort the notes from low to high
    MCNoteMaps::sortNoteMap(&mOpenVoicing);

    return mOpenVoicing;
}
