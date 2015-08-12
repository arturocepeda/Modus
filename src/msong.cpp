
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

#include "msong.h"
#include "mutils.h"
#include <iostream>
#include <fstream>
#include <sstream>

using namespace Modus;

MCSongStructure::MCSongStructure()
{
}

MCSongStructure::~MCSongStructure()
{
}

void MCSongStructure::addEntry(const MSSongSection& Section)
{
    if(sEntry.size() == 0)
    {
        sEntry.push_back(Section);
        return;    
    }

    int iNewEntryMeasure;
    int iFirstEntryMeasure;
    int iLastEntryMeasure;

    iNewEntryMeasure = Section.FirstMeasure;
    iFirstEntryMeasure = sEntry[0].FirstMeasure;
    iLastEntryMeasure = sEntry[sEntry.size() - 1].FirstMeasure;

    // first element
    if(iNewEntryMeasure < iFirstEntryMeasure)
    {
        sEntry.insert(sEntry.begin(), Section);
    }

    // last element
    else if(iNewEntryMeasure >= iLastEntryMeasure)
    {
        sEntry.push_back(Section);
    }
    
    // another position in the middle
    else
    {
        int iCurrentEntryMeasure;

        for(unsigned int i = 1; i < sEntry.size(); i++)
        {
            iCurrentEntryMeasure = sEntry[i].FirstMeasure;

            if(iNewEntryMeasure < iCurrentEntryMeasure)
            {
                sEntry.insert(sEntry.begin() + i, Section);
                break;
            }
        }
    }
}

void MCSongStructure::loadScript(std::istream* sScript)
{
    char sLine[MODUS_LINE_BUFFER];
    int iValue;

    MSSongSection ss;

    ss.BeatsPerMeasure = 4;
    ss.FirstMeasure = 1;
    ss.LastMeasure = 32;
    ss.Tempo = 120;
    ss.TargetTempo = 120;

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

        // bpm: beats per measure
        iValue = readScriptValue(sLine, "bpm:");
        if(iValue > 0)
            ss.BeatsPerMeasure = iValue;

        // measures: from, to
        iValue = readScriptValue(sLine, "from:");
        if(iValue > 0)
        {
            ss.FirstMeasure = iValue;
            ss.LastMeasure = iValue;
        }

        iValue = readScriptValue(sLine, "to:");
        if(iValue > ss.FirstMeasure)
            ss.LastMeasure = iValue;

        // tempo
        iValue = readScriptValue(sLine, "tempo:");
        if(iValue > -1)
        {
            ss.Tempo = (float)iValue;
            ss.TargetTempo = (float)iValue;
        }

        // target tempo
        iValue = readScriptValue(sLine, "target:");
        if(iValue > -1)
            ss.TargetTempo = (float)iValue;

        // add entry!
        if(strchr(sLine, '>') != NULL)
            addEntry(ss);
    }
}

bool MCSongStructure::loadScriptFromFile(const char* Filename)
{
    std::ifstream fFile(Filename);

    if(!fFile.is_open())
        return false;

    loadScript(&fFile);

    fFile.close();

    return true;
}

void MCSongStructure::loadScriptFromString(const char* Script)
{
    std::string sScriptString(Script);
    std::istringstream sScript(sScriptString);

    loadScript(&sScript);
}

bool MCSongStructure::loadScriptFromBinaryFile(const char* Filename)
{
    std::ifstream fFile(Filename, std::ifstream::binary);

    if(!fFile.is_open())
        return false;

    loadScriptFromBinaryData(fFile);
    fFile.close();

    return true;
}

void MCSongStructure::loadScriptFromBinaryData(std::istream& Stream)
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
    MSSongSection mSongSection;

    for(unsigned int i = 0; Stream.good() && i < iNumberOfEntries; i++)
    {
        Stream.read((char*)&mSongSection.BeatsPerMeasure, sizeof(unsigned int));
        Stream.read((char*)&mSongSection.FirstMeasure, sizeof(int));
        Stream.read((char*)&mSongSection.LastMeasure, sizeof(int));
        Stream.read((char*)&mSongSection.Tempo, sizeof(float));
        Stream.read((char*)&mSongSection.TargetTempo, sizeof(float));

        addEntry(mSongSection);
    }
}
