
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

#include "mscalepattern.h"
#include "mscales.h"
#include "mutils.h"
#include <iostream>
#include <fstream>
#include <sstream>

MCScalePattern::MCScalePattern()
{
}

MCScalePattern::~MCScalePattern()
{
}

void MCScalePattern::addEntry(const MSScalePatternEntry& ScalePatternEntry)
{
    MSScalePatternEntry mNewScalePatternEntry = ScalePatternEntry;

    while(mNewScalePatternEntry.RootNote >= 12)
        mNewScalePatternEntry.RootNote -= 12;

    if(sEntry.size() == 0)
    {
        sEntry.push_back(mNewScalePatternEntry);
        return;    
    }

    MSTimePosition mNewEntryTimePosition;
    MSTimePosition mFirstEntryTimePosition;
    MSTimePosition mLastEntryTimePosition;

    mNewEntryTimePosition = mNewScalePatternEntry.TimePosition;
    mFirstEntryTimePosition = sEntry[0].TimePosition;
    mLastEntryTimePosition = sEntry[sEntry.size() - 1].TimePosition;

    unsigned int iNewEntryPosition = mNewEntryTimePosition.getInTicks(iBeatsPerMeasure);
    unsigned int iFirstEntryPosition = mFirstEntryTimePosition.getInTicks(iBeatsPerMeasure);
    unsigned int iLastEntryPosition = mLastEntryTimePosition.getInTicks(iBeatsPerMeasure);
    unsigned int iCurrentEntryPosition;

    // first element
    if(iNewEntryPosition < iFirstEntryPosition)
    {
        sEntry.insert(sEntry.begin(), mNewScalePatternEntry);
    }

    // last element
    else if(iNewEntryPosition >= iLastEntryPosition)
    {
        sEntry.push_back(mNewScalePatternEntry);
    }
    
    // another position in the middle
    else
    {
        for(unsigned int i = 1; i < sEntry.size(); i++)
        {
            iCurrentEntryPosition = sEntry[i].TimePosition.getInTicks(iBeatsPerMeasure);

            if(iNewEntryPosition < iCurrentEntryPosition)
            {
                sEntry.insert(sEntry.begin() + i, mNewScalePatternEntry);
                break;
            }
        }
    }
}

void MCScalePattern::transpose(char NumberOfSemitones)
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
    }
}

void MCScalePattern::loadScript(std::istream* sScalesDefinition, std::istream* sScript)
{
    // load the scales list
    char sLine[LINE_BUFFER];
    char* sLinePointer;
    int iValue;

    MTScale sAux;
    MTScaleList sScales;

    while(sScalesDefinition->good())
    {
        // read a new line
        sScalesDefinition->getline(sLine, sizeof(sLine), '\n');

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
            sScales.push_back(sAux);
    }
    
    if(sScales.size() == 0)
        return;

    // clear all the previous scale entries
    clear();

    MSScalePatternEntry spe;
    spe.TimePosition.Measure = 1;
    spe.TimePosition.Beat = 1;
    spe.TimePosition.Tick = 0;
    spe.RootNote = 0;
    spe.Scale = sScales[0];

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
                spe.TimePosition.Measure = iValue;
                spe.TimePosition.Beat = 1;
            }

            iValue = readScriptValue(sLine, "b");
            if(iValue > -1)
                spe.TimePosition.Beat = iValue;

            continue;
        }

        // bpm: beats per measure
        iValue = readScriptValue(sLine, "bpm:");
        if(iValue > 0)
            iBeatsPerMeasure = iValue;

        // root: root note
        iValue = readScriptValue(sLine, "root:");
        if(iValue > -1)
            spe.RootNote = iValue;

        // scale: scale number
        iValue = readScriptValue(sLine, "scale:");
        if(iValue > -1 && iValue < (int)sScales.size())
            spe.Scale = sScales[iValue];

        // add entry!
        if(strchr(sLine, '>') != NULL)
        {
            addEntry(spe);

            if(strchr(sLine, '+') != NULL)
            {
                spe.TimePosition.Beat++;

                if(spe.TimePosition.Beat > iBeatsPerMeasure)
                {
                    spe.TimePosition.Measure++;
                    spe.TimePosition.Beat = 1;
                }
            }
        }
    }
}

bool MCScalePattern::loadScriptFromFile(const char* ScalesDefinitionFilename, const char* ScriptFilename)
{
    // open the files
    std::ifstream fScalesFile(ScalesDefinitionFilename);

    if(!fScalesFile.is_open())
        return false;

    std::ifstream fScriptFile(ScriptFilename);

    if(!fScriptFile.is_open())
    {
        fScalesFile.close();
        return false;
    }

    loadScript(&fScalesFile, &fScriptFile);

    // close the files
    fScalesFile.close();
    fScriptFile.close();

    return true;
}

bool MCScalePattern::loadScriptFromFile(const char* ScriptFilename)
{
    std::string sScalesDefinitionString(MCScales::ScalesDefinition);
    std::istringstream sScalesDefinition(sScalesDefinitionString);

    // open the file
    std::ifstream fScriptFile(ScriptFilename);

    if(!fScriptFile.is_open())
        return false;

    loadScript(&sScalesDefinition, &fScriptFile);

    // close the file
    fScriptFile.close();

    return true;
}

void MCScalePattern::loadScriptFromString(const char* ScalesDefinition, const char* Script)
{
    std::string sScalesDefinitionString(ScalesDefinition);
    std::string sScriptString(Script);

    std::istringstream sScalesDefinition(sScalesDefinitionString);
    std::istringstream sScript(sScriptString);

    loadScript(&sScalesDefinition, &sScript);
}

void MCScalePattern::loadScriptFromString(const char* Script)
{
    loadScriptFromString(MCScales::ScalesDefinition, Script);
}

bool MCScalePattern::loadScriptFromBinaryFile(const char* Filename)
{
    std::ifstream fFile(Filename, std::ifstream::binary);

    if(!fFile.is_open())
        return false;

    loadScriptFromBinaryData(fFile);
    fFile.close();

    return true;
}

void MCScalePattern::loadScriptFromBinaryData(std::istream& Stream)
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
    MSScalePatternEntry spe;
    char iNumberOfNotes;
    MTNote mNote;

    for(unsigned int i = 0; Stream.good() && i < iNumberOfEntries; i++)
    {
        spe.Scale.clear();

        Stream.read((char*)&spe.TimePosition.Measure, sizeof(short));
        Stream.read((char*)&spe.TimePosition.Beat, 1);
        Stream.read((char*)&spe.TimePosition.Tick, 1);
        Stream.read((char*)&spe.RootNote, 1);
        Stream.read(&iNumberOfNotes, 1);

        for(char j = 0; j < iNumberOfNotes; j++)
        {
            Stream.read((char*)&mNote, 1);
            spe.Scale.push_back(mNote);
        }
        
        addEntry(spe);
    }
}
