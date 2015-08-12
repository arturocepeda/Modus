
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

#include "mstringinstrument.h"

using namespace Modus;

//
//  MCStringInstrument
//
MCStringInstrument::MCStringInstrument(unsigned int ID, unsigned char NumberOfStrings)
    : MCInstrument(ID, MSRange(255, 0), NumberOfStrings)
{
    mStrings = new MTNote[NumberOfStrings];
    memset(mStrings, 0, NumberOfStrings);
}

MCStringInstrument::~MCStringInstrument()
{
    delete[] mStrings;
}

void MCStringInstrument::setOpenStringNote(unsigned char StringNumber, MTNote Note)
{
    if(StringNumber <= iNumberOfChannels && StringNumber > 0)
        mStrings[StringNumber - 1] = Note;
}


//
//  MCFrettedInstrument
//
MCFrettedInstrument::MCFrettedInstrument(unsigned int ID, unsigned char NumberOfStrings, unsigned char NumberOfFrets)
    : MCStringInstrument(ID, NumberOfStrings)
    , iNumberOfFrets(NumberOfFrets)
{
    mRange = MSRange(0, NumberOfFrets);
}

MCFrettedInstrument::~MCFrettedInstrument()
{
}

char MCFrettedInstrument::getFret(MTNote iNote, unsigned char iString)
{
    return iNote - mStrings[iString - 1];
}
