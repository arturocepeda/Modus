
////////////////////////////////////////////////////////////////////////
//
//  Modus v0.53
//  C++ Music Library
//
//  Copyright (c) 2012-2013 Arturo Cepeda Pérez
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

#include "mtimer.h"

//
//  Timer
//
Timer::Timer()
{
    tStartCount = tEndCount = std::chrono::high_resolution_clock::now();
    bRunning = true;
}

Timer::~Timer()
{
}

void Timer::start()
{
    bRunning = true;
    tStartCount = std::chrono::high_resolution_clock::now();
}

void Timer::stop()
{
    bRunning = false;
    tEndCount = std::chrono::high_resolution_clock::now();
}

long long Timer::getTime()
{
    if(bRunning)
        tEndCount = std::chrono::high_resolution_clock::now();

    return std::chrono::duration_cast<std::chrono::microseconds>(tEndCount - tStartCount).count();
}


//
//  MCTimer
//
MCTimer::MCTimer(float Tempo, unsigned int BeatsPerMeasure)
{
    mStructure = NULL;
    iCurrentSection = 0;
    bAccRit = false;
    cTempoLine = NULL;

    fCallbackTick = NULL;
    fCallbackSection = NULL;
    fCallbackEnd = NULL;

    iBeatsPerMeasure = BeatsPerMeasure;
    iTicksPerBeat = M_TICKS_PER_BEAT;

    setTempo(Tempo);
    reset();
}

MCTimer::~MCTimer()
{
    if(cTempoLine)
        delete cTempoLine;
}

void MCTimer::start()
{
    locateSection();
    callbackTick();

    tTimer.start();
    bRunning = true;
}

void MCTimer::stop()
{
    bRunning = false;
    tTimer.stop();
}

void MCTimer::reset()
{
    stop();

    mPosition.Measure = 1;
    mPosition.Beat = 1;
    mPosition.Tick = 0;

    iTimeNow = 0;
    iTimeBefore = 0;
}

bool MCTimer::update()
{
    if(!bRunning)
        return false;

    iTimeNow = tTimer.getTime();
    iTimeDelta = iTimeNow - iTimeBefore;
	
	if(iTimeDelta < iTick)
        return false;

	iTimeBefore = iTimeNow;

	// update current time position
	mPosition.Tick++;

	if(mPosition.Tick >= iTicksPerBeat)
	{
		mPosition.Tick = 0;
		mPosition.Beat++;
	}

	if(mPosition.Beat > iBeatsPerMeasure)
	{
		mPosition.Beat = 1;
		mPosition.Measure++;

		if(mStructure)
		{
			// check for a new section
			if(mStructure->getNumberOfEntries() > (iCurrentSection + 1) && 
			   mPosition.Measure == mStructure->getEntry(iCurrentSection + 1)->FirstMeasure)
			{
				setSectionSettings(iCurrentSection + 1);
			}

			// check for the end of the song
			else if(mStructure->getNumberOfEntries() == (iCurrentSection + 1) && 
					mPosition.Measure > mStructure->getEntry(iCurrentSection)->LastMeasure)
			{
				callbackEnd();
				stop();
			}
		}
	}

	// tick callback
	callbackTick();

	// tempo settings
	if(bAccRit && cTempoLine->can_calculate_y())
		setTempo((float)cTempoLine->y(mPosition.getInTicks(iBeatsPerMeasure)));

	return true;
}

bool MCTimer::isRunning()
{
    return bRunning;
}

void MCTimer::setSectionSettings(int iIndex)
{
    // section settings
    iCurrentSection = iIndex;
    setTempo(mStructure->getEntry(iIndex)->Tempo);
    setBeatsPerMeasure(mStructure->getEntry(iIndex)->BeatsPerMeasure);

    // section callback
    callbackSection();

    // "accelerando" or "ritardando"
    bAccRit = mStructure->getEntry(iIndex)->TargetTempo != mStructure->getEntry(iIndex)->Tempo;

    if(bAccRit)
    {
        if(cTempoLine)
            delete cTempoLine;

        MSTimePosition mSectionBegin;
        MSTimePosition mSectionEnd;

        mSectionBegin.Measure = mStructure->getEntry(iIndex)->FirstMeasure;
        mSectionEnd.Measure = mStructure->getEntry(iIndex)->LastMeasure + 1;

        cTempoLine = new CLine(mSectionBegin.getInTicks(mStructure->getEntry(iIndex)->BeatsPerMeasure), 
                               mStructure->getEntry(iIndex)->Tempo, 
                               mSectionEnd.getInTicks(mStructure->getEntry(iIndex)->BeatsPerMeasure), 
                               mStructure->getEntry(iIndex)->TargetTempo);
    }
}

void MCTimer::locateSection()
{
    if(!mStructure || mStructure->getNumberOfEntries() == 0)
        return;

    if(mPosition.Measure <= 1)
    {
        iCurrentSection = 0;
        setSectionSettings(0);
    }
    else
    {
        for(unsigned int i = 0; i < mStructure->getNumberOfEntries(); i++)
        {
            if(mPosition.Measure >= mStructure->getEntry(i)->FirstMeasure && 
               mPosition.Measure <= mStructure->getEntry(i)->LastMeasure)
            {
                iCurrentSection = i;
                setSectionSettings(i);
            }
        }
    }
}

void MCTimer::callbackTick()
{
    if(fCallbackTick)
        fCallbackTick(mPosition, fCallbackTickData);
}

void MCTimer::callbackSection()
{
    if(fCallbackSection)
        fCallbackSection(iCurrentSection, fCallbackSectionData);
}

void MCTimer::callbackEnd()
{
    if(fCallbackEnd)
        fCallbackEnd(fCallbackEndData);
}

void MCTimer::setTempo(float Tempo)
{
    if(Tempo > 0)
    {
        fTempo = Tempo;
        iTick = (long long)(60000000.0f / (fTempo * iTicksPerBeat));
    }
}

void MCTimer::setBeatsPerMeasure(unsigned int BeatsPerMeasure)
{
    if(BeatsPerMeasure > 0)
        iBeatsPerMeasure = BeatsPerMeasure;
}

void MCTimer::setTimePosition(const MSTimePosition& TimePosition)
{
    mPosition = TimePosition;

    iTimeBefore = 0;
    callbackTick();

    if(bRunning)
        locateSection();
}

void MCTimer::setTimePosition(int Measure, unsigned int Beat, unsigned int Tick)
{
    mPosition.Measure = Measure;
    mPosition.Beat = Beat;
    mPosition.Tick = Tick;

    iTimeBefore = 0;
    callbackTick();

    if(bRunning)
        locateSection();
}

void MCTimer::setCountoff(unsigned int NumberOfMeasures)
{
    setTimePosition(1 - NumberOfMeasures, 1, 0);
}

void MCTimer::setSongStructure(MCSongStructure* SongStructure)
{
    mStructure = SongStructure;
}

void MCTimer::setCallbackTick(void (*function)(const MSTimePosition& Position, void* Data), void* Data)
{
    fCallbackTick = function;
    fCallbackTickData = Data;
}

void MCTimer::setCallbackSection(void (*function)(unsigned int NewSection, void* Data), void* Data)
{
    fCallbackSection = function;
    fCallbackSectionData = Data;
}

void MCTimer::setCallbackEnd(void (*function)(void* Data), void* Data)
{
    fCallbackEnd = function;
    fCallbackEndData = Data;
}

float MCTimer::getTempo()
{
    return fTempo;
}

unsigned int MCTimer::getBeatsPerMeasure()
{
    return iBeatsPerMeasure;
}

unsigned int MCTimer::getCurrentSection()
{
    return iCurrentSection;
}

MSTimePosition &MCTimer::getTimePosition()
{
    return mPosition;
}
