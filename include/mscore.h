
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

#ifndef _MSCORE_H_
#define _MSCORE_H_

#include "mlist.h"
#include "mtypes.h"

/**
 *  @brief Represents a score, which consist of a list of notes placed on time positions
 */
class MCScore : public MCListMusic<MSScoreEntry>
{
private:
    void loadScript(std::istream* sScript);
    
public:
    /**
     *  @brief Constructor
     */
    MCScore();
    ~MCScore();

    /**
     *  @brief Add a new entry
     *  @param ScoreEntry Entry to be added to the list
     */
    void addEntry(const MSScoreEntry& ScoreEntry);
    /**
     *  @brief Merge the current entries with another score's entries
     *  @param Score Score to merge with this score
     */
    void addScore(MCScore& Score);

    /**
     *  @brief Transpose the pitch in all the notes by the specified number of semitones
     *  @param NumberOfSemitones Number of semitones to add or subtract (it can be negative)
     */
    void transpose(char NumberOfSemitones);
    /**
     *  @brief Change the intensity in all the notes by the specified increment
     *  @param Increment Quantity of the increment (it can be negative)
     */
    void changeIntensity(char Increment);

    /**
     *  @brief Load script with information to fill entries from a string
     *  @param Script String which contains the script
     */
    void loadScriptFromString(const char* Script);
    /**
     *  @brief Load script with information to fill entries from a file
     *  @param Filename Text file which contains the script
     *  @return Whether the file could be readed or not
     */
    bool loadScriptFromFile(const char* Filename);
    /**
     *  @brief Load script with information to fill entries from a file
     *  @param Stream Input file stream attached to the binary data
     */
    void loadScriptFromBinaryData(std::istream& Stream);
    /**
     *  @brief Load script with information to fill entries from a file
     *  @param Filename Binary file which contains the score data
     *  @return Whether the file could be readed or not
     */
    bool loadScriptFromBinaryFile(const char* Filename);
};

#endif
