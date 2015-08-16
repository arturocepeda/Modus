
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

#ifndef _MLIST_H_
#define _MLIST_H_

#include "mtypes.h"
#include "msettings.h"
#include <cstdlib>

#define MODUS_LINE_BUFFER 256

namespace Modus
{
    /**
    *  @brief Abstract class which represents a list of entries
    */
    template<typename EntryType>
    class MCList
    {
    protected:
        std::vector<EntryType> sEntry;

        int readScriptValue(char* sLine, const char* sField)
        {
            char* sSegment = strstr(sLine, sField);

            if(sSegment == NULL)
                return -1;

            // avoid the first characters
            while(sSegment[0] != '\0' && !isdigit(sSegment[0]))
                sSegment++;

            if(sSegment[0] == '\0')
                return -1;
    
            int i = 0;
            const int iValueSize = 16;
            char sValue[iValueSize];

            while(i < (iValueSize - 1) && sSegment[i] != '\0' && isdigit(sSegment[0]))
            {
                sValue[i] = sSegment[i];
                i++;
            }

            sValue[i] = 0;

            return atoi(sValue);
        }

    public:
        /**
        *  @brief Add a new entry
        *  @param Entry Entry to be added to the list
        */
        virtual void addEntry(const EntryType& Entry) = 0;
        /**
        *  @brief Remove the entry in the specified position
        *  @param Position Position of the entry to be removed from the list
        */
        void removeEntry(unsigned int Position)
        {
            if(Position < sEntry.size())
                sEntry.erase(sEntry.begin() + Position);
        }
        /**
        *  @brief Remove all entries
        */
        void clear()
        {
            sEntry.clear();
        }

        /**
        *  @brief Returns a pointer to the entry in the specified position
        *  @param Position Position of the entry
        */
        EntryType* operator[](unsigned int Position)
        {
            if(Position < sEntry.size())
                return &sEntry[Position];
            else
                return NULL;
        }
        /**
        *  @brief Returns a pointer to the entry in the specified position
        *  @param Position Position of the entry
        */
        EntryType* getEntry(unsigned int Position)
        {
            return this->operator[](Position);
        }
        /**
        *  @brief Returns a pointer to the first entry
        */
        EntryType* getFirstEntry()
        {
            return getEntry(0);
        }
        /**
        *  @brief Returns a pointer to the last entry
        */
        EntryType* getLastEntry()
        {
            return getEntry(sEntry.size() - 1);
        }
        /**
        *  @brief Returns the number of entries
        */
        unsigned int getNumberOfEntries()
        {
            return sEntry.size();
        }
    };


    /**
    *  @brief Abstract class which represents a list of entries with complete time positions and 
    *         information about notes and time
    */
    template<typename EntryType>
    class MCListMusic : public MCList<EntryType>
    {
    protected:
        unsigned int iBeatsPerMeasure;

        MCListMusic()
            : iBeatsPerMeasure(4)
        {
        }

        MTNoteMap readScriptNoteList(char* sList)
        {
            MTNoteMap mNoteMap;
            char sValue[16];
            unsigned int iValuePos;
            int iValue;

            do
            {
                while(!isdigit(sList[0]) && sList[0] != '\0')
                    sList++;

                iValuePos = 0;

                while(isdigit(sList[0]))
                {
                    sValue[iValuePos++] = sList[0];
                    sList++;
                }

                if(sList[0] != '\0')
                {
                    sValue[iValuePos] = '\0';

                    // here we have the next note of the scale
                    iValue = atoi(sValue);
                    mNoteMap.push_back(iValue);
                }

            } while(sList[0] != '\0');

            return mNoteMap;
        }

    public:
        /**
        *  @brief Displace all the entries by the specified number of measures
        *  @param NumberOfMeasures Number of measures to add or subtract (it can be negative)
        */
        void displace(int NumberOfMeasures)
        {
            for(unsigned int i = 0; i < this->sEntry.size(); i++)
                this->sEntry[i].TimePosition.Measure += NumberOfMeasures;
        }
        /**
        *  @brief Displace all the entries by the specified time
        *  @param NumberOfMeasures Number of measures to add or subtract (it can be negative)
        *  @param NumberOfBeats Number of beats to add or subtract (it can be negative)
        *  @param NumberOfTicks Number of ticks to add or subtract (it can be negative)
        *  @param BeatsPerMeasure Number of beats per measure to calculate time positions
        */
        void displace(int NumberOfMeasures, int NumberOfBeats, int NumberOfTicks, unsigned int BeatsPerMeasure)
        {
            if(NumberOfMeasures != 0)
                displace(NumberOfMeasures);

            for(unsigned int i = 0; i < this->sEntry.size(); i++)
                this->sEntry[i].TimePosition.add((NumberOfBeats * MCSettings::getTicksPerBeat()) + NumberOfTicks, BeatsPerMeasure);
        }
        /**
        *  @brief Transpose the note(s) in all the entries by the specified number of semitones
        *  @param NumberOfSemitones Number of semitones to add or subtract (it can be negative)
        */
        virtual void transpose(char NumberOfSemitones) = 0;

        /**
        *  @brief Returns the number of beats per measure
        *  @return Beats per measure
        */
        unsigned int getBeatsPerMeasure()
        {
            return iBeatsPerMeasure;
        }
    };
}

#endif
