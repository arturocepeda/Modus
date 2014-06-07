
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

#ifndef _MSOUNDGEN_H_
#define _MSOUNDGEN_H_

#include "mtypes.h"
#include "mutils.h"

#define M_DEFAULT_RELEASE   0.1f
#define M_QUICK_RELEASE     0.25f

/**
 *  @brief Abstract class intended to provide an interface to create sound generators that are able to
 *         generate sound through the sound card. A sound generator can be either used independently or
 *         attached to an instrument object
 */
class MCSoundGen
{
protected:
    unsigned int iID;
    unsigned char iNumberOfChannels;

    float fVolume;
    float fPan;
    float fReleaseSpeed;

    bool bDamper;
    CUniqueVector<unsigned char> vChannelsSustained;
    
    void releaseDamper();

public:
    /**
     *  @brief Constructor
     */
    MCSoundGen(unsigned int ID, unsigned char NumberOfChannels);
    virtual ~MCSoundGen() {}

    /**
     *  @brief Play a note
     *  @param Note Definition of the note to be played
     */
    virtual void playNote(MSNote& Note) = 0;
    /**
     *  @brief Release the note that is currently sounding through the specified channel
     *  @param Channel The note that is sounding through this channel will be released
     */
    virtual void releaseNote(unsigned char Channel) = 0;
    /**
     *  @brief Release all the notes that are currently sounding
     */
    void releaseAllNotes();
    /**
     *  @brief Update the sound generator. This method must be called with each new tick from the timer's tick 
     *         callback. If the sound generator is attached to an instrument, you will only need to update the 
     *         instrument
     */
    virtual void update() = 0;

    /**
     *  @brief Set the volume of the sound generator
     *  @param Volume Volume (value between 0.0f and 1.0f)
     */
    void setVolume(float Volume);
    /**
     *  @brief Set the pan of the sound generator
     *  @param Pan Pan (value between -1.0f and 1.0f)
     */
    void setPan(float Pan);
    /**
     *  @brief Bend the note that is currently sounding through the specified channel
     *  @param Channel The note that is sounding through this channel will be bended
     *  @param Cents Frequency variation in cents (100 cents = 1 semitone)
     */
    virtual void setBending(unsigned char Channel, int Cents) = 0;
    /**
     *  @brief Changes the intensity of the note that is currently sounding through the specified channel
     *  @param Channel The intensity of the note that is sounding through this channel will be changed
     *  @param Intensity New intensity value
     */
    virtual void setIntensity(unsigned char Channel, unsigned char Intensity) = 0;

    /**
     *  @brief Set the duration of the fade-out process each time a note is released
     *  @param ReleaseSpeed The higher this value is, the more abrupt the fade-out
     */
    void setReleaseSpeed(float ReleaseSpeed);

    /**
     *  @brief Activate or deactivate the damper pedal to sustain the notes
     *  @param DamperState New damper pedal's state
     */
    void setDamper(bool DamperState);
};


/**
 *  @brief Abstract class intended to provide an interface to create sound generators that work with
 *         digital audio samples. All the derived classes will maintain a list of sets of samples, where
 *         the audio files that must be loaded and the relationship between those files and the musical
 *         notes which the object can be asked to play are specified
 */
class MCSoundGenAudio : public MCSoundGen
{
protected:
    std::vector<unsigned char> iNumberOfSamples;
    std::vector<MSSampleSet> sSampleSet;

    bool b3DSound;
    float f3DPosition[3];
    float f3DDirection[3];

    int* iSampleSet;

    int findSampleSet(MSNote& Note);
    float calculateNoteVolume(unsigned char iIntensity, int iSampleSet);

public:
    /**
     *  @brief Constructor
     */
    MCSoundGenAudio(unsigned int ID, unsigned char NumberOfChannels, bool Sound3D);
    ~MCSoundGenAudio();

    /**
     *  @brief Release the note that is currently sounding through the specified channel
     *  @param Channel The note that is sounding through this channel will be released
     */
    virtual void releaseNote(unsigned char Channel) = 0;

    /**
     *  @brief Add a sample-set to the list
     *  @param SampleSet Sample-set that will be added
     */
    void addSampleSet(MSSampleSet& SampleSet);
    /**
     *  @brief Load all the sample files defined by the list
     */
    virtual void loadSamples() = 0;
    /**
     *  @brief Load sample-sets from a sample-pack file, which contains both information and audio files. 
     *         These files can be created with the "msamplepack" tool
     *  @param Stream Input file stream attached to the binary data
     *  @param callback Callback function which will be called each time a sample is loaded, to be able to 
     *                  track the sample loading process (for example, to show a progress bar). This function
     *                  will receive the total number of samples in the sample-pack and the current number of
     *                  samples loaded with each call
     *  @param Data Pointer to pass user data
     */
    virtual void loadSamplePack(std::istream& Stream, 
                                void (*callback)(unsigned int TotalSamples, unsigned int Loaded, void* Data) = NULL,
                                void* Data = NULL) = 0;
    /**
     *  @brief Load sample-sets from a sample-pack file, which contains both information and audio files. 
     *         These files can be created with the "msamplepack" tool
     *  @param Filename Sample-pack file
     *  @param callback Callback function which will be called each time a sample is loaded, to be able to 
     *                  track the sample loading process (for example, to show a progress bar). This function
     *                  will receive the total number of samples in the sample-pack and the current number of
     *                  samples loaded with each call
     *  @param Data Pointer to pass user data
     *  @return Whether the file could be readed or not
     */
    bool loadSamplePack(const char* Filename, 
                        void (*callback)(unsigned int TotalSamples, unsigned int Loaded, void* Data) = NULL,
                        void* Data = NULL);
    /**
     *  @brief Unload all the sample files defined by the list and clear the list
     */
    virtual void unloadSamples() = 0;

    /**
     *  @brief Get 3D position of the sound generator
     *  @param X Pointer to store the position: X
     *  @param Y Pointer to store the position: Y
     *  @param Z Pointer to store the position: Z
     */
    void get3DPosition(float* X, float* Y, float* Z);

    /**
     *  @brief Set 3D position of the sound generator
     *  @param X Position: X
     *  @param Y Position: Y
     *  @param Z Position: Z
     */
    virtual void set3DPosition(float X, float Y, float Z) = 0;

    /**
     *  @brief Get 3D direction of the sound generator
     *  @param X Pointer to store the direction: X
     *  @param Y Pointer to store the direction: Y
     *  @param Z Pointer to store the direction: Z
     */
    void get3DDirection(float* X, float* Y, float* Z);

    /**
     *  @brief Set 3D direction of the sound generator
     *  @param X direction: X
     *  @param Y direction: Y
     *  @param Z direction: Z
     */
    virtual void set3DDirection(float X, float Y, float Z) = 0;
};


struct MSAudioSource
{
    void* Source;
    unsigned int EntityID;
    unsigned int EntityChannel;
    unsigned int Assignment;

    float Volume;
    float Position[3];
    float Direction[3];

    bool Free;

    MSAudioSource();
};


class MCAudioSourceManager
{
protected:
    void* pAudioEngine;
    unsigned int iNumSources;

    MSAudioSource* sSources;
    unsigned int iCurrentAssignment;

    void assignSource(unsigned int SourceIndex, unsigned int EntityID, unsigned int EntityChannel);

public:
    MCAudioSourceManager(void* AudioEngine, unsigned int NumSources);
    virtual ~MCAudioSourceManager();

    virtual void allocateSources() = 0;
    virtual void releaseSources() = 0;

    unsigned int assignSource(unsigned int EntityID, unsigned int EntityChannel);
    void freeSource(unsigned int SourceIndex);

    const MSAudioSource& getSource(unsigned int SourceIndex);

    virtual void playSource(unsigned int SourceIndex, void* Sound) = 0;
    virtual void stopSource(unsigned int SourceIndex) = 0;

    virtual bool isSourcePlaying(unsigned int SourceIndex) = 0;

    float getSourceVolume(unsigned int SourceIndex);
    float getSourcePan(unsigned int SourceIndex);
    void getSourcePosition(unsigned int SourceIndex, float* X, float* Y, float* Z);
    void getSourceDirection(unsigned int SourceIndex, float* X, float* Y, float* Z);

    virtual void setSourceVolume(unsigned int SourceIndex, float Volume) = 0;
    virtual void setSourcePitch(unsigned int SourceIndex, int Cents) = 0;
    virtual void setSourcePan(unsigned int SourceIndex, float Pan) = 0;
    virtual void setSourcePosition(unsigned int SourceIndex, float X, float Y, float Z) = 0;
    virtual void setSourceDirection(unsigned int SourceIndex, float X, float Y, float Z) = 0;
};


struct MSAudioChannel
{
    unsigned int Index;
    bool Release;
    bool QuickRelease;
    float InitialReleaseVolume;
    float CurrentReleaseState;

    MSAudioChannel() : Release(false), QuickRelease(false) {}
};


struct MSInstrumentChannel
{
    std::vector<MSAudioChannel> AudioChannels;
};


/**
 *  @brief Abstract class intended to provide an interface to create sound generators that work with
 *         digital audio samples. This particular kind of audio sound generator utilizes multiple real
 *         audio channels per instrument channel so that each time a note is released, its corresponding
 *         audio channel is put on the background whereas the note that was most recently played always
 *         sounds through the first audio channel in the list of audio channels attached to the instrument
 *         channel
 */
class MCSoundGenAudioMultipleChannel : public MCSoundGenAudio
{
protected:
    MSInstrumentChannel* sInstrumentChannels;
    CUniqueVector<unsigned int> vChannelsToRelease;

    static MCAudioSourceManager* cManager;

    virtual void playAudioSample(unsigned int AudioChannelIndex, int SampleSet, int SampleIndex) = 0;

    void releaseChannel(unsigned char iChannel, bool bQuickly);

public:
    /**
     *  @brief Constructor
     */
    MCSoundGenAudioMultipleChannel(unsigned int ID, unsigned char NumberOfChannels, bool Sound3D);
    ~MCSoundGenAudioMultipleChannel();

    void playNote(MSNote& Note);
    /**
     *  @brief Release the note that is currently sounding through the specified channel
     *  @param Channel The note that is sounding through this channel will be released
     */
    void releaseNote(unsigned char Channel);

    void update();

    void setBending(unsigned char Channel, int Cents);
    void setIntensity(unsigned char Channel, unsigned char Intensity);
    void set3DPosition(float X, float Y, float Z);
    void set3DDirection(float X, float Y, float Z);
};

#endif
