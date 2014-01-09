
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

#include "mscales.h"

const char* MCScales::ScalesDefinition = "0: 0,2,4,5,7,9,11 \n"                 // Major
                                         "1: 0,2,3,5,7,8,10 \n"                 // Minor
                                         "2: 0,2,3,5,7,8,11 \n"                 // Minor Harmonic
                                         "3: 0,2,3,5,7,9,11 \n"                 // Minor Melodic
                                         "4: 0,2,4,5,7,8,10 \n"                 // Major-Minor

                                         "5: 0,2,3,5,7,9,10 \n"                 // Dorian
                                         "6: 0,1,3,5,7,8,10 \n"                 // Phrygian
                                         "7: 0,2,4,6,7,9,11 \n"                 // Lydian
                                         "8: 0,2,4,5,7,9,10 \n"                 // Mixolydian
                                         "9: 0,1,3,5,6,8,10 \n"                 // Locrian

                                         "10: 0,2,4,6,7,9,10 \n"                // Acoustic
                                         "11: 0,1,3,4,6,8,10 \n"                // Altered

                                         "12: 0,2,4,7,9 \n"                     // Pentatonic Major
                                         "13: 0,3,5,7,10 \n"                    // Pentatonic Minor
                                         "14: 0,3,5,6,7,10 \n"                  // Blues
                                         "15: 0,3,5,6,7,9,10 \n"                // Blues + Sixth

                                         "16: 0,1,4,5,7,8,10 \n"                // Gypsy
                                         "17: 0,1,4,5,7,8,11 \n"                // Double Harmonic

                                         "18: 0,2,4,6,8,10 \n"                  // Whole Tone
                                         "19: 0,2,3,5,6,8,9,11 \n"              // Octatonic (H-W)
                                         "20: 0,1,3,4,6,7,9,10 \n"              // Octatonic (W-H)
                                         "21: 0,1,2,3,4,5,6,7,8,9,10,11 \n";    // Chromatic

bool MCScales::equal(const MTScale& Scale1, const MTScale& Scale2)
{
    return std::equal(Scale1.begin(), Scale1.end(), Scale2.begin());
}

MTScale MCScales::Major()
{
    MTScale mScale;
    MTNote mNoteSet[] = {C, D, E, F, G, A, B};
    mScale.assign(mNoteSet, mNoteSet + 7);
    return mScale;
}

MTScale MCScales::Minor()
{
    MTScale mScale;
    MTNote mNoteSet[] = {C, D, Ef, F, G, Af, Bf};
    mScale.assign(mNoteSet, mNoteSet + 7);
    return mScale;
}

MTScale MCScales::MinorHarmonic()
{
    MTScale mScale;
    MTNote mNoteSet[] = {C, D, Ef, F, G, Af, B};
    mScale.assign(mNoteSet, mNoteSet + 7);
    return mScale;
}

MTScale MCScales::MinorMelodic()
{
    MTScale mScale;
    MTNote mNoteSet[] = {C, D, Ef, F, G, A, B};
    mScale.assign(mNoteSet, mNoteSet + 7);
    return mScale;
}

MTScale MCScales::MajorMinor()
{
    MTScale mScale;
    MTNote mNoteSet[] = {C, D, E, F, G, Af, Bf};
    mScale.assign(mNoteSet, mNoteSet + 7);
    return mScale;
}

MTScale MCScales::Dorian()
{
    MTScale mScale;
    MTNote mNoteSet[] = {C, D, Ef, F, G, A, Bf};
    mScale.assign(mNoteSet, mNoteSet + 7);
    return mScale;
}

MTScale MCScales::Phrygian()
{
    MTScale mScale;
    MTNote mNoteSet[] = {C, Df, Ef, F, G, Af, Bf};
    mScale.assign(mNoteSet, mNoteSet + 7);
    return mScale;
}

MTScale MCScales::Lydian()
{
    MTScale mScale;
    MTNote mNoteSet[] = {C, D, E, Fs, G, A, B};
    mScale.assign(mNoteSet, mNoteSet + 7);
    return mScale;
}

MTScale MCScales::Mixolydian()
{
    MTScale mScale;
    MTNote mNoteSet[] = {C, D, E, F, G, A, Bf};
    mScale.assign(mNoteSet, mNoteSet + 7);
    return mScale;
}

MTScale MCScales::Locrian()
{
    MTScale mScale;
    MTNote mNoteSet[] = {C, Df, Ef, F, Gf, Af, Bf};
    mScale.assign(mNoteSet, mNoteSet + 7);
    return mScale;
}

MTScale MCScales::Acoustic()
{
    MTScale mScale;
    MTNote mNoteSet[] = {C, D, E, Fs, G, A, Bf};
    mScale.assign(mNoteSet, mNoteSet + 7);
    return mScale;
}

MTScale MCScales::Altered()
{
    MTScale mScale;
    MTNote mNoteSet[] = {C, Df, Ef, Ff, Gf, Af, Bf};
    mScale.assign(mNoteSet, mNoteSet + 7);
    return mScale;
}

MTScale MCScales::PentatonicMajor()
{
    MTScale mScale;
    MTNote mNoteSet[] = {C, D, E, G, A};
    mScale.assign(mNoteSet, mNoteSet + 5);
    return mScale;
}

MTScale MCScales::PentatonicMinor()
{
    MTScale mScale;
    MTNote mNoteSet[] = {C, Ef, F, G, Bf};
    mScale.assign(mNoteSet, mNoteSet + 5);
    return mScale;
}

MTScale MCScales::Blues()
{
    MTScale mScale;
    MTNote mNoteSet[] = {C, Ef, F, Fs, G, Bf};
    mScale.assign(mNoteSet, mNoteSet + 6);
    return mScale;
}

MTScale MCScales::BluesWithSixth()
{
    MTScale mScale;
    MTNote mNoteSet[] = {C, Ef, F, Fs, G, A, Bf};
    mScale.assign(mNoteSet, mNoteSet + 7);
    return mScale;
}

MTScale MCScales::Gypsy()
{
    MTScale mScale;
    MTNote mNoteSet[] = {C, Df, E, F, G, Af, Bf};
    mScale.assign(mNoteSet, mNoteSet + 7);
    return mScale;
}

MTScale MCScales::DoubleHarmonic()
{
    MTScale mScale;
    MTNote mNoteSet[] = {C, Df, E, F, G, Af, B};
    mScale.assign(mNoteSet, mNoteSet + 7);
    return mScale;
}

MTScale MCScales::WholeTone()
{
    MTScale mScale;
    MTNote mNoteSet[] = {C, D, E, Fs, Gs, As};
    mScale.assign(mNoteSet, mNoteSet + 6);
    return mScale;
}

MTScale MCScales::OctatonicHW()
{
    MTScale mScale;
    MTNote mNoteSet[] = {C, Cs, Ds, E, Fs, G, A, Bf};
    mScale.assign(mNoteSet, mNoteSet + 8);
    return mScale;
}

MTScale MCScales::OctatonicWH()
{
    MTScale mScale;
    MTNote mNoteSet[] = {C, D, Ef, F, Fs, Gs, A, B};
    mScale.assign(mNoteSet, mNoteSet + 8);
    return mScale;
}

MTScale MCScales::Chromatic()
{
    MTScale mScale;
    MTNote mNoteSet[] = {C, Cs, D, Ds, E, F, Fs, G, Gs, A, As, B};
    mScale.assign(mNoteSet, mNoteSet + 12);
    return mScale;
}
