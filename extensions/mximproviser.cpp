
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

#include "mximproviser.h"
#include "mutils.h"

//
//  MCImproviser
//
void MCImproviser::init()
{
    mTimePosition.Tick = 0;
    mHarmonyPattern = NULL;
    mScalePattern = NULL;
    iBeatsPerMeasure = 4;
    iIntensity = 127;
    mPreviousNote = 0;

    mRange.LowestNote = 0;
    mRange.HighestNote = 127;

    srand((unsigned int)time(0));
}

unsigned char MCImproviser::getIntensity()
{
    return iIntensity;
}

void MCImproviser::setHarmonyPattern(MCHarmonyPattern* HarmonyPattern)
{
    mHarmonyPattern = HarmonyPattern;
}

void MCImproviser::setScalePattern(MCScalePattern* ScalePattern)
{
    mScalePattern = ScalePattern;
}

void MCImproviser::setRange(const MSRange& Range)
{
    mRange.LowestNote = Range.LowestNote;
    mRange.HighestNote = Range.HighestNote;
}

void MCImproviser::setRange(MTNote NoteFrom, MTNote NoteTo)
{
    if(NoteFrom < NoteTo)
    {
        mRange.LowestNote = NoteFrom;
        mRange.HighestNote = NoteTo;
    }
    else
    {
        mRange.LowestNote = NoteTo;
        mRange.HighestNote = NoteFrom;
    }
}

void MCImproviser::setBeatsPerMeasure(unsigned char BeatsPerMeasure)
{
    if(BeatsPerMeasure > 0)
        iBeatsPerMeasure = BeatsPerMeasure;
}

void MCImproviser::setIntensity(unsigned char Intensity)
{
    iIntensity = Intensity;
}

void MCImproviser::setPreviousNote(MTNote PreviousNote)
{
    mPreviousNote = PreviousNote;
}



//
//  MCWalkingBassImproviser
//
MCWalkingBassImproviser::MCWalkingBassImproviser()
{
    init();
}

MCWalkingBassImproviser::~MCWalkingBassImproviser()
{
}

void MCWalkingBassImproviser::fillLinkPatterns(MTNoteMapList* mLinkPatterns, MTNote mNote)
{
    MTNoteMap mLink;

    mLinkPatterns->clear();

    // major
    mLink.clear(); 
    mLink.push_back(mNote); mLink.push_back(mNote + 2); mLink.push_back(mNote + 4); mLink.push_back(mNote + 7);
    mLinkPatterns->push_back(mLink);

    mLink.clear(); 
    mLink.push_back(mNote); mLink.push_back(mNote + 2); mLink.push_back(mNote + 4); mLink.push_back(mNote + 0);
    mLinkPatterns->push_back(mLink);
    
    mLink.clear(); 
    mLink.push_back(mNote); mLink.push_back(mNote - 1); mLink.push_back(mNote - 3); mLink.push_back(mNote - 5);
    mLinkPatterns->push_back(mLink);

    mLink.clear(); 
    mLink.push_back(mNote); mLink.push_back(mNote - 1); mLink.push_back(mNote + 2); mLink.push_back(mNote);
    mLinkPatterns->push_back(mLink);

    mLink.clear(); 
    mLink.push_back(mNote); mLink.push_back(mNote - 5); mLink.push_back(mNote - 8); mLink.push_back(mNote - 10);
    mLinkPatterns->push_back(mLink);

    // minor
    mLink.clear(); 
    mLink.push_back(mNote); mLink.push_back(mNote + 2); mLink.push_back(mNote + 3); mLink.push_back(mNote + 7);
    mLinkPatterns->push_back(mLink);

    mLink.clear(); 
    mLink.push_back(mNote); mLink.push_back(mNote + 2); mLink.push_back(mNote + 3); mLink.push_back(mNote + 0);
    mLinkPatterns->push_back(mLink);

    mLink.clear(); 
    mLink.push_back(mNote); mLink.push_back(mNote - 12); mLink.push_back(mNote - 10); mLink.push_back(mNote - 8);
    mLinkPatterns->push_back(mLink);

    mLink.clear(); 
    mLink.push_back(mNote); mLink.push_back(mNote - 2); mLink.push_back(mNote - 4); mLink.push_back(mNote - 5);
    mLinkPatterns->push_back(mLink);

    mLink.clear(); 
    mLink.push_back(mNote); mLink.push_back(mNote - 5); mLink.push_back(mNote - 9); mLink.push_back(mNote - 10);
    mLinkPatterns->push_back(mLink);

    // dominant
    mLink.clear(); 
    mLink.push_back(mNote); mLink.push_back(mNote - 2); mLink.push_back(mNote - 5); mLink.push_back(mNote - 8);
    mLinkPatterns->push_back(mLink);

    // mixolydian / dorian
    mLink.clear(); 
    mLink.push_back(mNote); mLink.push_back(mNote - 2); mLink.push_back(mNote - 3); mLink.push_back(mNote - 5);
    mLinkPatterns->push_back(mLink);

    mLink.clear(); 
    mLink.push_back(mNote); mLink.push_back(mNote + 12); mLink.push_back(mNote + 10); mLink.push_back(mNote + 7);
    mLinkPatterns->push_back(mLink);

    mLink.clear(); 
    mLink.push_back(mNote); mLink.push_back(mNote + 7); mLink.push_back(mNote + 12); mLink.push_back(mNote + 10);
    mLinkPatterns->push_back(mLink);

    mLink.clear(); 
    mLink.push_back(mNote); mLink.push_back(mNote - 3); mLink.push_back(mNote - 2); mLink.push_back(mNote - 1);
    mLinkPatterns->push_back(mLink);

    // phrygian
    mLink.clear(); 
    mLink.push_back(mNote); mLink.push_back(mNote + 7); mLink.push_back(mNote + 13); mLink.push_back(mNote + 12);
    mLinkPatterns->push_back(mLink);

    mLink.clear(); 
    mLink.push_back(mNote); mLink.push_back(mNote + 7); mLink.push_back(mNote + 10); mLink.push_back(mNote + 13);
    mLinkPatterns->push_back(mLink);

    // locrian
    mLink.clear(); 
    mLink.push_back(mNote); mLink.push_back(mNote + 3); mLink.push_back(mNote + 6); mLink.push_back(mNote + 10);
    mLinkPatterns->push_back(mLink);

    mLink.clear(); 
    mLink.push_back(mNote); mLink.push_back(mNote + 3); mLink.push_back(mNote + 5); mLink.push_back(mNote + 6);
    mLinkPatterns->push_back(mLink);

    // phrygian / locrian
    mLink.clear(); 
    mLink.push_back(mNote); mLink.push_back(mNote + 3); mLink.push_back(mNote + 1); mLink.push_back(mNote);
    mLinkPatterns->push_back(mLink);

    // altered
    mLink.clear(); 
    mLink.push_back(mNote); mLink.push_back(mNote + 1); mLink.push_back(mNote + 3); mLink.push_back(mNote + 4);
    mLinkPatterns->push_back(mLink);

    mLink.clear(); 
    mLink.push_back(mNote); mLink.push_back(mNote - 2); mLink.push_back(mNote - 4); mLink.push_back(mNote - 6);
    mLinkPatterns->push_back(mLink);

    // chromatic
    mLink.clear(); 
    mLink.push_back(mNote); mLink.push_back(mNote + 2); mLink.push_back(mNote + 3); mLink.push_back(mNote + 4);
    mLinkPatterns->push_back(mLink);
}

void MCWalkingBassImproviser::improvise(MCScore* Score, int MeasureFrom, int MeasureTo)
{
    MCScore mScore;

    if(mHarmonyPattern == NULL || mScalePattern == NULL || mHarmonyPattern->getNumberOfEntries() == 0 || 
       mScalePattern->getNumberOfEntries() == 0)
    {
        return;
    }
    
    MSScoreEntry se;
    MSHarmonyPatternEntry* hpe;
    MSScalePatternEntry* spe;

    unsigned int iHarmonyPatternPosition;
    unsigned int iScalePatternPosition;
    unsigned int iScorePosition;

    MTNoteMap mBassNoteMap;
    MTNoteMap mChordMap;
    MTNoteMap mScaleMap;

    MTNoteMap mFirstNotes;
    unsigned int iFirstNoteIndex;

    MTNoteMapList mLinkPatterns;
    MTNoteMap mTestPattern;
    MTNoteMap mCurrentChord;
    MTNoteMapList mLinkSuitablePatterns;
    std::vector<unsigned int> iLinkSuitablePatternsScore;
    MTNoteMap mChosenLinkPattern;

    unsigned int i;
    
    mTimePosition.Beat = 1;
    mTimePosition.Tick = 0;

    se.Note.Mode = 0;
    se.Note.Duration = 0;
    se.Note.Channel = 0;
    se.Note.MDA = 0;
    se.Note.MDB = 0;
    se.Note.MDC = 0;

    // find the first chord in the harmony pattern
    iHarmonyPatternPosition = 0;
    
    while(iHarmonyPatternPosition < mHarmonyPattern->getNumberOfEntries() && 
          (mHarmonyPattern->getEntry(iHarmonyPatternPosition)->TimePosition.Measure < MeasureFrom))
    {
        iHarmonyPatternPosition++;
    }
    
    if(iHarmonyPatternPosition == mHarmonyPattern->getNumberOfEntries())
        return;

    // and find the first scale in the scale pattern
    iScalePatternPosition = 0;

    while(iScalePatternPosition < mScalePattern->getNumberOfEntries() &&
          (mScalePattern->getEntry(iScalePatternPosition)->TimePosition.Measure < MeasureFrom))
    {
        iScalePatternPosition++;
    }

    if(iScalePatternPosition == mScalePattern->getNumberOfEntries())
        return;

    // fill all the measures with quarter notes
    for(mTimePosition.Measure = MeasureFrom; mTimePosition.Measure <= MeasureTo; mTimePosition.Measure++)
    {
        se.Note.Intensity = iIntensity;

        // update harmony
        while(((iHarmonyPatternPosition + 1) < mHarmonyPattern->getNumberOfEntries()) && 
              (mHarmonyPattern->getEntry(iHarmonyPatternPosition + 1)->TimePosition <= mTimePosition))
        {
            iHarmonyPatternPosition++;
        }

        hpe = mHarmonyPattern->getEntry(iHarmonyPatternPosition);

        // generate a notemap only with the bass note from the current chord
        mChordMap = MCNoteMaps::createNoteMap(hpe->RootNote, hpe->Chord, mRange);
        mBassNoteMap = MCNoteMaps::extractNoteEntries(hpe->BassNote, mChordMap);

        // choose the first note
        iFirstNoteIndex = MCNoteMaps::getPositionNearestNote(mPreviousNote, mBassNoteMap);

        // the line tends to go high naturally
        if(iFirstNoteIndex > 0 && eventOccurs(75))
            iFirstNoteIndex--;

        se.TimePosition = mTimePosition;
        se.Note.Pitch = mBassNoteMap[iFirstNoteIndex];
        mFirstNotes.push_back(se.Note.Pitch);
        mScore.addEntry(se);

        // update scale
        while(((iScalePatternPosition + 1) < mScalePattern->getNumberOfEntries()) &&
              (mScalePattern->getEntry(iScalePatternPosition + 1)->TimePosition <= mTimePosition))
        {
            iScalePatternPosition++;
        }

        spe = mScalePattern->getEntry(iScalePatternPosition);

        // generate again a notemap from the scale
        mScaleMap = MCNoteMaps::createNoteMap(spe->RootNote, spe->Scale, mRange);

        // fill a list of linking possibilities
        fillLinkPatterns(&mLinkPatterns, mFirstNotes[mTimePosition.Measure - MeasureFrom]);

        mCurrentChord = hpe->Chord;

        for(i = 0; i < mCurrentChord.size(); i++)
            mCurrentChord[i] += mFirstNotes[mTimePosition.Measure - MeasureFrom];

        mLinkSuitablePatterns.clear();
        mLinkSuitablePatterns.push_back(mCurrentChord);

        // patterns with all the notes on the current scale
        for(i = 0; i < mLinkPatterns.size(); i++)
        {
            if(MCNoteMaps::contains(mScaleMap, mLinkPatterns[i]))
                mLinkSuitablePatterns.push_back(mLinkPatterns[i]);
        }

        // choose one of them
        mChosenLinkPattern = mLinkSuitablePatterns[rand() % mLinkSuitablePatterns.size()];

        for(mTimePosition.Beat = 2; mTimePosition.Beat <= iBeatsPerMeasure; mTimePosition.Beat++)
        {
            se.TimePosition = mTimePosition;

            if(mChosenLinkPattern.size() >= mTimePosition.Beat)
            {
                se.Note.Pitch = mChosenLinkPattern[mTimePosition.Beat - 1];

                if(se.Note.Pitch > mRange.HighestNote)
                    se.Note.Pitch -= 12;
                else if(se.Note.Pitch < mRange.LowestNote)
                    se.Note.Pitch += 12;

                mPreviousNote = se.Note.Pitch;

                mScore.addEntry(se);
            }
        }

        mTimePosition.Beat = 1;
    }

    // now check for measures in which harmony changes on beat 3
    iScorePosition = 0;
    
    for(iHarmonyPatternPosition = 0; iHarmonyPatternPosition < mHarmonyPattern->getNumberOfEntries(); iHarmonyPatternPosition++)
    {
        hpe = mHarmonyPattern->getEntry(iHarmonyPatternPosition);

        if(hpe->TimePosition.Measure > MeasureTo)
            break;

        while(mScore.getEntry(iScorePosition)->TimePosition < hpe->TimePosition)
            iScorePosition++;

        if(hpe->TimePosition.Beat == 3)
        {
            // pitch for beat 3
            mChordMap = MCNoteMaps::createNoteMap(hpe->RootNote, hpe->Chord, mRange);
            mBassNoteMap = MCNoteMaps::extractNoteEntries(hpe->BassNote, mChordMap);
            MTNote mNoteBeat2 = mScore.getEntry(iScorePosition - 1)->Note.Pitch;
            mScore.getEntry(iScorePosition)->Note.Pitch = mBassNoteMap[MCNoteMaps::getPositionNearestNote(mNoteBeat2, mBassNoteMap)];

            // pitch for beat 4
            mScore.getEntry(iScorePosition + 1)->Note.Pitch = mScore.getEntry(iScorePosition)->Note.Pitch + hpe->Chord[1];

            if(mScore.getEntry(iScorePosition + 1)->Note.Pitch > mRange.HighestNote)
                mScore.getEntry(iScorePosition + 1)->Note.Pitch -= 12;
        }
    }

    // once we have the complete walking bass line, put some chromatic passings to make it more elegant
    for(i = 4; i < mScore.getNumberOfEntries(); i += 4)
    {
        // towards up
        if(mScore.getEntry(i - 2)->Note.Pitch == (mScore.getEntry(i)->Note.Pitch - 2))
        {
            mScore.getEntry(i - 1)->Note.Pitch = mScore.getEntry(i)->Note.Pitch - 1;
        }

        // towards down
        if(mScore.getEntry(i - 2)->Note.Pitch == (mScore.getEntry(i)->Note.Pitch + 2))
        {
            mScore.getEntry(i - 1)->Note.Pitch = mScore.getEntry(i)->Note.Pitch + 1;
        }
    }

    Score->addScore(mScore);
}
