
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

#ifndef _SCALES_H_
#define _SCALES_H_

#include "mconstants.h"
#include "mtypes.h"

/**
 *  @brief Static class to work with scales
 */
class MCScales
{
public:
    /**
     *  @brief Default scales definition string
     */
    static const char* ScalesDefinition;

    /**
     *  @brief Check whether two scales are the same
     *  @param Scale1 First scale to be compared
     *  @param Scale2 Second scale to be compared
     */
    static bool equal(const MTScale& Scale1, const MTScale& Scale2);

    static MTScale Major();
    static MTScale Minor();
    static MTScale MinorHarmonic();
    static MTScale MinorMelodic();
    static MTScale MajorMinor();

    static MTScale Dorian();
    static MTScale Phrygian();
    static MTScale Lydian();
    static MTScale Mixolydian();
    static MTScale Locrian();
    static MTScale Acoustic();
    static MTScale Altered();

    static MTScale PentatonicMajor();
    static MTScale PentatonicMinor();
    static MTScale Blues();
    static MTScale BluesWithSixth();

    static MTScale Gypsy();
    static MTScale DoubleHarmonic();

    static MTScale WholeTone();
    static MTScale OctatonicHW();
    static MTScale OctatonicWH();
    static MTScale Chromatic();
};

#endif
