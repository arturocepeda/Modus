
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

///\file

#ifndef _CHORDS_H_
#define _CHORDS_H_

#include "mtypes.h"

namespace Modus
{
    /**
    *  @brief Static class to work with chords
    */
    class MCChords
    {
    public:
        /**
        *  @brief Default chords definition string
        */
        static const char* ChordsDefinition;

        /**
        *  @brief Check whether two chords are the same
        *  @param Chord1 First chord to be compared
        *  @param Chord2 Second chord to be compared
        */
        static bool equal(const MTChord& Chord1, const MTChord& Chord2);

        static MTChord cM();
        static MTChord cM6();
        static MTChord cM7();
        static MTChord cMaug5();

        static MTChord cm();
        static MTChord cm7();
        static MTChord cm7b5();
        static MTChord cm6();
        static MTChord cmM7();

        static MTChord c7();
        static MTChord c9();
        static MTChord c7m9();

        static MTChord cdis();
        static MTChord cdis7();

        static MTChord csus4();
        static MTChord c7sus4();
        static MTChord c7b5();

        /**
        *  @brief Creates a note map with a closed chord from a root note and a chord pattern
        *  @param RootNote Root note of the chord
        *  @param Chord Chord pattern, where 0 will be the root note
        *  @param LowestNote The chord will be created from the first note equal or higher than this pitch value
        *  @param NumberOfNotes Size of the resulting note map
        *  @return Note map with the closed chord
        */
        static MTNoteMap createClosedChord(MTNote RootNote, MTChord& Chord, MTNote LowestNote, 
                                            unsigned char NumberOfNotes);
        /**
        *  @brief Creates a note map with an open chord from a root note and a chord pattern
        *  @param RootNote Root note of the chord
        *  @param Chord Chord pattern, where 0 will be the root note
        *  @param LowestNote The chord will be created from the first note equal or higher than this pitch value
        *  @param NumberOfNotes Size of the resulting note map
        *  @return Note map with the open chord
        */
        static MTNoteMap createOpenChord(MTNote RootNote, MTChord& Chord, MTNote LowestNote, 
                                        unsigned char NumberOfNotes);

        /**
        *  @brief Creates a note map with a closed voicing from a root note and a chord pattern
        *  @param RootNote Root note of the chord
        *  @param Chord Chord pattern, where 0 will be the root note
        *  @param BassNote Bass note, which may be different than the root note (ex: Cm7/G)
        *  @param Tensions Notes to be added to the basic chord
        *  @param LowestNote The chord will be created from the first note equal or higher than this pitch value
        *  @param RemoveBassNote Indicates whether the bass note must be included in the voicing or not
        *  @param RemovePerfectFifth Indicates whether the perfect fifth must be included in the voicing or not
        *  @return Note map with the closed voicing
        */
        static MTNoteMap createClosedVoicing(MTNote RootNote, MTChord& Chord, MTNote BassNote, MTChord& Tensions, 
                                            MTNote LowestNote, bool RemoveBassNote, bool RemovePerfectFifth);
        /**
        *  @brief Creates a note map with a open voicing from a root note and a chord pattern
        *  @param RootNote Root note of the chord
        *  @param Chord Chord pattern, where 0 will be the root note
        *  @param BassNote Bass note, which may be different than the root note (ex: Cm7/G)
        *  @param Tensions Notes to be added to the basic chord
        *  @param LowestNote The chord will be created from the first note equal or higher than this pitch value
        *  @param RemoveBassNote Indicates whether the bass note must be included in the voicing or not
        *  @param RemovePerfectFifth Indicates whether the perfect fifth must be included in the voicing or not
        *  @return Note map with the open voicing
        */
        static MTNoteMap createOpenVoicing(MTNote RootNote, MTChord& Chord, MTNote BassNote, MTChord& Tensions, 
                                            MTNote LowestNote, bool RemoveBassNote, bool RemovePerfectFifth);
    };
}

#endif
