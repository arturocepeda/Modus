
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

#include "minstrument.h"
#include "mutils.h"
#include <math.h>
#include <algorithm>

MCInstrument::MCInstrument(unsigned int ID, const MSRange& Range, unsigned char NumberOfChannels)
    : iID(ID)
    , mRange(Range)
    , iNumberOfChannels(NumberOfChannels)
    , iCurrentMoment(0)
    , iScorePosition(0)
    , iTranspose(0)
    , iIntensityVariation(0)
    , mSoundGen(NULL)
    , mScore(NULL)
    , fCallbackPlay(NULL)
    , fCallbackNextNote(NULL)
    , fCallbackRelease(NULL)
    , fCallbackBending(NULL)
    , fCallbackDamper(NULL)
    , fCallbackIntensity(NULL)
{
    mCurrentNote = new MSNote[iNumberOfChannels];
    iReleaseMoment = new unsigned int[iNumberOfChannels];
    fBendingState = new float[iNumberOfChannels];
    bVibrato = new bool[iNumberOfChannels];
    fCurrentIntensity = new float[iNumberOfChannels];

    memset(mCurrentNote, 0, sizeof(MSNote) * iNumberOfChannels);
    memset(iReleaseMoment, 0, sizeof(unsigned int) * iNumberOfChannels);
    memset(fBendingState, 0, sizeof(float) * iNumberOfChannels);
    memset(bVibrato, 0, iNumberOfChannels);
    memset(fCurrentIntensity, 0, sizeof(float) * iNumberOfChannels);
}

MCInstrument::~MCInstrument()
{
    delete[] mCurrentNote;
    delete[] iReleaseMoment;
    delete[] fBendingState;
    delete[] bVibrato;
    delete[] fCurrentIntensity;
}

void MCInstrument::control(const MSNote& mNote)
{
    switch(mNote.Pitch)
    {
    // bending: Mode 0 --> up, Mode 1 --> down, Intensity --> semitones, MDA --> cents
    case M_CTRL_BEND:
        bend(mNote.Channel, ((mNote.Mode == 0)? 1: -1) * (mNote.Intensity * 100 + mNote.MDA), mNote.Duration);
        break;

    // damper: Intensity 0 --> off, Intensity 1 --> on
    case M_CTRL_DAMPER:
        setDamper(mNote.Intensity > 0);
        break;

    // intensity change: Channel, Intensity, Duration
    case M_CTRL_INTENSITY:
        changeIntensity(mNote.Channel, mNote.Intensity, mNote.Duration);
        break;

    // release speed: Intensity
    case M_CTRL_RELEASE:
        if(mSoundGen)
            mSoundGen->setReleaseSpeed(mNote.Intensity * 0.001f);
        break;

    // vibrato: Channel, Intensity --> cents, Duration --> cycle length in number of ticks
    case M_CTRL_VIBRATO:
        vibrato(mNote.Channel, mNote.Intensity, mNote.Duration);
        break;
    }
}

void MCInstrument::playNote(const MSNote& Note)
{
    if(Note.Channel >= iNumberOfChannels)
        return;

    // control notes
    if(Note.Pitch > 127)
    {
        control(Note);
        return;
    }

    // note off
    if(Note.Intensity == 0)
    {
        release(Note.Channel);
        return;
    }

    // note on
    MSNote mPlayNote = Note;
    mPlayNote.Pitch += iTranspose;

    // if the pitch is out of the instrument range, ignore the note
    if(mPlayNote.Pitch < mRange.LowestNote || mPlayNote.Pitch > mRange.HighestNote)
        return;

    // intensity
    if(iIntensityVariation != 0)
    {
        if(iIntensityVariation > 0)
            mPlayNote.Intensity = std::min(mPlayNote.Intensity + iIntensityVariation, 127);
        else
            mPlayNote.Intensity = std::max(mPlayNote.Intensity + iIntensityVariation, 1);
    }

    // reset bending state and vibrato
    fBendingState[Note.Channel] = 0.0f;
    bVibrato[Note.Channel] = false;

    // play callback
    if(fCallbackPlay)
        fCallbackPlay(iID, mPlayNote, fCallbackPlayData);

    // sound generator
    if(mSoundGen)
        mSoundGen->playNote(mPlayNote);

    // register the note
    mCurrentNote[Note.Channel] = mPlayNote;

    // release management
    if(Note.Duration > 0)
    {
        iReleaseMoment[Note.Channel] = iCurrentMoment + Note.Duration;
        vChannelsToRelease.add(Note.Channel);
    }
}

void MCInstrument::playChord(const MTNoteMap& PitchList, unsigned char Intensity, unsigned char Mode, 
                             unsigned char* ListOfChannels, unsigned char Duration)
{
    MSNote mNote;

    mNote.Intensity = Intensity;
    mNote.Mode = Mode;
    mNote.Duration = Duration;

    for(unsigned int i = 0; i < PitchList.size(); i++)
    {
        mNote.Pitch = PitchList[i];
        mNote.Channel = ListOfChannels[i];
        playNote(mNote);
        mNote.Channel++;
    }
}

void MCInstrument::playChord(const MTNoteMap& PitchList, unsigned char Intensity, unsigned char Mode, 
                             unsigned char FirstChannel, unsigned char Duration)
{
    unsigned char* iListOfChannels = new unsigned char[PitchList.size()];

    for(unsigned int i = 0; i < PitchList.size(); i++)
        iListOfChannels[i] = FirstChannel + i;

    playChord(PitchList, Intensity, Mode, iListOfChannels, Duration);

    delete[] iListOfChannels;
}

void MCInstrument::release(unsigned char Channel)
{
    if(Channel >= iNumberOfChannels)
        return;

    // release callback
    if(fCallbackRelease)
        fCallbackRelease(iID, mCurrentNote[Channel], fCallbackReleaseData);

    memset(&mCurrentNote[Channel], 0, sizeof(MSNote));
    bVibrato[Channel] = false;

    // sound generator
    if(mSoundGen)
        mSoundGen->releaseNote(Channel);
}

void MCInstrument::releaseAll()
{
    for(unsigned char i = 0; i < iNumberOfChannels; i++)
    {
        // release callback
        if(fCallbackRelease && mCurrentNote[i].Pitch > 0)
            fCallbackRelease(iID, mCurrentNote[i], fCallbackReleaseData);

        memset(&mCurrentNote[i], 0, sizeof(MSNote));
    }

    // sound generator
    if(mSoundGen)
        mSoundGen->releaseAllNotes();
}

void MCInstrument::bend(unsigned char Channel, int Cents)
{
    if(Channel >= iNumberOfChannels)
        return;

    fBendingState[Channel] = (float)Cents;

    // callback
    if(fCallbackBending)
        fCallbackBending(iID, mCurrentNote[Channel], Cents, fCallbackBendingData);

    // sound generator
    if(mSoundGen)
        mSoundGen->setBending(Channel, Cents);
}

void MCInstrument::bend(unsigned char Channel, int Cents, unsigned int NumberOfTicks)
{
    if(Channel >= iNumberOfChannels)
        return;

    if(NumberOfTicks <= 1)
    {
        bend(Channel, Cents);
        return;
    }

    // bending settings
    MSNoteChange mBending;

    mBending.Channel = Channel;
    mBending.NumberOfSteps = NumberOfTicks;
    mBending.CurrentStep = 0;
    mBending.Increment = (Cents - fBendingState[Channel]) / NumberOfTicks;

    vBending.push_back(mBending);
}

void MCInstrument::vibrato(unsigned char Channel, int Cents, unsigned int NumberOfTicksPerCycle)
{
    if(Channel >= iNumberOfChannels)
        return;

    // vibrato settings (channel, amplitude, current value, increment)
    MSNoteChange mVibrato;

    mVibrato.Channel = Channel;
    mVibrato.Amplitude = (float)Cents;
    mVibrato.CurrentPosition = PI;
    mVibrato.Increment = 2.0f * PI / (float)NumberOfTicksPerCycle;

    vVibrato.push_back(mVibrato);
    bVibrato[Channel] = true;
}

void MCInstrument::changeIntensity(unsigned char Channel, unsigned char Intensity)
{
    if(Channel >= iNumberOfChannels)
        return;

    mCurrentNote[Channel].Intensity = Intensity;

    // callback
    if(fCallbackIntensity)
        fCallbackIntensity(iID, mCurrentNote[Channel], Intensity, fCallbackIntensityData);

    // sound generator
    if(mSoundGen)
        mSoundGen->setIntensity(Channel, Intensity);
}

void MCInstrument::changeIntensity(unsigned char Channel, unsigned char Intensity, unsigned int NumberOfTicks)
{
    if(Channel >= iNumberOfChannels)
        return;

    if(NumberOfTicks <= 1)
    {
        changeIntensity(Channel, Intensity);
        return;
    }

    // intensity change settings (channel, number of steps, current step, increment)
    fCurrentIntensity[Channel] = (float)mCurrentNote[Channel].Intensity;
    MSNoteChange mIntensityChange = {Channel, NumberOfTicks, 0, (float)(Intensity - mCurrentNote[Channel].Intensity) / NumberOfTicks};
    vIntensityChange.push_back(mIntensityChange);
}

void MCInstrument::update(const MSTimePosition& TimePosition)
{
    unsigned int i;

    iCurrentMoment++;

    // check for notes to play on the score
    if(mScore && mScore->getNumberOfEntries() > 0)
        checkScore(TimePosition);

    // check for channels to release
    for(i = 0; i < vChannelsToRelease.size(); i++)
    {
        if(iCurrentMoment == iReleaseMoment[vChannelsToRelease[i]])
        {
            release(vChannelsToRelease[i]);
            vChannelsToRelease.remove(i);
            i--;
        }
    }

    // check for bending
    for(i = 0; i < vBending.size(); i++)
    {
        bend(vBending[i].Channel, (int)(fBendingState[vBending[i].Channel] + vBending[i].Increment));
        vBending[i].CurrentStep++;

        // the desired position has been reached
        if(vBending[i].CurrentStep == vBending[i].NumberOfSteps)
        {
            vBending.erase(vBending.begin() + i);
            i--;
        }
    }

    // check for vibrato
    for(i = 0; i < vVibrato.size(); i++)
    {
        if(!bVibrato[vVibrato[i].Channel])
        {
            vVibrato.erase(vVibrato.begin() + i);
            i--;
            continue;
        }

        // [1/2 + 1/2 * cos(a)] gives a value between 0 and 1, which multiplied by the amplitude (the maximum 
        // number of cents for the vibrato) gives the current number of cents
        vVibrato[i].CurrentPosition += vVibrato[i].Increment;
        bend(vVibrato[i].Channel, (int)((0.5f + 0.5f * cos(vVibrato[i].CurrentPosition)) * vVibrato[i].Amplitude));
    }

    // check for intensity changes
    for(i = 0; i < vIntensityChange.size(); i++)
    {
        fCurrentIntensity[vIntensityChange[i].Channel] += vIntensityChange[i].Increment;
        changeIntensity(vIntensityChange[i].Channel, (unsigned char)fCurrentIntensity[vIntensityChange[i].Channel]);
        vIntensityChange[i].CurrentStep++;

        // the desired intensity has been reached
        if(vIntensityChange[i].CurrentStep == vIntensityChange[i].NumberOfSteps)
        {
            vIntensityChange.erase(vIntensityChange.begin() + i);
            i--;
        }
    }

    // update the sound generator
    if(mSoundGen)
        mSoundGen->update();
}

void MCInstrument::checkScore(const MSTimePosition& TimePosition)
{
    MSScoreEntry* se;
    unsigned int iScoreEntries = mScore->getNumberOfEntries();

    // since the instrument must be capable of follow the score even when it has been changed dynamically, 
    // we must always ensure that the current position is set on an entry of which the time position is greater 
    // or equal than the current time position, while the time position of the previous entry (if there is a 
    // previous entry) is less than the current time position

    // 1) check if there are more notes to play on the score
    if(iScorePosition >= iScoreEntries)
    {
        se = mScore->getEntry(iScoreEntries - 1);

        // there are no more notes to play, so we just leave
        if(se->TimePosition < TimePosition)
        {
            return;
        }

        // there are more notes to play
        else
        {
            se = mScore->getEntry(0);

            if(se->TimePosition >= TimePosition)
                iScorePosition = 0;
            else
                iScorePosition = iScoreEntries - 1;
        }
    }

    // 2) set our eyes looking to the next note to play
    se = mScore->getEntry(iScorePosition);

    if(se->TimePosition < TimePosition)
    {
        while(se->TimePosition < TimePosition)
        {
            iScorePosition++;

            if(iScorePosition == iScoreEntries)
                return;

            se = mScore->getEntry(iScorePosition);
        }
    }
    else if(iScorePosition > 0)
    {
        se = mScore->getEntry(iScorePosition - 1);

        while(se->TimePosition >= TimePosition)
        {
            iScorePosition--;

            if(iScorePosition == 0)
                break;

            se = mScore->getEntry(iScorePosition - 1);
        }
    }
    
    // 3) play the note(s)
    se = mScore->getEntry(iScorePosition);
    bool bPlayed = false;

    while(se->TimePosition == TimePosition)
    {
        playNote(se->Note);
        iScorePosition++;
        bPlayed = true;

        if(iScorePosition == iScoreEntries)
            break;

        se = mScore->getEntry(iScorePosition);
    }

    // next note callback
    if(!bPlayed || !fCallbackNextNote)
        return;

    unsigned int iNextNoteScorePosition = iScorePosition;
    MSTimePosition& mNextNoteTimePosition = se->TimePosition;

    while(iNextNoteScorePosition < iScoreEntries && se->TimePosition == mNextNoteTimePosition)
    {
        fCallbackNextNote(iID, se->Note, TimePosition, se->TimePosition, fCallbackNextNoteData);
        se = mScore->getEntry(++iNextNoteScorePosition);
    }
}

unsigned int MCInstrument::getID()
{
    return iID;
}

unsigned char MCInstrument::getNumberOfChannels()
{
    return iNumberOfChannels;
}

MSRange& MCInstrument::getRange()
{
    return mRange;
}

unsigned char MCInstrument::getRangeSize()
{
    return mRange.getSize();
}

void MCInstrument::setSoundGen(MCSoundGen* SoundGen)
{
    mSoundGen = SoundGen;
}

void MCInstrument::setScore(MCScore* Score)
{
    mScore = Score;
}

void MCInstrument::setDamper(bool DamperState)
{
    // callback
    if(fCallbackDamper)
        fCallbackDamper(iID, DamperState, fCallbackDamperData);

    // sound generator
    if(mSoundGen)
        mSoundGen->setDamper(DamperState);
}

void MCInstrument::setTranspose(char NumberOfSemitones)
{
    iTranspose = NumberOfSemitones;
}

void MCInstrument::setIntensityVariation(char Variation)
{
    iIntensityVariation = Variation;
}

void MCInstrument::setCallbackPlay(void (*function)(unsigned int InstrumentID, const MSNote& Note, void* Data),
                                   void* Data)
{
    fCallbackPlay = function;
    fCallbackPlayData = Data;
}

void MCInstrument::setCallbackNextNote(void (*function)(unsigned int InstrumentID, const MSNote& Note, 
                                                        const MSTimePosition& CurrentTimePosition,
                                                        const MSTimePosition& NoteTimePosition, 
                                                        void* Data),
                                       void* Data)
{
    fCallbackNextNote = function;
    fCallbackNextNoteData = Data;
}

void MCInstrument::setCallbackRelease(void (*function)(unsigned int InstrumentID, const MSNote& Note, void* Data),
                                      void* Data)
{
    fCallbackRelease = function;
    fCallbackReleaseData = Data;
}

void MCInstrument::setCallbackBending(void (*function)(unsigned int InstrumentID, const MSNote& Note, int Cents, 
                                                       void* Data), 
                                      void* Data)
{
    fCallbackBending = function;
    fCallbackBendingData = Data;
}

void MCInstrument::setCallbackDamper(void (*function)(unsigned int InstrumentID, bool DamperState, void* Data), 
                                     void* Data)
{
    fCallbackDamper = function;
    fCallbackDamperData = Data;
}

void MCInstrument::setCallbackIntensity(void (*function)(unsigned int InstrumentID, const MSNote& Note, 
                                                         unsigned char Intensity, void* Data), 
                                        void* Data)
{
    fCallbackIntensity = function;
    fCallbackIntensityData = Data;
}
