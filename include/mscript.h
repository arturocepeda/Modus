
////////////////////////////////////////////////////////////////////////
//
//  Modus v0.54
//  C++ Music Library
//
//  Copyright (c) 2012-2014 Arturo Cepeda Pérez
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

#ifndef _MSCRIPT_H_
#define _MSCRIPT_H_

#include "mconstants.h"
#include "mtypes.h"

/**
 *  @brief Static class for assistance on writing scripts for score objects
 */
class MCScript
{
public:
    /**
     *  @brief Write beats per measure
     *  @param String Pointer to the string where the commands will be written
     *  @param BeatsPerMeasure Number of beats per measure
     */
    static void writeBMP(char* String, unsigned int BeatsPerMeasure);

    /**
     *  @brief Write a time position
     *  @param String Pointer to the string where the commands will be written
     *  @param TimePosition Time position
     */
    static void writeTimePosition(char* String, MSTimePosition& TimePosition);
    /**
     *  @brief Write a time position
     *  @param String Pointer to the string where the commands will be written
     *  @param Measure Time position: measure
     *  @param Beat Time position: beat (optional)
     *  @param Tick Time position: tick (optional)
     */
    static void writeTimePosition(char* String, unsigned int Measure, unsigned int Beat = 1, unsigned int Tick = 0);
    /**
     *  @brief Write the beat from a time position
     *  @param String Pointer to the string where the commands will be written
     *  @param Beat Time position: beat
     */
    static void writeCurrentBeat(char* String, unsigned int Beat);
    /**
     *  @brief Write the tick from a time position
     *  @param String Pointer to the string where the commands will be written
     *  @param Tick Time position: tick
     */
    static void writeCurrentTick(char* String, unsigned int Tick);
    /**
     *  @brief Write the beat and the tick from a time position
     *  @param String Pointer to the string where the commands will be written
     *  @param Beat Time position: beat
     *  @param Tick Time position: tick
     */
    static void writeCurrentBeatAndTick(char* String, unsigned int Beat, unsigned int Tick);

    /**
     *  @brief Write a note
     *  @param String Pointer to the string where the commands will be written
     *  @param Note Definition of the note
     *  @param AddDuration If true, an add duration command (+) will be added
     */
    static void writeNote(char* String, MSNote& Note, bool AddDuration = false);
    /**
     *  @brief Write a note
     *  @param String Pointer to the string where the commands will be written
     *  @param Pitch Pitch of the note
     *  @param Intensity Intensity of the note
     *  @param Mode Mode of the note
     *  @param Channel Channel through which the note will be played
     *  @param Duration Duration of the note
     *  @param AddDuration If true, an add duration command (+) will be added
     */
    static void writeNote(char* String, MTNote Pitch, unsigned char Intensity, unsigned char Mode,
                          unsigned char Channel, unsigned char Duration, bool AddDuration = false);
    /**
     *  @brief Write a rest
     *  @param String Pointer to the string where the commands will be written
     *  @param Channel Channel which will be silenced
     *  @param Duration Duration of the rest
     *  @param AddDuration If true, an add duration command (+) will be added
     */
    static void writeRest(char* String, unsigned char Channel, unsigned char Duration, bool AddDuration = false);

    /**
     *  @brief Write a chord
     *  @param String Pointer to the string where the commands will be written
     *  @param PitchList Note map with all the chord's pitches
     *  @param Intensity Intensity of the chord's notes
     *  @param Mode Mode of the chord's notes
     *  @param FirstChannel Channel for the lowest note. Consecutive values will be assigned to the rest of the notes
     *  @param Duration Duration of the chord
     *  @param TicksBetweenNotes If greater than 0, the chord will be an arpeggio
     *  @param TimePosition Time position of the chord or the arpeggio's first note
     *  @param BeatsPerMeasure Number of beats per measure to calculate time positions
     */
    static void writeChord(char* String, MTNoteMap& PitchList, unsigned char Intensity, unsigned char Mode,
                           unsigned char FirstChannel, unsigned char Duration, unsigned char TicksBetweenNotes,
                           MSTimePosition& TimePosition, unsigned char BeatsPerMeasure);
    /**
     *  @brief Write a scale
     *  @param String Pointer to the string where the commands will be written
     *  @param PitchList Note map with all the scale's pitches
     *  @param Intensity Intensity of the scale's notes
     *  @param Mode Mode of the scale's notes
     *  @param Channel Channel for the all the scale's notes
     *  @param DurationOfEachNote Duration of each note of the scale
     *  @param TimePosition Time position of the scale's first note
     *  @param BeatsPerMeasure Number of beats per measure to calculate time positions
     */
    static void writeScale(char* String, MTNoteMap& PitchList, unsigned char Intensity, unsigned char Mode,
                           unsigned char Channel, unsigned char DurationOfEachNote, 
                           MSTimePosition& TimePosition, unsigned char BeatsPerMeasure);
};

#endif
