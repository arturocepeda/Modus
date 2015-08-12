
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

#ifndef _MSTRINGINSTRUMENT_H_
#define _MSTRINGINSTRUMENT_H_

#include "mtypes.h"
#include "minstrument.h"

namespace Modus
{
    /**
     *  @brief Represents a string instrument
     */
    class MCStringInstrument : public MCInstrument
    {
    protected:
        MTNote* mStrings;

    public:
        /**
         *  @brief Constructor
         */
        MCStringInstrument(unsigned int ID, unsigned char NumberOfStrings);
        ~MCStringInstrument();

        /**
         *  @brief Set the nth string open note
         *  @param StringNumber String index starting from 1 (1-n)
         *  @param Note Note that sounds when the open string is played
         */
        void setOpenStringNote(unsigned char StringNumber, MTNote Note);
    };

    /**
     *  @brief Represents a fretted instrument
     */
    class MCFrettedInstrument : public MCStringInstrument
    {
    protected:
        unsigned char iNumberOfFrets;

    public:
        /**
         *  @brief Constructor
         */
        MCFrettedInstrument(unsigned int ID, unsigned char NumberOfStrings, unsigned char NumberOfFrets);
        ~MCFrettedInstrument();

        /**
         *  @brief Get the fret number corresponding to a note on a string
         *  @param Note Desired note
         *  @param String String index starting from 1 (1-n)	 
         *  @return Fret number
         */
        char getFret(MTNote Note, unsigned char String);
    };
}

#endif
