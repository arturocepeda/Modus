
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

#ifndef _MINSTRUMENT_H_
#define _MINSTRUMENT_H_

#include "mtypes.h"
#include "mscore.h"
#include "msoundgen.h"
#include "mutils.h"

namespace Modus
{
    /**
    *  @brief Represents a progressive change that is applied to a note. Concretely, this is
    *         used for bending, vibrato and intensity changes
    */
    struct MSNoteChange
    {
        unsigned char Channel;

        union
        {
            unsigned int NumberOfSteps;
            float Amplitude;
        };

        union
        {
            unsigned int CurrentStep;
            float CurrentPosition;
        };

        float Increment;
    };

    /**
    *  @brief Represents an entity that is able to play notes
    */
    class MCInstrument : private CNonCopyable
    {
    protected:
        unsigned int iID;
        MSRange mRange;
        unsigned char iNumberOfChannels;
        char iTranspose;
        char iIntensityVariation;

        unsigned int iCurrentMoment;
        unsigned int iScorePosition;

        CUniqueVector<unsigned char> vChannelsToRelease;
        std::vector<MSNoteChange> vBending;
        std::vector<MSNoteChange> vVibrato;
        std::vector<MSNoteChange> vIntensityChange;    

        MSNote* mCurrentNote;
        unsigned int* iReleaseMoment;
        float* fBendingState;
        bool* bVibrato;
        float* fCurrentIntensity;

        MCSoundGen* mSoundGen;
        MCScore* mScore;

        void control(const MSNote& mNote);
        void checkScore(const MSTimePosition& TimePosition);

        void (*fCallbackPlay)(unsigned int InstrumentID, const MSNote& Note, void* Data);
        void (*fCallbackNextNote)(unsigned int InstrumentID, const MSNote& Note, 
                                    const MSTimePosition& CurrentTimePosition,
                                    const MSTimePosition& NoteTimePosition, 
                                    void* Data);
        void (*fCallbackRelease)(unsigned int InstrumentID, const MSNote& Note, void* Data);
        void (*fCallbackBending)(unsigned int InstrumentID, const MSNote& Note, int Cents, void* Data);
        void (*fCallbackDamper)(unsigned int InstrumentID, bool DamperState, void* Data);
        void (*fCallbackIntensity)(unsigned int InstrumentID, const MSNote& Note, unsigned char Intensity, void* Data);

        void* fCallbackPlayData;
        void* fCallbackNextNoteData;
        void* fCallbackReleaseData;
        void* fCallbackBendingData;
        void* fCallbackDamperData;
        void* fCallbackIntensityData;

    public:
        /**
        *  @brief Constructor
        */
        MCInstrument(unsigned int ID, const MSRange& Range, unsigned char NumberOfChannels);
        ~MCInstrument();
    
        /**
        *  @brief Play a note
        *  @param Note Definition of the note to be played
        */
        void playNote(const MSNote& Note);
        /**
        *  @brief Play a chord
        *  @param PitchList List of pitches of which the chord is made up
        *  @param Intensity Chord note's intensity
        *  @param Mode Chord note's mode
        *  @param ListOfChannels Pointer to the list of channels corresponding to the chord's pitches
        *  @param Duration Chord note's duration
        */
        void playChord(const MTNoteMap& PitchList, unsigned char Intensity, unsigned char Mode, 
                        unsigned char* ListOfChannels, unsigned char Duration);
        /**
        *  @brief Play a chord
        *  @param PitchList List of pitches of which the chord is made up
        *  @param Intensity Chord note's intensity
        *  @param Mode Chord note's mode
        *  @param FirstChannel Channel for the lowest note. Consecutive values will be assigned to the rest of the notes
        *  @param Duration Chord note's duration
        */
        void playChord(const MTNoteMap& PitchList, unsigned char Intensity, unsigned char Mode, 
                        unsigned char FirstChannel, unsigned char Duration);

        /**
        *  @brief Release the note that is currently being played through the specified channel
        *  @param Channel The note that is sounding through this channel will be released
        */
        void release(unsigned char Channel);
        /**
        *  @brief Release all the notes that are currently being played by the instrument
        */
        void releaseAll();

        /**
        *  @brief Bend the note that is currently being played through the specified channel
        *  @param Channel The note that is sounding through this channel will be bended
        *  @param Cents Frequency variation in cents (100 cents = 1 semitone)
        */
        void bend(unsigned char Channel, int Cents);
        /**
        *  @brief Defines a bending that takes several ticks to reach the desired position
        *  @param Channel The note that is sounding through this channel will be bended
        *  @param Cents Frequency variation in cents (100 cents = 1 semitone)
        *  @param NumberOfTicks Number of ticks to reach the desired frequency
        */
        void bend(unsigned char Channel, int Cents, unsigned int NumberOfTicks);
        /**
        *  @brief Plays with vibrato until the note is released or a new note is played through the same channel
        *  @param Channel The vibrato will have effect on the note that is sounding through this channel
        *  @param Cents Frequency variation in cents (100 cents = 1 semitone)
        *  @param NumberOfTicksPerCycle Cycle length in number of ticks
        */
        void vibrato(unsigned char Channel, int Cents, unsigned int NumberOfTicksPerCycle);
        /**
        *  @brief Changes the intensity of the note that is currently being played through the specified channel
        *  @param Channel The intensity of the note that is sounding through this channel will be changed
        *  @param Intensity New intensity value
        */
        void changeIntensity(unsigned char Channel, unsigned char Intensity);
        /**
        *  @brief Defines an intensity change that takes several ticks to reach the desired intensity
        *  @param Channel The intensity of the note that is sounding through this channel will be changed
        *  @param Intensity New intensity value
        *  @param NumberOfTicks Number of ticks to reach the desired intensity
        */
        void changeIntensity(unsigned char Channel, unsigned char Intensity, unsigned int NumberOfTicks);

        /**
        *  @brief Update the instrument. This method must be called with each new tick from the timer's tick 
        *         callback. If the instrument is part of a band, you only need to update the band
        *  @param TimePosition Current time position
        */
        void update(const MSTimePosition& TimePosition);

        /**
        *  @brief Returns the ID number of the instrument
        */
        unsigned int getID();
        /**
        *  @brief Returns the number of channels of the instrument
        */
        unsigned char getNumberOfChannels();
        /**
        *  @brief Returns the range of the instrument
        */
        MSRange& getRange();
        /**
        *  @brief Returns the number of different notes the instrument is able to play
        */  
        unsigned char getRangeSize();

        /**
        *  @brief Attach a sound generator to the instrument
        *  @param SoundGen Pointer to the sound generator to be attached
        */
        void setSoundGen(MCSoundGen* SoundGen);
        /**
        *  @brief Attach a score to the instrument
        *  @param Score Pointer to the score object to be attached
        */
        void setScore(MCScore* Score);

        /**
        *  @brief Activate or deactivate the damper pedal to sustain the notes
        *  @param DamperState New damper pedal's state
        */
        void setDamper(bool DamperState);

        /**
        *  @brief Define a transposition, so all the notes that come will be transposed
        *  @param NumberOfSemitones Quantity of the transposition (it can be negative)
        */
        void setTranspose(char NumberOfSemitones);
        /**
        *  @brief Define an intensity variation, so the intensity of the notes that come will be varied
        *  @param Variation Quantity of the intensity variation (it can be negative)
        */
        void setIntensityVariation(char Variation);

        /**
        *  @brief Set up a callback function which will be called each time a note is played
        *  @param function Function to be called
        *  @param Data Pointer to pass user data
        */
        void setCallbackPlay(void (*function)(unsigned int InstrumentID, const MSNote& Note, void* Data), 
                            void* Data = NULL);
        /**
        *  @brief Set up a callback function which will be called each time a note is played with information
        *         about the next notes to be played, only if the instrument has an attached score
        *  @param function Function to be called
        *  @param Data Pointer to pass user data
        */ 
        void setCallbackNextNote(void (*function)(unsigned int InstrumentID, const MSNote& Note, 
                                                    const MSTimePosition& CurrentTimePosition,
                                                    const MSTimePosition& NoteTimePosition, void* Data), 
                                void* Data = NULL);
        /**
        *  @brief Set up a callback function which will be called each time a note is released
        *  @param function Function to be called
        *  @param Data Pointer to pass user data
        */
        void setCallbackRelease(void (*function)(unsigned int InstrumentID, const MSNote& Note, void* Data), 
                                void* Data = NULL);
        /**
        *  @brief Set up a callback function which will be called each time a note is bended
        *  @param function Function to be called
        *  @param Data Pointer to pass user data
        */
        void setCallbackBending(void (*function)(unsigned int InstrumentID, const MSNote& Note, int Cents, void* Data),
                                void* Data = NULL);
        /**
        *  @brief Set up a callback function which will be called each time the damper pedal's state changes
        *  @param function Function to be called
        *  @param Data Pointer to pass user data
        */
        void setCallbackDamper(void (*function)(unsigned int InstrumentID, bool DamperState, void* Data), 
                                void* Data = NULL);
        /**
        *  @brief Set up a callback function which will be called each time a note's intensity changes
        *  @param function Function to be called
        *  @param Data Pointer to pass user data
        */
        void setCallbackIntensity(void (*function)(unsigned int InstrumentID, const MSNote& Note, unsigned char Intensity,
                                                    void* Data), void* Data = NULL);
    };
}

#endif
