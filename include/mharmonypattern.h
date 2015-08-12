
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

#include "mlist.h"
#include "mtypes.h"

#ifndef _MHARMONYPATTERN_H_
#define _MHARMONYPATTERN_H_

namespace Modus
{
    /**
    *  @brief Represents a harmony pattern, which consist of a list of chords placed on time positions
    */
    class MCHarmonyPattern : public MCListMusic<MSHarmonyPatternEntry>
    {
    private:
        void loadScript(std::istream* sChordsDefinition, std::istream* sScript);

    public:
        /**
        *  @brief Constructor
        */
        MCHarmonyPattern();
        ~MCHarmonyPattern();

        /**
        *  @brief Add a new entry
        *  @param HarmonyPatternEntry Entry to be added to the list
        */
        void addEntry(const MSHarmonyPatternEntry& HarmonyPatternEntry);

        /**
        *  @brief Transpose the root and the bass note in all the entries by the specified number of semitones
        *  @param NumberOfSemitones Number of semitones to add or subtract (it can be negative)
        */
        void transpose(char NumberOfSemitones);

        /**
        *  @brief Load script with information to fill entries from a string
        *  @param ChordsDefinitionString String in which all the chord patterns are defined. Each chord pattern
        *                                will have a unique ID number to be referenced from the script
        *  @param Script String which contains the script
        */
        void loadScriptFromString(const char* ChordsDefinitionString, const char* Script);
        /**
        *  @brief Load script with information to fill entries from a string
        *  @param Script String which contains the script
        */
        void loadScriptFromString(const char* Script);
        /**
        *  @brief Load script with information to fill entries from a file
        *  @param ChordsDefinitionFilename Text file in which all the chord patterns are defined. Each chord pattern
        *                                  will have a unique ID number to be referenced from the script
        *  @param ScriptFilename Text file which contains the script
        *  @return Whether the file could be readed or not
        */
        bool loadScriptFromFile(const char* ChordsDefinitionFilename, const char* ScriptFilename);
        /**
        *  @brief Load script with information to fill entries from a file
        *  @param ScriptFilename Text file which contains the script
        *  @return Whether the file could be readed or not
        */
        bool loadScriptFromFile(const char* ScriptFilename);
        /**
        *  @brief Load script with information to fill entries from a file
        *  @param Stream Input file stream attached to the binary data
        */
        void loadScriptFromBinaryData(std::istream& Stream);
        /**
        *  @brief Load script with information to fill entries from a file
        *  @param Filename Binary file which contains the harmony pattern data
        *  @return Whether the file could be readed or not
        */
        bool loadScriptFromBinaryFile(const char* Filename);
    };
}

#endif
