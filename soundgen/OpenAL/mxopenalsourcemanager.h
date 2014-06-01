
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

#ifndef _MXOPENALSOURCEMANAGER_H_
#define _MXOPENALSOURCEMANAGER_H_

#include "./../externals/OpenAL/include/al.h"
#include "./../externals/OpenAL/include/alc.h"

struct MSOpenALSource
{
    ALuint Source;
    int EntityID;
    int EntityChannel;
    unsigned int Assignment;
    bool Free;

    MSOpenALSource()
    {
        Source = 0;
        EntityID = 0;
        EntityChannel = 0;
        Assignment = 0;
        Free = true;
    }
};

class MCOpenALSourceManager
{
private:
    int iNumSources;
    ALuint* alSourceAlloc;
    MSOpenALSource* alSource;
    unsigned int iCurrentAssignment;

    void assignSource(int SourceIndex, int EntityID, int EntityChannel);

public:
    MCOpenALSourceManager(int NumSources);
    ~MCOpenALSourceManager();

    int assignSource(int EntityID, int EntityChannel);
    void releaseSource(int SourceIndex);

    ALuint getSource(int SourceIndex);
    int getEntityID(int SourceIndex);
    int getEntityChannel(int SourceIndex);
};

#endif
