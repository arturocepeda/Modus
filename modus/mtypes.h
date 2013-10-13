
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

///\file

#ifndef _MTYPES_H_
#define _MTYPES_H_

#ifdef __linux__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#endif

#include <vector>
#include "mconstants.h"

/**
 * \typedef MTNote
 *  Note pitch (MIDI note value)
 */
typedef unsigned char MTNote;

 
/**
 * \typedef MTNoteMap
 *  Vector of notes
 */
typedef std::vector<MTNote> MTNoteMap;


/**
 * \typedef MTNoteMapList
 *  Vector of note maps
 */
typedef std::vector<MTNoteMap> MTNoteMapList;


/**
 * \typedef MTChord
 *  Pattern that represents a chord (no actual pitches)
 */
typedef MTNoteMap MTChord;


/**
 * \typedef MTChordList
 *  Vector of chord patterns
 */
typedef std::vector<MTChord> MTChordList;


/**
 * \typedef MTScale
 *  Pattern that represents a scale (no actual pitches)
 */
typedef MTNoteMap MTScale;


/**
 * \typedef MTScaleList
 *  Vector of scale patterns
 */
typedef std::vector<MTScale> MTScaleList;



/**
 *  @brief Defines a time position in the music (measure, beat and tick)
 *
 *  @param Measure Number of measure
 *  @param Beat Number of beat. A new beat comes each 24 ticks
 *  @param Tick Number of tick, which is the smallest unit of time in Modus
 */
struct MSTimePosition
{
    int Measure;
    unsigned int Beat;
    unsigned int Tick;

    MSTimePosition();
    MSTimePosition(int Measure);
    MSTimePosition(int Measure, unsigned int Beat);
    MSTimePosition(int Measure, unsigned int Beat, unsigned int Tick);

    bool operator == (const MSTimePosition& TimePosition) const;
    bool operator < (const MSTimePosition& TimePosition) const;
    bool operator > (const MSTimePosition& TimePosition) const;
    bool operator <= (const MSTimePosition& TimePosition) const;
    bool operator >= (const MSTimePosition& TimePosition) const;

    void add(int Ticks, unsigned int BeatsPerMeasure);
    int getInTicks(unsigned int BeatsPerMeasure) const;
    MSTimePosition getWithIncrement(int Ticks, unsigned int BeatsPerMeasure) const;
    unsigned int getDifferenceWith(const MSTimePosition& TimePosition, unsigned int BeatsPerMeasure) const;
};


/**
 *  @brief Definition of an instrument's range
 *
 *  @param LowestNote Lowest note the instrument can play
 *  @param HighestNote Highest note the instrument can play
 */
struct MSRange
{
    MTNote LowestNote;
    MTNote HighestNote;

    MSRange();
    MSRange(MTNote LowestNote, MTNote HighestNote);

    unsigned char getSize() const;
};


/**
 *  @brief Definition of a note to be played
 *
 *  @param Pitch Pitch of the note
 *  @param Mode Playing mode, utilized to be able to establish different ways of making an instrument sound
 *  @param Intensity MIDI velocity value (0-127). 0 means "note off"
 *  @param Channel Instrument's channel through which the note is going to be played
 *  @param Duration Duration in ticks (24 per beat). 0 means "infinite"
 *  @param MDA Metadata A
 *  @param MDB Metadata B
 *  @param MDC Metadata C
 */
struct MSNote
{
    MTNote Pitch;
    unsigned char Intensity;
    unsigned char Mode;
    unsigned char Channel;
    unsigned char Duration;
    unsigned char MDA;
    unsigned char MDB;
    unsigned char MDC;

    MSNote();
    MSNote(MTNote Pitch, unsigned char Intensity, unsigned char Mode, unsigned char Channel, unsigned char Duration);
    MSNote(MTNote Pitch, unsigned char Intensity, unsigned char Mode, unsigned char Channel, unsigned char Duration,
           unsigned char MDA, unsigned char MDB, unsigned char MDC);
};


/**
 *  @brief Section of a song
 *
 *  @param FirstMeasure Start measure
 *  @param LastMeasure End measure
 *  @param BeatsPerMeasure Beats per measure
 *  @param Tempo Tempo
 *  @param TargetTempo If different than tempo, it will be interpreted as accelerando/ritardando
 */
struct MSSongSection
{
    int FirstMeasure;
    int LastMeasure;
    unsigned int BeatsPerMeasure;
    float Tempo;
    float TargetTempo;
};


/**
 *  @brief Item in a score
 *
 *  @param TimePosition Moment in which the note must be played
 *  @param Note Note to be played
 */
struct MSScoreEntry
{
    MSTimePosition TimePosition;
    MSNote Note;
};


/**
 *  @brief Item in a scale pattern
 *
 *  @param TimePosition Time position
 *  @param RootNote Scale's root note
 *  @param Scale Scale pattern
 */
struct MSScalePatternEntry
{
    MSTimePosition TimePosition;
    MTNote RootNote;
    MTScale Scale;
};


/**
 *  @brief Item in a harmony pattern
 *
 *  @param TimePosition Time position
 *  @param RootNote Chord's root note
 *  @param Chord Chord pattern
 *  @param BassNote Chord's bass note. Sometimes the root note and the bass note are not the same (ex: Cm7 / G)
 *  @param Tensions Notes that can be played along with the chord
 */
struct MSHarmonyPatternEntry
{
    MSTimePosition TimePosition;
    MTNote RootNote;
    MTChord Chord;
    MTNote BassNote;
    MTChord Tensions;
};


/**
 *  @brief Set of samples for a sound generator
 *
 *  @param Path Location of the sound files (with final '/' or '\\')
 *  @param Format Extension of the sound files (without '.')
 *  @param ID Set identifier
 *  @param Mode Playing mode
 *  @param MinimumIntesity Interval of intensity: minimum value
 *  @param MaximumIntensity Interval of intensity: maximum value
 *  @param Range Interval of pitches
 */
struct MSSampleSet
{
    char Path[256];
    char Format[8];
    unsigned char ID;
    unsigned char Mode;
    unsigned char MinimumIntensity;
    unsigned char MaximumIntensity;
    MSRange Range;

    MSSampleSet();
};


/**
 *  @brief Point of synchronization between the timer and the music. It can be useful if you want
 *         to generate or play music along with a pre-recorded background
 *
 *  @param TimePosition Time position in which the timer must be set
 *  @param MustSynchronize Flag to determine whether it's necessary synchronize or not
*/ 
struct MSSyncPoint
{
    MSTimePosition TimePosition;
    bool MustSynchronize;
};


/**
 * \typedef MTSyncPointList
 *  Vector of sync points
 */
typedef std::vector<MSSyncPoint> MTSyncPointList;



/**
 *  @brief Static class with some methods to work with notes (MTNote)
 */
class MCNotes
{
public:
    /**
     *  @brief Check if a note pitch is natural or sharp/flat
     *  @param Note Pitch value to check
     */
    static bool isNatural(MTNote Note);
    /**
     *  @brief Given a note pitch, returns which note it is without octaves
     *  @param Note Pitch value to check
     */
    static MTNote whichNote(MTNote Note);
    /**
     *  @brief Given a note pitch, writes the note's name into a string
     *  @param Note Pitch value to check
     *  @param String Pointer to the string where the note's name will be written
     */
    static void whichNote(MTNote Note, char* String);
    /**
     *  @brief Given a note pitch, writes the note's name into a string. If the note is not natural,
     *         the name will be written with a flat
     *  @param Note Pitch value to check
     *  @param String Pointer to the string where the note's name will be written
     */
    static void whichNoteFlat(MTNote Note, char* String);
    /**
     *  @brief Given a note pitch, writes the note's name into a string. If the note is not natural,
     *         the name will be written with a sharp
     *  @param Note Pitch value to check
     *  @param String Pointer to the string where the note's name will be written
     */
    static void whichNoteSharp(MTNote Note, char* String);
    /**
     *  @brief Given a note name, returns which note it is
     *  @param String String to check
     */
    static MTNote fromString(const char* String);

    /**
     *  @brief Transpose the note pitch an octave higher
     *  @param Note Pointer to the pitch value to modify
     */
    static inline void octaveUp(MTNote* Note);
    /**
     *  @brief Transpose the note pitch an octave lower
     *  @param Note Pointer to the pitch value to modify
     */
    static inline void octaveDown(MTNote* Note);
};


/**
 *  @brief Static class with some methods to work with note maps (MTNoteMap)
 */
class MCNoteMaps
{
public:
    /**
     *  @brief creates a note map from notes which belong to a specific scale or chord inside a range. 
     *  For example, with the F-Major chord in the range [C3-C5] the note map would contain the following 
     *  notes: (C3, F3, A3, C4, F4, A4, C5)
     *
     *  @param RootNote Root note of the chord or scale (C#, F, Ab, ...)
     *  @param Scale Type of scale or chord that will be the pattern to create the note map. The static 
     *         classes MCScales and MCChords may be useful to specify this value
     *  @param Range Desired range to fill with notes
     */
    static MTNoteMap createNoteMap(MTNote RootNote, const MTScale& Scale, const MSRange& Range);
    /**
     *  @brief Creates a note map with all the occurrences of a particular note, that may be repeated in 
     *         different octaves, in a note map
     *
     *  @param Note Selected note (C#, F, Ab, ...)
     *  @param Notemap Note map which contains (or not) the selected note
     */
    static MTNoteMap extractNoteEntries(MTNote Note, const MTNoteMap& Notemap);
    /**
     *  @brief Creates a note map by combining two existing note maps
     *
     *  @param Notemap1 First note map to include
     *  @param Notemap2 Second note map to include
     */
    static MTNoteMap combineNoteMaps(const MTNoteMap& Notemap1, const MTNoteMap& Notemap2);
    /**
     *  @brief Sort a note map from lowest to highest pitch
     *  @param Notemap Pointer to the note map to sort
     */
    static void sortNoteMap(MTNoteMap* Notemap);
    /**
     *  @brief Transpose all the notes contained in a note map
     *  @param Notemap Pointer to the note map to transpose
     *  @param Semitones Number of semitones (it can be negative)
     */
    static void transposeNoteMap(MTNoteMap* Notemap, char Semitones);
    /**
     *  @brief Check if a note map contains a particular note
     *  @param Notemap Note map to look for the note
     *  @param Note Pitch value to find
     */
    static bool contains(const MTNoteMap& Notemap, MTNote Note);
    /**
     *  @brief Check if a note map contains a list of notes
     *  @param Notemap Note map to look for the notes
     *  @param Notes Note map with the list of pitch values to find
     */
    static bool contains(const MTNoteMap& Notemap, const MTNoteMap& Notes);
    /**
     *  @brief Given a pitch value, returns the position of the nearest note in a note map
     *  @param Note Reference pitch value
     *  @param Notemap Note map to look for the nearest note
     */
    static int getPositionNearestNote(MTNote Note, const MTNoteMap& Notemap);
};

#endif
