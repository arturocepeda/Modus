
////////////////////////////////////////////////////////////////////////
//
//  Modus v0.54
//  C++ Music Library
//
//  Copyright (c) 2012-2014 Arturo Cepeda Pérez
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

#include "mharmonypattern.h"
#include "mchords.h"
#include "mutils.h"
#include <iostream>
#include <fstream>
#include <sstream>

//
//  MCHarmonyPattern
//
MCHarmonyPattern::MCHarmonyPattern()
{
}

MCHarmonyPattern::~MCHarmonyPattern()
{
}

void MCHarmonyPattern::addEntry(const MSHarmonyPatternEntry& HarmonyPatternEntry)
{
    MSHarmonyPatternEntry mNewHarmonyPatternEntry = HarmonyPatternEntry;

    while(mNewHarmonyPatternEntry.RootNote >= 12)
        mNewHarmonyPatternEntry.RootNote -= 12;

    while(mNewHarmonyPatternEntry.BassNote >= 12)
        mNewHarmonyPatternEntry.BassNote -= 12;

    if(sEntry.size() == 0)
    {
        sEntry.push_back(mNewHarmonyPatternEntry);
        return;    
    }

    MSTimePosition mNewEntryTimePosition;
    MSTimePosition mFirstEntryTimePosition;
    MSTimePosition mLastEntryTimePosition;

    mNewEntryTimePosition = mNewHarmonyPatternEntry.TimePosition;
    mFirstEntryTimePosition = sEntry[0].TimePosition;
    mLastEntryTimePosition = sEntry[sEntry.size() - 1].TimePosition;

    unsigned int iNewEntryPosition = mNewEntryTimePosition.getInTicks(iBeatsPerMeasure);
    unsigned int iFirstEntryPosition = mFirstEntryTimePosition.getInTicks(iBeatsPerMeasure);
    unsigned int iLastEntryPosition = mLastEntryTimePosition.getInTicks(iBeatsPerMeasure);
    unsigned int iCurrentEntryPosition;

    // first element
    if(iNewEntryPosition < iFirstEntryPosition)
    {
        sEntry.insert(sEntry.begin(), mNewHarmonyPatternEntry);
    }

    // last element
    else if(iNewEntryPosition >= iLastEntryPosition)
    {
        sEntry.push_back(mNewHarmonyPatternEntry);
    }
    
    // another position in the middle
    else
    {
        for(unsigned int i = 1; i < sEntry.size(); i++)
        {
            iCurrentEntryPosition = sEntry[i].TimePosition.getInTicks(iBeatsPerMeasure);

            if(iNewEntryPosition < iCurrentEntryPosition)
            {
                sEntry.insert(sEntry.begin() + i, mNewHarmonyPatternEntry);
                break;
            }
        }
    }
}

void MCHarmonyPattern::transpose(char NumberOfSemitones)
{
    char iNewPitch;

    for(unsigned int i = 0; i < sEntry.size(); i++)
    {
        iNewPitch = sEntry[i].RootNote + NumberOfSemitones;

        if(iNewPitch < 0)
            iNewPitch += 12;
        else if(iNewPitch >= 12)
            iNewPitch -= 12;

        sEntry[i].RootNote = iNewPitch;

        iNewPitch = sEntry[i].BassNote + NumberOfSemitones;

        if(iNewPitch < 0)
            iNewPitch += 12;
        else if(iNewPitch >= 12)
            iNewPitch -= 12;

        sEntry[i].BassNote = iNewPitch;
    }
}

void MCHarmonyPattern::loadScript(std::istream* sChordsDefinition, std::istream* sScript)
{
    // load the scales list
    char sLine[LINE_BUFFER];
    char* sLinePointer;
    int iValue;

    MTChord sAux;
    MTChordList sChords;

    while(sChordsDefinition->good())
    {
        // read a new line
        sChordsDefinition->getline(sLine, sizeof(sLine), '\n');

        // remove the comments
        if(strchr(sLine, ';') != NULL)
            strchr(sLine, ';')[0] = '\0';

        // no information: empty line
        if(sLine[0] == '\0')
            continue;

        // scale definition found
        if(strchr(sLine, ':') != NULL)
            sLinePointer = strchr(sLine, ':') + 1;
        else
            continue;

        if(sLinePointer[0] == '\0')
            continue;

        // here we have a list of notes separated by ','
        sAux = readScriptNoteList(sLinePointer);

        // add the scale to the list
        if(sAux.size() > 0)
            sChords.push_back(sAux);
    }
    
    if(sChords.size() == 0)
        return;

    // clear all the previous scale entries
    clear();

    MSHarmonyPatternEntry hpe;
    hpe.TimePosition.Measure = 1;
    hpe.TimePosition.Beat = 1;
    hpe.TimePosition.Tick = 0;
    hpe.Chord = sChords[0];

    // load the scale entries
    while(sScript->good())
    {
        // read a new line
        sScript->getline(sLine, sizeof(sLine), '\n');
        
        // remove the comments
        if(strchr(sLine, ';') != NULL)
            strchr(sLine, ';')[0] = '\0';

        // no information: empty line
        if(sLine[0] == '\0')
            continue;

        // lower case
        strtolower(sLine);

        // time position
        if(strchr(sLine, '[') != NULL)
        {
            iValue = readScriptValue(sLine, "m");
            if(iValue > -1)
            {
                hpe.TimePosition.Measure = iValue;
                hpe.TimePosition.Beat = 1;
            }

            iValue = readScriptValue(sLine, "b");
            if(iValue > -1)
                hpe.TimePosition.Beat = iValue;

            continue;
        }

        // by default there are no tensions
        hpe.Tensions.clear();

        // B: beats per measure
        iValue = readScriptValue(sLine, "bpm:");
        if(iValue > 0)
            iBeatsPerMeasure = iValue;

        // root: root note
        iValue = readScriptValue(sLine, "root:");
        if(iValue > -1)
        {
            hpe.RootNote = iValue;
            hpe.BassNote = iValue;  // by default the same note
        }

        // chord: chord number
        iValue = readScriptValue(sLine, "chord:");
        if(iValue > -1 && iValue < (int)sChords.size())
            hpe.Chord = sChords[iValue];

        // bass: bass note
        iValue = readScriptValue(sLine, "bass:");
        if(iValue > -1)
            hpe.BassNote = iValue;

        // tensions: list of tensions
        if(strstr(sLine, "tensions:") != NULL)
            hpe.Tensions = readScriptNoteList(strstr(sLine, "tensions:"));

        // add entry!
        if(strchr(sLine, '>') != NULL)
        {
            addEntry(hpe);

            if(strchr(sLine, '+') != NULL)
            {
                hpe.TimePosition.Beat++;

                if(hpe.TimePosition.Beat > iBeatsPerMeasure)
                {
                    hpe.TimePosition.Measure++;
                    hpe.TimePosition.Beat = 1;
                }
            }
        }
    }
}

bool MCHarmonyPattern::loadScriptFromFile(const char* ChordsDefinitionFilename, const char* ScriptFilename)
{
    // open the files
    std::ifstream fChordsFile(ChordsDefinitionFilename);

    if(!fChordsFile.is_open())
        return false;

    std::ifstream fScriptFile(ScriptFilename);

    if(!fScriptFile.is_open())
    {
        fChordsFile.close();
        return false;
    }

    loadScript(&fChordsFile, &fScriptFile);

    // close the files
    fChordsFile.close();
    fScriptFile.close();

    return true;
}

bool MCHarmonyPattern::loadScriptFromFile(const char* ScriptFilename)
{
    std::string sChordsDefinitionString(MCChords::ChordsDefinition);
    std::istringstream sChordsDefinition(sChordsDefinitionString);

    // open the file
    std::ifstream fScriptFile(ScriptFilename);

    if(!fScriptFile.is_open())
        return false;

    loadScript(&sChordsDefinition, &fScriptFile);

    // close the file
    fScriptFile.close();

    return true;
}

void MCHarmonyPattern::loadScriptFromString(const char* ChordsDefinition, const char* Script)
{
    std::string sChordsDefinitionString(ChordsDefinition);
    std::string sScriptString(Script);

    std::istringstream sChordsDefinition(sChordsDefinitionString);
    std::istringstream sScript(sScriptString);

    loadScript(&sChordsDefinition, &sScript);
}

void MCHarmonyPattern::loadScriptFromString(const char* Script)
{
    loadScriptFromString(MCChords::ChordsDefinition, Script);
}

bool MCHarmonyPattern::loadScriptFromBinaryFile(const char* Filename)
{
    std::ifstream fFile(Filename, std::ifstream::binary);

    if(!fFile.is_open())
        return false;

    loadScriptFromBinaryData(fFile);
    fFile.close();

    return true;
}

void MCHarmonyPattern::loadScriptFromBinaryData(std::istream& Stream)
{
    // clear all the previous score entries
    clear();

    // header
    char sBuffer[16];
    Stream.read(sBuffer, 16);

    // number of entries
    unsigned int iNumberOfEntries;
    Stream.read((char*)&iNumberOfEntries, sizeof(unsigned int));

    // entries
    MSHarmonyPatternEntry hpe;
    char iNumberOfNotes;
    MTNote mNote;

    for(unsigned int i = 0; Stream.good() && i < iNumberOfEntries; i++)
    {
        hpe.Chord.clear();
        hpe.Tensions.clear();

        Stream.read((char*)&hpe.TimePosition.Measure, sizeof(short));
        Stream.read((char*)&hpe.TimePosition.Beat, 1);
        Stream.read((char*)&hpe.TimePosition.Tick, 1);
        Stream.read((char*)&hpe.RootNote, 1);
        Stream.read(&iNumberOfNotes, 1);

        for(char j = 0; j < iNumberOfNotes; j++)
        {
            Stream.read((char*)&mNote, 1);
            hpe.Chord.push_back(mNote);
        }

        Stream.read((char*)&hpe.BassNote, 1);
        Stream.read((char*)&iNumberOfNotes, 1);

        for(int j = 0; j < iNumberOfNotes; j++)
        {
            Stream.read((char*)&mNote, 1);
            hpe.Tensions.push_back(mNote);
        }
        
        addEntry(hpe);
    }
}
