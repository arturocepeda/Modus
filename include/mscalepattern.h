
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

#include "mlist.h"
#include "mtypes.h"

#ifndef _MSCALEPATTERN_H_
#define _MSCALEPATTERN_H_

/**
 *  @brief Represents a scale pattern, which consist of a list of scales placed on time positions
 */
class MCScalePattern : public MCListMusic<MSScalePatternEntry>
{
private:
    void loadScript(std::istream* sScalesDefinition, std::istream* sScript);

public:
    /**
     *  @brief Constructor
     */
    MCScalePattern();
    ~MCScalePattern();
    
    /**
     *  @brief Add a new entry
     *  @param ScalePatternEntry Entry to be added to the list
     */
    void addEntry(const MSScalePatternEntry& ScalePatternEntry);

    /**
     *  @brief Transpose the root note in all the entries by the specified number of semitones
     *  @param NumberOfSemitones Number of semitones to add or subtract (it can be negative)
     */
    void transpose(char NumberOfSemitones);

    /**
     *  @brief Load script with information to fill entries from a string
     *  @param ScalesDefinition String in which all the scale patterns are defined. Each scale pattern
     *                          will have a unique ID number to be referenced from the script
     *  @param Script String which contains the script
     */
    void loadScriptFromString(const char* ScalesDefinition, const char* Script);
    /**
     *  @brief Load script with information to fill entries from a string
     *  @param Script String which contains the script
     */
    void loadScriptFromString(const char* Script);
    /**
     *  @brief Load script with information to fill entries from a file
     *  @param ScalesDefinitionFilename Text file in which all the scale patterns are defined. Each scale pattern
     *                                  will have a unique ID number to be referenced from the script
     *  @param ScriptFilename Text file which contains the script
     *  @return Whether the file could be readed or not
     */
    bool loadScriptFromFile(const char* ScalesDefinitionFilename, const char* ScriptFilename);
    /**
     *  @brief Load script with information to fill entries from a file
     *  @param ScriptFilename Text file which contains the script
     *  @return Whether the file could be readed or not
     */
    bool loadScriptFromFile(const char* ScriptFilename);
};

#endif
