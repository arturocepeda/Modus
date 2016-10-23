
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

#include "mscore.h"
#include "mutils.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

using namespace Modus;

MCScore::MCScore()
{
}

MCScore::~MCScore()
{
}

void MCScore::addEntry(const MSScoreEntry& ScoreEntry)
{
    if(sEntry.size() == 0)
    {
        sEntry.push_back(ScoreEntry);
        return;    
    }

    int iFirst = 0;
    int iLast = sEntry.size() - 1;
    int iEntryPosition = ScoreEntry.TimePosition.getInTicks(iBeatsPerMeasure);

    // first element
    if(iEntryPosition <= sEntry[iFirst].TimePosition.getInTicks(iBeatsPerMeasure))
    {
        sEntry.insert(sEntry.begin(), ScoreEntry);
    }

    // last element
    else if(iEntryPosition >= sEntry[iLast].TimePosition.getInTicks(iBeatsPerMeasure))
    {
        sEntry.push_back(ScoreEntry);
    }
       
    // another position in the middle
    else
    {
        int iMiddle;

        do
        {
            iMiddle = (iFirst + iLast) / 2;

            if(iEntryPosition <= sEntry[iMiddle].TimePosition.getInTicks(iBeatsPerMeasure))
                iLast = iMiddle - 1;
            else
                iFirst = iMiddle + 1;

        } while(iFirst <= iLast);

        sEntry.insert(sEntry.begin() + iFirst, ScoreEntry);
    }
}

void MCScore::addScore(MCScore& Score)
{
    for(unsigned int i = 0; i < Score.getNumberOfEntries(); i++)
        addEntry(*Score.getEntry(i));
}

void MCScore::transpose(char NumberOfSemitones)
{
    for(unsigned int i = 0; i < sEntry.size(); i++)
        sEntry[i].Note.Pitch += NumberOfSemitones;
}

void MCScore::changeIntensity(char Increment)
{
    if(Increment > 0)
    {
        for(unsigned int i = 0; i < sEntry.size(); i++)
            sEntry[i].Note.Intensity = std::min(127, sEntry[i].Note.Intensity + Increment);
    }
    else
    {
        for(unsigned int i = 0; i < sEntry.size(); i++)
            sEntry[i].Note.Intensity = std::max(1, sEntry[i].Note.Intensity + Increment);
    }
}

void MCScore::loadScript(std::istream* sScript)
{
    char sLine[MODUS_LINE_BUFFER];
    int iValue;

    MSScoreEntry se;

    se.TimePosition.Measure = 1;
    se.TimePosition.Beat = 1;
    se.TimePosition.Tick = 0;

    se.Note.Channel = 0;
    se.Note.Duration = 0;
    se.Note.Intensity = 0;
    se.Note.Mode = 0;
    se.Note.Pitch = 0;

    // clear all the previous score entries
    clear();

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
            iValue = readScriptValue(sLine, "m:");
            if(iValue > -1)
            {
                se.TimePosition.Measure = iValue;
                se.TimePosition.Beat = 1;
                se.TimePosition.Tick = 0;
            }

            iValue = readScriptValue(sLine, "b:");
            if(iValue > -1)
            {
                se.TimePosition.Beat = iValue;
                se.TimePosition.Tick = 0;
            }

            iValue = readScriptValue(sLine, "t:");
            if(iValue > -1)
                se.TimePosition.Tick = iValue;

            continue;
        }

        // metadata values don't remain when a new entry comes
        se.Note.MDA = 0;
        se.Note.MDB = 0;
        se.Note.MDC = 0;

        // bpm: beats per measure
        iValue = readScriptValue(sLine, "bpm:");
        if(iValue > 0)
            iBeatsPerMeasure = iValue;

        // p: pitch
        iValue = readScriptValue(sLine, "p:");
        if(iValue > -1)
            se.Note.Pitch = iValue;

        // d: duration
        iValue = readScriptValue(sLine, "d:");
        if(iValue > -1)
            se.Note.Duration = iValue;

        // ch: channel
        iValue = readScriptValue(sLine, "ch:");
        if(iValue > -1)
            se.Note.Channel = iValue;

        // i: intensity
        iValue = readScriptValue(sLine, "i:");
        if(iValue > -1)
            se.Note.Intensity = iValue;

        // mode: mode
        iValue = readScriptValue(sLine, "mode:");
        if(iValue > -1)
            se.Note.Mode = iValue;

        // mda: metadata A
        iValue = readScriptValue(sLine, "mda:");
        if(iValue > -1)
            se.Note.MDA = iValue;

        // mdb: metadata B
        iValue = readScriptValue(sLine, "mdb:");
        if(iValue > -1)
            se.Note.MDB = iValue;

        // mdc: metadata C
        iValue = readScriptValue(sLine, "mdc:");
        if(iValue > -1)
            se.Note.MDC = iValue;

        // add entry!
        if(strchr(sLine, '>') != NULL)
        {
            addEntry(se);

            if(strchr(sLine, '+') != NULL)
                se.TimePosition.add(se.Note.Duration, iBeatsPerMeasure);
        }
    }
}

bool MCScore::loadScriptFromFile(const char* Filename)
{
    std::ifstream fFile(Filename);

    if(!fFile.is_open())
        return false;

    loadScript(&fFile);

    fFile.close();

    return true;
}

void MCScore::loadScriptFromString(const char* Script)
{
    std::string sScriptString(Script);
    std::istringstream sScript(sScriptString);

    loadScript(&sScript);
}

bool MCScore::loadScriptFromBinaryFile(const char* Filename)
{
    std::ifstream fFile(Filename, std::ifstream::binary);

    if(!fFile.is_open())
        return false;

    loadScriptFromBinaryData(fFile);
    fFile.close();

    return true;
}

void MCScore::loadScriptFromBinaryData(std::istream& Stream)
{
    // clear all the previous score entries
    clear();

    // header
    char sBuffer[16];
    Stream.read(sBuffer, 16);

    // metadata fields
    bool bMetadata;
    Stream.read((char*)&bMetadata, 1);

    // number of entries
    unsigned int iNumberOfEntries;
    Stream.read((char*)&iNumberOfEntries, sizeof(unsigned int));

    // entries
    MSScoreEntry se;

    for(unsigned int i = 0; Stream.good() && i < iNumberOfEntries; i++)
    {
        Stream.read((char*)&se.TimePosition.Measure, sizeof(short));
        Stream.read((char*)&se.TimePosition.Beat, 1);
        Stream.read((char*)&se.TimePosition.Tick, 1);
        Stream.read((char*)&se.Note.Mode, 1);
        Stream.read((char*)&se.Note.Channel, 1);
        Stream.read((char*)&se.Note.Pitch, 1);
        Stream.read((char*)&se.Note.Intensity, 1);
        Stream.read((char*)&se.Note.Duration, 1);

        if(bMetadata)
        {
            Stream.read((char*)&se.Note.MDA, 1);
            Stream.read((char*)&se.Note.MDB, 1);
            Stream.read((char*)&se.Note.MDC, 1);
        }
        
        addEntry(se);
    }
}
