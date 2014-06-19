
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


//
//  Audio source manager
//
MCFMODSourceManager::MCFMODSourceManager(FMOD::System* AudioSystem, unsigned int NumSources)
   : MCAudioSourceManager(NumSources)
   , fmodSystem(AudioSystem)
{
}

void MCFMODSourceManager::allocateSources()
{
   fmodSystem->createChannelGroup("Modus", &fmodChannelGroup);

   for(unsigned int i = 0; i < iNumSources; i++)
      sSources[i].Source = 0;

   fFrequencies = new float[iNumSources];
}

void MCFMODSourceManager::releaseSources()
{
   for(unsigned int i = 0; i < iNumSources; i++)
   {
      FMOD::Channel* fmodChannel = reinterpret_cast<FMOD::Channel*>(sSources[i].Source);
      fmodChannel->stop();
   }

   fmodChannelGroup->release();
   delete[] fFrequencies;
}

void MCFMODSourceManager::playSource(unsigned int SourceIndex, void* Sound, bool Sound3D)
{
   if(fmodSystem->playSound(FMOD_CHANNEL_FREE, reinterpret_cast<FMOD::Sound*>(Sound), true, 
                            reinterpret_cast<FMOD::Channel**>(&sSources[SourceIndex].Source)) == FMOD_OK)
   {
      FMOD::Channel* fmodChannel = reinterpret_cast<FMOD::Channel*>(sSources[SourceIndex].Source);

      if(Sound3D)
      {
         fmodChannel->set3DAttributes(reinterpret_cast<const FMOD_VECTOR*>(sSources[SourceIndex].Position),
                                      reinterpret_cast<const FMOD_VECTOR*>(sSources[SourceIndex].Direction));
      }
      else
      {
         fmodChannel->setPan(sSources[SourceIndex].Position[0]);
      }

      fmodChannel->getFrequency(&fFrequencies[SourceIndex]);
      fmodChannel->setChannelGroup(fmodChannelGroup);
      fmodChannel->setVolume(sSources[SourceIndex].Volume);
      fmodChannel->setPaused(false);
   }
}

void MCFMODSourceManager::stopSource(unsigned int SourceIndex)
{
   FMOD::Channel* fmodChannel = reinterpret_cast<FMOD::Channel*>(sSources[SourceIndex].Source);
   fmodChannel->stop();
}

bool MCFMODSourceManager::isSourcePlaying(unsigned int SourceIndex)
{
   FMOD::Channel* fmodChannel = reinterpret_cast<FMOD::Channel*>(sSources[SourceIndex].Source);
   bool bChannelPlaying;

   fmodChannel->isPlaying(&bChannelPlaying);

   return bChannelPlaying;
}

void MCFMODSourceManager::setSourceVolume(unsigned int SourceIndex, float Volume)
{
   sSources[SourceIndex].Volume = Volume;

   FMOD::Channel* fmodChannel = reinterpret_cast<FMOD::Channel*>(sSources[SourceIndex].Source);
   bool bChannelPlaying;

   fmodChannel->isPlaying(&bChannelPlaying);

   if(bChannelPlaying)
      fmodChannel->setVolume(Volume);
}

void MCFMODSourceManager::setSourcePitch(unsigned int SourceIndex, int Cents)
{
   FMOD::Channel* fmodChannel = reinterpret_cast<FMOD::Channel*>(sSources[SourceIndex].Source);

   float fFrequencyRatio = (float)pow(2, Cents / 1200.0f);
   fmodChannel->setFrequency(fFrequencies[SourceIndex] * fFrequencyRatio);
}

void MCFMODSourceManager::setSourcePan(unsigned int SourceIndex, float Pan)
{
   sSources[SourceIndex].Position[0] = Pan;

   FMOD::Channel* fmodChannel = reinterpret_cast<FMOD::Channel*>(sSources[SourceIndex].Source);
   bool bChannelPlaying;

   fmodChannel->isPlaying(&bChannelPlaying);

   if(bChannelPlaying)
      fmodChannel->setPan(Pan);
}

void MCFMODSourceManager::setSourcePosition(unsigned int SourceIndex, float X, float Y, float Z)
{
   sSources[SourceIndex].Position[0] = X;
   sSources[SourceIndex].Position[1] = Y;
   sSources[SourceIndex].Position[2] = Z;

   FMOD::Channel* fmodChannel = reinterpret_cast<FMOD::Channel*>(sSources[SourceIndex].Source);
   bool bChannelPlaying;

   fmodChannel->isPlaying(&bChannelPlaying);

   if(bChannelPlaying)
   {
      fmodChannel->set3DAttributes(reinterpret_cast<const FMOD_VECTOR*>(sSources[SourceIndex].Position),
                                   reinterpret_cast<const FMOD_VECTOR*>(sSources[SourceIndex].Direction));
   }
}

void MCFMODSourceManager::setSourceDirection(unsigned int SourceIndex, float X, float Y, float Z)
{
   sSources[SourceIndex].Direction[0] = X;
   sSources[SourceIndex].Direction[1] = Y;
   sSources[SourceIndex].Direction[2] = Z;

   FMOD::Channel* fmodChannel = reinterpret_cast<FMOD::Channel*>(sSources[SourceIndex].Source);
   bool bChannelPlaying;

   fmodChannel->isPlaying(&bChannelPlaying);

   if(bChannelPlaying)
   {
      fmodChannel->set3DAttributes(reinterpret_cast<const FMOD_VECTOR*>(sSources[SourceIndex].Position),
                                   reinterpret_cast<const FMOD_VECTOR*>(sSources[SourceIndex].Direction));
   }
}


//
//  Sound generator
//
unsigned int MCSoundGenFMOD::iNumberOfInstances = 0;

MCSoundGenFMOD::MCSoundGenFMOD(unsigned int ID, unsigned int NumberOfChannels, bool Sound3D, FMOD::System* FMODSystem)
   : MCSoundGenAudioMultipleChannel(ID, NumberOfChannels, Sound3D)
   , fmodSystem(FMODSystem)
   , fmodSounds(0)
{
   if(!cManager)
   {
      cManager = new MCFMODSourceManager(FMODSystem, FMOD_SOURCES);
      cManager->allocateSources();
   }

   iNumberOfInstances++;

   iSampleSet = new int[iNumberOfChannels];
   fFrequency = new float[iNumberOfChannels];

   for(unsigned int i = 0; i < iNumberOfChannels; i++)
      iSampleSet[i] = -1;
}

MCSoundGenFMOD::~MCSoundGenFMOD()
{
   delete[] fFrequency;

   unloadSamples();

   iNumberOfInstances--;

   if(iNumberOfInstances == 0)
   {
      cManager->releaseSources();
      delete cManager;
      cManager = NULL;
   }
}

void MCSoundGenFMOD::loadSamples()
{
    if(sSampleSet.size() == 0)
        return;

    char sFilename[512];
    unsigned int i, j;
    FMOD_MODE fmodMode;

    fmodSounds = new FMOD::Sound**[sSampleSet.size()];

    if(b3DSound)
        fmodMode = FMOD_SOFTWARE | FMOD_CREATESAMPLE | FMOD_3D;
    else
        fmodMode = FMOD_SOFTWARE | FMOD_CREATESAMPLE;

    for(i = 0; i < sSampleSet.size(); i++)
    {
        fmodSounds[i] = new FMOD::Sound*[iNumberOfSamples[i]];

        for(j = 0; j < iNumberOfSamples[i]; j++)
        {
            sprintf(sFilename, "%s%02d_%03d.%s", sSampleSet[i].Path, 
                                                 sSampleSet[i].ID, 
                                                 j + sSampleSet[i].Range.LowestNote,
                                                 sSampleSet[i].Format);

            if(fmodSystem->createSound(sFilename, fmodMode, NULL, &fmodSounds[i][j]) != FMOD_OK)
                fmodSounds[i][j] = NULL;
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

    fmodSounds = new FMOD::Sound**[iNumSampleSets];

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

        fmodSounds[i] = new FMOD::Sound*[iNumSamples];

        unsigned int iSampleSize;
        char* sSampleData;

        for(j = 0; j < iNumSamples; j++)
        {
            Stream.read((char*)&iSampleSize, sizeof(unsigned int));
            fmodSounds[i][j] = NULL;

            if(iSampleSize == 0)
                continue;

            sSampleData = new char[iSampleSize];
            Stream.read(sSampleData, iSampleSize);

            fmodExinfo.length = iSampleSize;
            fmodSystem->createSound(sSampleData, fmodMode, &fmodExinfo, &fmodSounds[i][j]);

            // callback
            if(callback)
                callback(iTotalSamples, ++iLoaded, Data);

            delete[] sSampleData;
        }
    }
}

void MCSoundGenFMOD::unloadSamples()
{
    if(!fmodSounds)
        return;

    unsigned int i;
    unsigned int j;

    for(i = 0; i < sSampleSet.size(); i++)
    {
        for(j = 0; j < iNumberOfSamples[i]; j++)
        {
            if(fmodSounds[i][j])
                fmodSounds[i][j]->release();
        }

        delete[] fmodSounds[i];
    }

    sSampleSet.clear();
    iNumberOfSamples.clear();

    delete[] fmodSounds;
    fmodSounds = NULL;
}

void MCSoundGenFMOD::playAudioSample(unsigned int SourceIndex, int SampleSet, int SampleIndex)
{
   cManager->playSource(SourceIndex, fmodSounds[SampleSet][SampleIndex], b3DSound);
}
