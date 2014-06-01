
////////////////////////////////////////////////////////////////////////
//
//  Modus
//  C++ Music Library
//  [Sound Generator]
//
//  Copyright (c) 2012-2014 Arturo Cepeda
//
//  --------------------------------------------------------------------
//
//  Audio engine: 
//
//  FMOD Sound System
//  Copyright (c) Firelight Technologies Pty, Ltd., 1994-2012
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

#include "mxsoundgenfmod.h"
#include <fstream>
#include <cmath>
#include "mutils.h"

MCSoundGenFMOD::MCSoundGenFMOD(unsigned int NumberOfChannels, bool Sound3D, FMOD::System* FMODSystem)
{
    iNumberOfChannels = NumberOfChannels;
    b3DSound = Sound3D;
    fmodSystem = FMODSystem;
    fmodSound = NULL;
    fVolume = 1.0f;
    fPan = 0.0f;
    fReleaseSpeed = M_DEFAULT_RELEASE;
    bDamper = false;

    fmod3DPosition.x = 0.0f;
    fmod3DPosition.y = 0.0f;
    fmod3DPosition.z = 0.0f;

    fmodChannel = new FMOD::Channel**[iNumberOfChannels];
    iSampleSet = new int[iNumberOfChannels];
    fFrequency = new float[iNumberOfChannels];

    fmodSystem->createChannelGroup("Instrument", &fmodChannelGroup);

    for(unsigned int i = 0; i < iNumberOfChannels; i++)
    {
        fmodChannel[i] = new FMOD::Channel*[2];

        fmodChannel[i][0] = NULL;
        fmodChannel[i][1] = NULL;

        iSampleSet[i] = -1;
    }

    initChannelData();
}

MCSoundGenFMOD::~MCSoundGenFMOD()
{
    unsigned int i;

    fmodChannelGroup->release();

    for(i = 0; i < iNumberOfChannels; i++)
        delete[] fmodChannel[i];

    delete[] fmodChannel;
    delete[] iSampleSet;
    delete[] fFrequency;

    for(i = 0; i < fmodDSP.size(); i++)
        fmodDSP[i]->release();

    unloadSamples();
    releaseChannelData();
}

unsigned int MCSoundGenFMOD::addEffect(FMOD_DSP_TYPE fmodDSPType)
{
    FMOD::DSP* fmodNewDSP;
    unsigned int iDSPIndex;

    fmodSystem->createDSPByType(fmodDSPType, &fmodNewDSP);
    fmodDSP.push_back(fmodNewDSP);
    iDSPIndex = fmodDSP.size() - 1;
    fmodChannelGroup->addDSP(fmodDSP[iDSPIndex], NULL);

    return iDSPIndex;
}

void MCSoundGenFMOD::loadSamples()
{
    if(sSampleSet.size() == 0)
        return;

    char sFilename[512];
    unsigned int i, j;
    FMOD_MODE fmodMode;

    fmodSound = new FMOD::Sound**[sSampleSet.size()];

    if(b3DSound)
        fmodMode = FMOD_SOFTWARE | FMOD_CREATESAMPLE | FMOD_3D;
    else
        fmodMode = FMOD_SOFTWARE | FMOD_CREATESAMPLE;

    for(i = 0; i < sSampleSet.size(); i++)
    {
        fmodSound[i] = new FMOD::Sound*[iNumberOfSamples[i]];

        for(j = 0; j < iNumberOfSamples[i]; j++)
        {
            sprintf(sFilename, "%s%02d_%03d.%s", sSampleSet[i].Path, 
                                                 sSampleSet[i].ID, 
                                                 j + sSampleSet[i].Range.LowestNote,
                                                 sSampleSet[i].Format);

            if(fmodSystem->createSound(sFilename, fmodMode, NULL, &fmodSound[i][j]) != FMOD_OK)
                fmodSound[i][j] = NULL;
        }
    }
}

void MCSoundGenFMOD::loadSamplePack(std::istream& Stream, 
                                    void (*callback)(unsigned int TotalSamples, unsigned int Loaded, void* Data),
                                    void* Data)
{
    unsigned int iTotalSamples = 0;
    unsigned int iLoaded = 0;

    // read total number of samples
    if(callback)
    {
        Stream.seekg(-(std::ios::off_type)sizeof(unsigned int), std::ios::end);
        Stream.read((char*)&iTotalSamples, sizeof(unsigned int));
        Stream.seekg(0, std::ios::beg);
    }

    char sVersion[16];
    char sFormat[8];
    char sReserved[4];

    unsigned int iInput;
    unsigned int iNumSampleSets;

    Stream.read(sVersion, 16);                                     // Version
    Stream.read(sFormat, 8);                                       // Format
    Stream.read((char*)&iNumSampleSets, sizeof(unsigned int));     // Number of sample sets
    Stream.read(sReserved, 4);                                     // (Reserved)

    unsigned int i;
    unsigned int j;

    // release previously loaded samples
    unloadSamples();

    // clear sampleset list
    if(sSampleSet.size() > 0)
    {
        sSampleSet.clear();
        iNumberOfSamples.clear();
    }

    // load samplesets and samples
    MSSampleSet mSampleSet;
    unsigned int iNumSamples;

    FMOD_MODE fmodMode;
    FMOD_CREATESOUNDEXINFO fmodExinfo;

    if(b3DSound)
        fmodMode = FMOD_OPENMEMORY | FMOD_SOFTWARE | FMOD_CREATESAMPLE | FMOD_3D;
    else
        fmodMode = FMOD_OPENMEMORY | FMOD_SOFTWARE | FMOD_CREATESAMPLE;

    memset(&fmodExinfo, 0, sizeof(fmodExinfo));
    fmodExinfo.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);

    fmodSound = new FMOD::Sound**[iNumSampleSets];

    for(i = 0; i < iNumSampleSets; i++)
    {
        // sample set
        strcpy(mSampleSet.Path, "");
        strcpy(mSampleSet.Format, sFormat);

        Stream.read((char*)&iInput, sizeof(unsigned int));        // SampleSet: ID
        mSampleSet.ID = (unsigned char)iInput;

        Stream.read((char*)&iInput, sizeof(unsigned int));        // SampleSet: Mode
        mSampleSet.Mode = (unsigned char)iInput;

        Stream.read((char*)&iInput, sizeof(unsigned int));        // SampleSet: Minimum intensity
        mSampleSet.MinimumIntensity = (unsigned char)iInput;

        Stream.read((char*)&iInput, sizeof(unsigned int));        // SampleSet: Maximum intensity
        mSampleSet.MaximumIntensity = (unsigned char)iInput;

        Stream.read((char*)&iInput, sizeof(unsigned int));        // SampleSet: Lowest pitch
        mSampleSet.Range.LowestNote = (unsigned char)iInput;

        Stream.read((char*)&iInput, sizeof(unsigned int));        // SampleSet: Highest pitch
        mSampleSet.Range.HighestNote = (unsigned char)iInput;

        sSampleSet.push_back(mSampleSet);

        // samples inside the sample set
        Stream.read((char*)&iNumSamples, sizeof(unsigned int));   // SampleSet: Number of samples
        iNumberOfSamples.push_back((unsigned char)iNumSamples);

        Stream.read(sReserved, 4);                                // SampleSet: (Reserved)

        fmodSound[i] = new FMOD::Sound*[iNumSamples];

        unsigned int iSampleSize;
        char* sSampleData;

        for(j = 0; j < iNumSamples; j++)
        {
            Stream.read((char*)&iSampleSize, sizeof(unsigned int));
            fmodSound[i][j] = NULL;

            if(iSampleSize == 0)
                continue;

            sSampleData = new char[iSampleSize];
            Stream.read(sSampleData, iSampleSize);

            fmodExinfo.length = iSampleSize;
            fmodSystem->createSound(sSampleData, fmodMode, &fmodExinfo, &fmodSound[i][j]);

            // callback
            if(callback)
                callback(iTotalSamples, ++iLoaded, Data);

            delete[] sSampleData;
        }
    }
}

void MCSoundGenFMOD::unloadSamples()
{
    if(!fmodSound)
        return;

    unsigned int i;
    unsigned int j;

    for(i = 0; i < sSampleSet.size(); i++)
    {
        for(j = 0; j < iNumberOfSamples[i]; j++)
        {
            if(fmodSound[i][j])
                fmodSound[i][j]->release();
        }

        delete[] fmodSound[i];
    }

    sSampleSet.clear();
    iNumberOfSamples.clear();

    delete[] fmodSound;
    fmodSound = NULL;
}

void MCSoundGenFMOD::playNote(MSNote& Note)
{
    if(!fmodSound)
        return;

    // find the appropriate sample set for this note
    iSampleSet[Note.Channel] = findSampleSet(Note);

    if(iSampleSet[Note.Channel] == -1)
        return;

    // calculate sample number
    int iSample = Note.Pitch - sSampleSet[iSampleSet[Note.Channel]].Range.LowestNote;

    // set the channel free
    releaseChannel(Note.Channel, true);

    // if the channel is in any sustained list, remove it
    vChannelsSustained.removeValue(Note.Channel);
    vSustainedChannelsToRelease.removeValue(Note.Channel);

    // play the note
    if(fmodSystem->playSound(FMOD_CHANNEL_FREE, fmodSound[iSampleSet[Note.Channel]][iSample], true, 
                             &fmodChannel[Note.Channel][0]) == FMOD_OK)
    {
        // 3D sound
        if(b3DSound)
            fmodChannel[Note.Channel][0]->set3DAttributes(&fmod3DPosition, NULL);

        // 2D sound
        else
            fmodChannel[Note.Channel][0]->setPan(fPan);

        fmodChannel[Note.Channel][0]->getFrequency(&fFrequency[Note.Channel]);
        fmodChannel[Note.Channel][0]->setChannelGroup(fmodChannelGroup);
        fmodChannel[Note.Channel][0]->setVolume(calculateNoteVolume(Note.Intensity, iSampleSet[Note.Channel]));
        fmodChannel[Note.Channel][0]->setPaused(false);
    }
}

void MCSoundGenFMOD::releaseChannel(unsigned char iChannel, bool bQuickly)
{
    if(!fmodSound || iChannel >= iNumberOfChannels || !fmodChannel[iChannel][0])
        return;

    // check whether the primary channel is playing a sound
    bool bChannelPlaying;
    fmodChannel[iChannel][0]->isPlaying(&bChannelPlaying);

    // if the primary channel is playing a sound, change the pointer to the secondary channel and notify
    // the generator to release it
    if(bChannelPlaying)
    {
        // we need two values: the volume of the channel at the moment (initial) and another value that
        // always goes from 1.0f to 0.0f, in order to do a fade out that doesn't depend on the initial
        // volume
        fmodChannel[iChannel][0]->getVolume(&fInitialReleaseVolume[iChannel]);
        fCurrentReleaseVolume[iChannel] = 1.0f;

        // if the secondary channel is already busy, set it free
        if(fmodChannel[iChannel][1])
            fmodChannel[iChannel][1]->stop();

        fmodChannel[iChannel][1] = fmodChannel[iChannel][0];
        fmodChannel[iChannel][0] = NULL;

        bQuickRelease[iChannel] = bQuickly;
        bRelease[iChannel] = !bQuickly;

        // add the current channel to the list to be released
        vChannelsToRelease.add(iChannel);
    }
}

void MCSoundGenFMOD::update()
{
    unsigned int iChannel;

    // a fade out will be done to the secondary channels that must be released
    for(unsigned int i = 0; i < vChannelsToRelease.size(); i++)
    {
        iChannel = vChannelsToRelease[i];

        if(!fmodChannel[iChannel][1])
            continue;

        // next step for the fade-out
        if(fCurrentReleaseVolume[iChannel] > 0.0f)
        {
            fCurrentReleaseVolume[iChannel] -= (bQuickRelease[iChannel])? M_QUICK_RELEASE: fReleaseSpeed;
            fmodChannel[iChannel][1]->setVolume(fCurrentReleaseVolume[iChannel] * fInitialReleaseVolume[iChannel]);
            
            // check damper
            if(bDamper)
            {
                vSustainedChannelsToRelease.add(iChannel);
                vChannelsToRelease.remove(i);
                i--;
            }            
        }

        // fade-out completed: stop source
        else
        {
            fmodChannel[iChannel][1]->stop();
            fmodChannel[iChannel][1] = NULL;

            bRelease[iChannel] = false;
            bQuickRelease[iChannel] = false;

            vChannelsToRelease.remove(i);
            i--;
        }
    }
}

void MCSoundGenFMOD::get3DPosition(float* X, float* Y, float* Z)
{
    *X = fmod3DPosition.x;
    *Y = fmod3DPosition.y;
    *Z = fmod3DPosition.z;
}

void MCSoundGenFMOD::setBending(unsigned char Channel, int Cents)
{
    if(!fmodSound || !fmodChannel[Channel][0])
        return;

    float fFrequencyRatio = (float)pow(2, Cents / 1200.0f);
    fmodChannel[Channel][0]->setFrequency(fFrequency[Channel] * fFrequencyRatio);
}

void MCSoundGenFMOD::setIntensity(unsigned char Channel, unsigned char Intensity)
{
    if(iSampleSet[Channel] > -1)
        fmodChannel[Channel][0]->setVolume(calculateNoteVolume(Intensity, iSampleSet[Channel]));
}

void MCSoundGenFMOD::set3DPosition(float X, float Y, float Z)
{
    unsigned int i;
    unsigned int j;
    bool bChannelPlaying;

    fmod3DPosition.x = X;
    fmod3DPosition.y = Y;
    fmod3DPosition.z = Z;

    // take effect on the channels that are sounding
    for(i = 0; i < iNumberOfChannels; i++)
    {
        for(j = 0; j < 2 && fmodChannel[i][j]; j++)
        {
            fmodChannel[i][j]->isPlaying(&bChannelPlaying);

            if(bChannelPlaying)
                fmodChannel[i][j]->set3DAttributes(&fmod3DPosition, NULL);
        }
    }
}

void MCSoundGenFMOD::setEffectParameter(unsigned int EffectIndex, int ParameterIndex, float Value)
{
    if(EffectIndex < fmodDSP.size())
        fmodDSP[EffectIndex]->setParameter(ParameterIndex, Value);
}
