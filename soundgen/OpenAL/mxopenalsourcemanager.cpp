
////////////////////////////////////////////////////////////////////////
//
//  Modus
//  C++ Music Library
//  [Sound Generator]
//
//  Copyright (c) 2012-2014 Arturo Cepeda
//
//  --------------------------------------------------------------------
//
//  Audio engine: 
//
//  OpenAL - Open Audio Library
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

#include "mxopenalsourcemanager.h"

MCOpenALSourceManager::MCOpenALSourceManager(int NumSources)
{
    iNumSources = NumSources;
    alSource = new MSOpenALSource[iNumSources];
    alSourceAlloc = new ALuint[iNumSources];
    alGenSources(iNumSources, alSourceAlloc);

    for(int i = 0; i < iNumSources; i++)
        alSource[i].Source = alSourceAlloc[i];

    iCurrentAssignment = 0;
}

MCOpenALSourceManager::~MCOpenALSourceManager()
{
    alDeleteSources(iNumSources, alSourceAlloc);
    delete[] alSourceAlloc;
    delete[] alSource;
}

int MCOpenALSourceManager::assignSource(int EntityID, int EntityChannel)
{
    int iIndexOldestAssignment = 0;
    unsigned int iOldestAssignment = alSource[0].Assignment;
    
    for(int i = 0; i < iNumSources; i++)
    {
        // this source is free
        if(alSource[i].Free)
        {
            assignSource(i, EntityID, EntityChannel);
            return i;
        }

        if(alSource[i].Assignment < iOldestAssignment)
        {
            iOldestAssignment = alSource[i].Assignment;
            iIndexOldestAssignment = i;
        }
    }

    // there are no free sources
    assignSource(iIndexOldestAssignment, EntityID, EntityChannel);
    alSourceStop(alSource[iIndexOldestAssignment].Source);
    return iIndexOldestAssignment;
}

void MCOpenALSourceManager::assignSource(int SourceIndex, int EntityID, int EntityChannel)
{
    //alSourceStop(alSource[SourceIndex].Source);
    alSource[SourceIndex].EntityID = EntityID;
    alSource[SourceIndex].EntityChannel = EntityChannel;
    alSource[SourceIndex].Assignment = ++iCurrentAssignment;
    alSource[SourceIndex].Free = false;
}

void MCOpenALSourceManager::releaseSource(int SourceIndex)
{
    if(SourceIndex < iNumSources && SourceIndex >= 0)
    {
        alSource[SourceIndex].EntityID = 0;
        alSource[SourceIndex].EntityChannel = 0;
        alSource[SourceIndex].Free = true;
    }
}

ALuint MCOpenALSourceManager::getSource(int SourceIndex)
{
    if(SourceIndex < iNumSources && SourceIndex >= 0)
        return alSource[SourceIndex].Source;
    else
        return 0;
}

int MCOpenALSourceManager::getEntityID(int SourceIndex)
{
    if(SourceIndex < iNumSources && SourceIndex >= 0)
        return alSource[SourceIndex].EntityID;
    else
        return 0;
}

int MCOpenALSourceManager::getEntityChannel(int SourceIndex)
{
    if(SourceIndex < iNumSources && SourceIndex >= 0)
        return alSource[SourceIndex].EntityChannel;
    else
        return 0;
}
