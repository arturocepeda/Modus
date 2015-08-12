
////////////////////////////////////////////////////////////////////////
//
//  Modus
//  C++ Music Library
//  [Sound Generator]
//
//  Copyright (c) 2012-2015 Arturo Cepeda
//
//  --------------------------------------------------------------------
//
//  Audio engine: 
//
//  XAudio2 (Windows Phone 8)
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

#include "mxsoundgenxaudio2.h"
#include <fstream>
#include <cmath>
#include "mutils.h"

using namespace Modus;


//
//  MCSourceVoiceCallback
//
MCSourceVoiceCallback::MCSourceVoiceCallback(MSSourceVoice& SourceVoiceRef)
    : sSourceVoiceRef(SourceVoiceRef)
{
}

MCSourceVoiceCallback::~MCSourceVoiceCallback()
{
}

void MCSourceVoiceCallback::OnStreamEnd()
{
    sSourceVoiceRef.Playing = false;
}

void MCSourceVoiceCallback::OnVoiceProcessingPassEnd()
{
}

void MCSourceVoiceCallback::OnVoiceProcessingPassStart(UINT32 SamplesRequired)
{
}

void MCSourceVoiceCallback::OnBufferEnd(void* pBufferContext)
{
}

void MCSourceVoiceCallback::OnBufferStart(void* pBufferContext)
{
}

void MCSourceVoiceCallback::OnLoopEnd(void* pBufferContext)
{
}

void MCSourceVoiceCallback::OnVoiceError(void* pBufferContext, HRESULT Error)
{
}


//
//  Audio source manager
//
MCXAudio2SourceManager::MCXAudio2SourceManager(IXAudio2* Engine, unsigned int NumSources)
   : MCAudioSourceManager(NumSources)
   , xaEngine(Engine)
{
}

void MCXAudio2SourceManager::allocateSources()
{


   fFrequencies = new float[iNumSources];
}

void MCXAudio2SourceManager::releaseSources()
{

   delete[] fFrequencies;
}

void MCXAudio2SourceManager::playSource(unsigned int SourceIndex, void* Sound, bool Sound3D)
{
}

void MCXAudio2SourceManager::stopSource(unsigned int SourceIndex)
{
}

bool MCXAudio2SourceManager::isSourcePlaying(unsigned int SourceIndex)
{
    return false;
}

void MCXAudio2SourceManager::setSourceVolume(unsigned int SourceIndex, float Volume)
{
   sSources[SourceIndex].Volume = Volume;

}

void MCXAudio2SourceManager::setSourcePitch(unsigned int SourceIndex, int Cents)
{
   float fFrequencyRatio = (float)pow(2, Cents / 1200.0f);
}

void MCXAudio2SourceManager::setSourcePan(unsigned int SourceIndex, float Pan)
{
}

void MCXAudio2SourceManager::setSourcePosition(unsigned int SourceIndex, float X, float Y, float Z)
{
   sSources[SourceIndex].Position[0] = X;
   sSources[SourceIndex].Position[1] = Y;
   sSources[SourceIndex].Position[2] = Z;

}

void MCXAudio2SourceManager::setSourceDirection(unsigned int SourceIndex, float X, float Y, float Z)
{
   sSources[SourceIndex].Direction[0] = X;
   sSources[SourceIndex].Direction[1] = Y;
   sSources[SourceIndex].Direction[2] = Z;

}


//
//  Sound generator
//
unsigned int MCSoundGenXAudio2::iNumberOfInstances = 0;

MCSoundGenXAudio2::MCSoundGenXAudio2(unsigned int ID, unsigned int NumberOfChannels, bool Sound3D, IXAudio2* Engine)
   : MCSoundGenAudioMultipleChannel(ID, NumberOfChannels, Sound3D)
{
   if(!cManager)
   {
      cManager = new MCXAudio2SourceManager(Engine, MODUS_XAUDIO2_SOURCES);
      cManager->allocateSources();
   }

   iNumberOfInstances++;

   iSampleSet = new int[iNumberOfChannels];
   fFrequency = new float[iNumberOfChannels];

   for(unsigned int i = 0; i < iNumberOfChannels; i++)
      iSampleSet[i] = -1;
}

MCSoundGenXAudio2::~MCSoundGenXAudio2()
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

void MCSoundGenXAudio2::loadSamples()
{
    if(sSampleSet.size() == 0)
        return;

    char sFilename[512];
    unsigned int i, j;
    //FMOD_MODE fmodMode;

    //fmodSounds = new FMOD::Sound**[sSampleSet.size()];

    //if(b3DSound)
    //    fmodMode = FMOD_SOFTWARE | FMOD_CREATESAMPLE | FMOD_3D;
    //else
    //    fmodMode = FMOD_SOFTWARE | FMOD_CREATESAMPLE;

    for(i = 0; i < sSampleSet.size(); i++)
    {
        //fmodSounds[i] = new FMOD::Sound*[iNumberOfSamples[i]];

        for(j = 0; j < iNumberOfSamples[i]; j++)
        {
            sprintf(sFilename, "%s%02d_%03d.%s", sSampleSet[i].Path, 
                                                 sSampleSet[i].ID, 
                                                 j + sSampleSet[i].Range.LowestNote,
                                                 sSampleSet[i].Format);

            //if(fmodSystem->createSound(sFilename, fmodMode, NULL, &fmodSounds[i][j]) != FMOD_OK)
            //    fmodSounds[i][j] = NULL;
        }
    }
}

void MCSoundGenXAudio2::loadSamplePack(std::istream& Stream, 
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

    //FMOD_MODE fmodMode;
    //FMOD_CREATESOUNDEXINFO fmodExinfo;

    //if(b3DSound)
    //    fmodMode = FMOD_OPENMEMORY | FMOD_SOFTWARE | FMOD_CREATESAMPLE | FMOD_3D;
    //else
    //    fmodMode = FMOD_OPENMEMORY | FMOD_SOFTWARE | FMOD_CREATESAMPLE;

    //memset(&fmodExinfo, 0, sizeof(fmodExinfo));
    //fmodExinfo.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);

    //fmodSounds = new FMOD::Sound**[iNumSampleSets];

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

        //fmodSounds[i] = new FMOD::Sound*[iNumSamples];

        unsigned int iSampleSize;
        char* sSampleData;

        for(j = 0; j < iNumSamples; j++)
        {
            Stream.read((char*)&iSampleSize, sizeof(unsigned int));
            //fmodSounds[i][j] = NULL;

            if(iSampleSize == 0)
                continue;

            sSampleData = new char[iSampleSize];
            Stream.read(sSampleData, iSampleSize);

            //fmodExinfo.length = iSampleSize;
            //fmodSystem->createSound(sSampleData, fmodMode, &fmodExinfo, &fmodSounds[i][j]);

            // callback
            if(callback)
                callback(iTotalSamples, ++iLoaded, Data);

            delete[] sSampleData;
        }
    }
}

void MCSoundGenXAudio2::unloadSamples()
{
    //if(!fmodSounds)
        //return;

    unsigned int i;
    unsigned int j;

    for(i = 0; i < sSampleSet.size(); i++)
    {
        for(j = 0; j < iNumberOfSamples[i]; j++)
        {
            //if(fmodSounds[i][j])
                //fmodSounds[i][j]->release();
        }

        //delete[] fmodSounds[i];
    }

    sSampleSet.clear();
    iNumberOfSamples.clear();

    //delete[] fmodSounds;
    //fmodSounds = NULL;
}

void MCSoundGenXAudio2::playAudioSample(unsigned int SourceIndex, int SampleSet, int SampleIndex)
{
   //cManager->playSource(SourceIndex, fmodSounds[SampleSet][SampleIndex], b3DSound);
}
