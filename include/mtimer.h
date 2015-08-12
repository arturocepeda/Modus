
////////////////////////////////////////////////////////////////////////
//
//  Modus v0.60
//  C++ Music Library
//
//  Copyright (c) 2012-2015 Arturo Cepeda
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

#ifndef _MTIMER_H_
#define _MTIMER_H_

#ifdef _WIN32
# include <windows.h>
#else
# include <sys/time.h>
#endif

#include "mtypes.h"
#include "mutils.h"
#include "msong.h"

namespace Modus
{
    class Timer
    {
    private:
        double dStartTime;
        double dEndTime;
        bool bRunning;

    #ifdef WIN32
        LARGE_INTEGER iFrequency;
        LARGE_INTEGER iStartCount;
        LARGE_INTEGER iEndCount;
    #else
        timeval iStartCount;
        timeval iEndCount;
    #endif

    public:
        Timer();
        ~Timer();

        void start();
        void stop();
        double getTime();    // microseconds
    };

    /**
     *  @brief Timer object that provides a periodic signal to make it possible to play music according to
     *         a particular tempo. This timer is able to perform metric modulations, tempo changes, accelerandos 
     *         and ritardandos automatically when a song structure object is attached to it
     */
    class MCTimer
    {
    private:
        Timer tTimer;
        MSTimePosition mPosition;

        MCSongStructure* mStructure;
        unsigned int iCurrentSection;
        bool bAccRit;
        CLine* cTempoLine;

        bool bRunning;
        float fTempo;
        unsigned int iBeatsPerMeasure;
        double dTick;

        void (*fCallbackTick)(const MSTimePosition& TimePosition, void* Data);
        void (*fCallbackSection)(unsigned int NewSection, void* Data);
        void (*fCallbackEnd)(void* Data);

        void* fCallbackTickData;
        void* fCallbackSectionData;
        void* fCallbackEndData;

        double dTimeNow;
        double dTimeBefore;
        double dTimeDelta;

        void setSectionSettings(int iIndex);
        void locateSection();

        void callbackTick();
        void callbackSection();
        void callbackEnd();

    public:
        /**
         *  @brief Constructor
         */
        MCTimer(float Tempo = 120.0f, unsigned int BeatsPerMeasure = 4);
        ~MCTimer();

        /**
         *  @brief Activate the timer
         */
        void start();
        /**
         *  @brief Stop the timer
         */
        void stop();
        /**
         *  @brief Stop the timer and set it back to the initial time position (Measure 1, Beat 1, Tick 0)
         */
        void reset();
        /**
         *  @brief Update the timer. This method must be called from inside a loop and will throw an event each
         *         time a new tick comes. The time period is defined by the specified tempo and the event will
         *         be notified throw a callback function, which can be registered through the setCallbackTick()
         *         method
         *  @return Whether the update took place or not
         */
        bool update();

        /**
         *  @brief Check if the timer is running or stopped
         */
        bool isRunning();

        /**
         *  @brief Set the tempo
         *  @param Tempo New tempo
         */
        void setTempo(float Tempo);
        /**
         *  @brief Change the number of beats per measure
         *  @param BeatsPerMeasure Number of beats per measure
         */
        void setBeatsPerMeasure(unsigned int BeatsPerMeasure);
        /**
         *  @brief Set the current time position
         *  @param TimePosition New time position
         */
        void setTimePosition(const MSTimePosition& TimePosition);
        /**
         *  @brief Set the current time position
         *  @param Measure New time position: measure
         *  @param Beat New time position: beat
         *  @param Tick New time position: tick
         */
        void setTimePosition(int Measure, unsigned int Beat, unsigned int Tick);
        /**
         *  @brief Set the current time position to one or more measures before the first one (Measure 1)
         *  @param NumberOfMeasures Number of countoff measures
         */
        void setCountoff(unsigned int NumberOfMeasures);

        /**
         *  @brief Attach a song structure object to the timer, so it won't be necessary to adjust the tempo
         *         or the number of beats per measure manually anymore
         *  @param SongStructure Pointer to the song structure object to be attached
         */
        void setSongStructure(MCSongStructure* SongStructure);

        /**
         *  @brief Register a callback function which will be called each time a new tick comes
         *  @param function Function to be called
         *  @param Data Pointer to pass user data
         */
        void setCallbackTick(void (*function)(const MSTimePosition& TimePosition, void* Data), void* Data = NULL);
        /**
         *  @brief Register a callback function which will be called each time a new section comes
         *         (it only makes sense if a song structure object has been attached to the timer)
         *  @param function Function to be called
         *  @param Data Pointer to pass user data
         */
        void setCallbackSection(void (*function)(unsigned int NewSection, void* Data), void* Data = NULL);
        /**
         *  @brief Register a callback function which will be called when the end of the song is reached
         *         (it only makes sense if a song structure object has been attached to the timer)
         *  @param function Function to be called
         *  @param Data Pointer to pass user data
         */
        void setCallbackEnd(void (*function)(void* Data), void* Data = NULL);

        /**
         *  @brief Returns the current tempo
         */
        float getTempo();
        /**
         *  @brief Returns the current number of beats per measure
         */
        unsigned int getBeatsPerMeasure();
        /**
         *  @brief Returns the number of the current section in the song structure (it only makes sense if 
         *         a song structure object has been attached to the timer)
         */
        unsigned int getCurrentSection();
        /**
         *  @brief Returns the current time position
         */
        MSTimePosition& getTimePosition();
    };
}

#endif
