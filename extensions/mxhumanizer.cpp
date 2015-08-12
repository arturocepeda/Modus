
////////////////////////////////////////////////////////////////////////
//
//  Modus
//  C++ Music Library
//  [eXtension]
//
//  Copyright (c) 2012-2015 Arturo Cepeda
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

#include "mxhumanizer.h"
#include <time.h>
#include <iostream>

using namespace Modus;

void MCHumanizer::humanizeRandom(MCScore* Score, unsigned char TimeMaxError, 
                                 unsigned char IntensityMaxError, unsigned char DurationMaxError,
                                 unsigned char BeatsPerMeasure)
{
    humanizeRandom(Score, TimeMaxError, IntensityMaxError, DurationMaxError, INT_MIN, INT_MAX, BeatsPerMeasure);
}

void MCHumanizer::humanizeRandom(MCScore* Score, unsigned char TimeMaxError, 
                                 unsigned char IntensityMaxError, unsigned char DurationMaxError,
                                 int MeasureFrom, int MeasureTo, unsigned char BeatsPerMeasure)
{
    int iError;
    MSScoreEntry mAuxEntry;
    int iAuxPosition;

    // seed for random numbers: [-MaxError, MaxError]
    srand((unsigned int)time(NULL));

    for(unsigned int i = 0; i < Score->getNumberOfEntries(); i++)
    {
        // measure bounds management
        if(Score->getEntry(i)->TimePosition.Measure < MeasureFrom)
            continue;

        if(Score->getEntry(i)->TimePosition.Measure > MeasureTo)
            return;

        // ignore control notes
        if(Score->getEntry(i)->Note.Pitch > 127)
            continue;

        // intensity error
        if(Score->getEntry(i)->Note.Intensity != 0)
        {
            iError = rand() % (IntensityMaxError * 2 + 1) - IntensityMaxError;

            if(iError >= 0)
                Score->getEntry(i)->Note.Intensity = std::min(127, Score->getEntry(i)->Note.Intensity + iError);
            else
                Score->getEntry(i)->Note.Intensity = std::max(0, Score->getEntry(i)->Note.Intensity + iError);
        }

        // duration error
        if(Score->getEntry(i)->Note.Duration != 0)
        {
            iError = rand() % (DurationMaxError * 2 + 1) - DurationMaxError;

            if(iError >= 0)
                Score->getEntry(i)->Note.Duration += iError;
            else
                Score->getEntry(i)->Note.Duration = std::max(1, Score->getEntry(i)->Note.Intensity + iError);
        }

        // time error
        iError = rand() % (TimeMaxError * 2 + 1) - TimeMaxError;
        Score->getEntry(i)->TimePosition.add(iError, BeatsPerMeasure);

        if(Score->getEntry(i)->TimePosition.Measure < MeasureFrom)
        {
            Score->getEntry(i)->TimePosition.Measure = MeasureFrom;
            Score->getEntry(i)->TimePosition.Beat = 1;
            Score->getEntry(i)->TimePosition.Tick = 0;
        }

        iAuxPosition = i;

        while(iAuxPosition > 0 && Score->getEntry(iAuxPosition - 1)->TimePosition > Score->getEntry(i)->TimePosition)
            iAuxPosition--;

        if(Score->getEntry(iAuxPosition)->TimePosition > Score->getEntry(i)->TimePosition)
        {
            mAuxEntry = *Score->getEntry(i);
            Score->removeEntry(i);
            Score->addEntry(mAuxEntry);
        }
    }
}
