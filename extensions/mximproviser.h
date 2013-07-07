
////////////////////////////////////////////////////////////////////////
//
//  Modus v0.50
//  C++ Music Library
//  [eXtension]
//
//  Copyright (c) 2012 Arturo Cepeda
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

#ifndef _MXIMPROVISER_H_
#define _MXIMPROVISER_H_

#include "mtypes.h"
#include "mscore.h"
#include "mharmonypattern.h"
#include "mscalepattern.h"
#include <time.h>

class MCImproviser
{
protected:
    MCHarmonyPattern* mHarmonyPattern;
    MCScalePattern* mScalePattern;
    MSRange mRange;

    MTNote mPreviousNote;
    MTNote mCurrentNote;
    MSTimePosition mTimePosition;
    unsigned char iBeatsPerMeasure;
    unsigned char iIntensity;

    void init();

public:
    virtual void improvise(MCScore* Score, int MeasureFrom, int MeasureTo) = 0;

    unsigned char getIntensity();

    void setHarmonyPattern(MCHarmonyPattern* HarmonyPattern);
    void setScalePattern(MCScalePattern* ScalePattern);
    void setRange(const MSRange& Range);
    void setRange(MTNote NoteFrom, MTNote NoteTo);
    void setBeatsPerMeasure(unsigned char BeatsPerMeasure);
    void setIntensity(unsigned char Intensity);
    void setPreviousNote(MTNote PreviousNote);
};

class MCWalkingBassImproviser : public MCImproviser
{
private:
    void fillLinkPatterns(MTNoteMapList* mLinkPatterns, MTNote mNote);

public:
    MCWalkingBassImproviser();
    ~MCWalkingBassImproviser();

    void improvise(MCScore* Score, int MeasureFrom, int MeasureTo);
};

#endif
