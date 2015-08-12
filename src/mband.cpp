
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

#include "mband.h"

using namespace Modus;

void MCBand::addInstrument(MCInstrument* Instrument)
{
    if(!Instrument)
        return;

    // check whether the instrument is already part of the band
    for(unsigned int i = 0; i < mInstruments.size(); i++)
    {
        if(mInstruments[i] == Instrument)
            return;
    }

    // if not, add it
    mInstruments.push_back(Instrument);
}

void MCBand::removeInstrument(unsigned int Position)
{
    if(Position < mInstruments.size())
        mInstruments.erase(mInstruments.begin() + Position);
}

void MCBand::removeInstrument(MCInstrument* Instrument)
{
    for(unsigned int i = 0; i < mInstruments.size(); i++)
    {
        if(mInstruments[i] == Instrument)
        {
            mInstruments.erase(mInstruments.begin() + i);
            break;
        }
    }
}

void MCBand::addBand(MCBand* Band)
{
    if(!Band)
        return;

    // check whether the band is already part of the band
    for(unsigned int i = 0; i < mBands.size(); i++)
    {
        if(mBands[i] == Band)
            return;
    }

    // if not, add it
    mBands.push_back(Band);
}

void MCBand::removeBand(unsigned int Position)
{
    if(Position < mBands.size())
        mBands.erase(mBands.begin() + Position);
}

void MCBand::removeBand(MCBand* Band)
{
    for(unsigned int i = 0; i < mBands.size(); i++)
    {
        if(mBands[i] == Band)
        {
            mBands.erase(mBands.begin() + i);
            break;
        }
    }
}

void MCBand::update(const MSTimePosition& TimePosition)
{
    unsigned int i;

    for(i = 0; i < mInstruments.size(); i++)
        mInstruments[i]->update(TimePosition);

    for(i = 0; i < mBands.size(); i++)
        mBands[i]->update(TimePosition);
}

void MCBand::clear()
{
    mInstruments.clear();
    mBands.clear();
}

MCInstrument* MCBand::getInstrument(unsigned int Position)
{
    if(Position < mInstruments.size())
        return mInstruments[Position];
    else
        return NULL;
}

unsigned int MCBand::getNumberOfInstruments()
{
    return mInstruments.size();
}

MCBand* MCBand::getBand(unsigned int Position)
{
    if(Position < mBands.size())
        return mBands[Position];
    else
        return NULL;
}

unsigned int MCBand::getNumberOfBands()
{
    return mBands.size();
}
