
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
//  OpenSL ES (Android)
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

#include "mxsoundgenopensl.h"
#include <fstream>
#include <cmath>
#include "mutils.h"

using namespace Modus;

//
//  Audio source manager
//
MCOpenSLSourceManager::MCOpenSLSourceManager(unsigned int NumSources)
   : MCAudioSourceManager(NumSources)
{
   // create engine and output mix
   slCreateEngine(&slEngineObject, 0, NULL, 0, NULL, NULL);
   (*slEngineObject)->Realize(slEngineObject, SL_BOOLEAN_FALSE);
   (*slEngineObject)->GetInterface(slEngineObject, SL_IID_ENGINE, (void*)&slEngine);
   
   const SLuint32 slEngineIdCount = 1;
   const SLInterfaceID slEngineIds[1] = { SL_IID_ENVIRONMENTALREVERB };
   const SLboolean slEngineReq[1] = { SL_BOOLEAN_FALSE };
   (*slEngine)->CreateOutputMix(slEngine, &slOutputMix, slEngineIdCount, slEngineIds, slEngineReq);
   (*slOutputMix)->Realize(slOutputMix, SL_BOOLEAN_FALSE);
}

MCOpenSLSourceManager::~MCOpenSLSourceManager()
{
   (*slOutputMix)->Destroy(slOutputMix);
}

void MCOpenSLSourceManager::allocateSources()
{
   slSources = new OpenSLSource[iNumSources];
}

void MCOpenSLSourceManager::releaseSources()
{
   for(unsigned int i = 0; i < iNumSources; i++)
   {
      if(slSources[i].AudioPlayer)
         (*slSources[i].AudioPlayer)->Destroy(slSources[i].AudioPlayer);
   }

   delete[] slSources;
}

void MCOpenSLSourceManager::playSource(unsigned int SourceIndex, void* Sound, bool Sound3D)
{
   MSAudioBuffer* slBuffer = reinterpret_cast<MSAudioBuffer*>(Sound);

   SLDataLocator_AndroidFD slDataLocatorFD = { SL_DATALOCATOR_ANDROIDFD, slBuffer->FileDescriptor, slBuffer->Start, slBuffer->Length };
   SLDataFormat_MIME slDataFormat = { SL_DATAFORMAT_MIME, NULL, SL_CONTAINERTYPE_UNSPECIFIED };
   SLDataSource slDataSource = { &slDataLocatorFD, &slDataFormat };
   SLDataLocator_OutputMix slDataLocatorOutputMix = { SL_DATALOCATOR_OUTPUTMIX, slOutputMix };
   SLDataSink slDataSink = { &slDataLocatorOutputMix, NULL };

   const SLInterfaceID slAudioPlayerIds[1] = { SL_IID_VOLUME };
   const SLboolean slAudioPlayerReq[1] = { SL_BOOLEAN_TRUE };

   (*slEngine)->CreateAudioPlayer(slEngine, &slSources[SourceIndex].AudioPlayer, &slDataSource, &slDataSink, 1, slAudioPlayerIds, slAudioPlayerReq);
   (*slSources[SourceIndex].AudioPlayer)->Realize(slSources[SourceIndex].AudioPlayer, SL_BOOLEAN_FALSE);
   (*slSources[SourceIndex].AudioPlayer)->GetInterface(slSources[SourceIndex].AudioPlayer, SL_IID_PLAY, &slSources[SourceIndex].PlaybackState);
   (*slSources[SourceIndex].AudioPlayer)->GetInterface(slSources[SourceIndex].AudioPlayer, SL_IID_VOLUME, &slSources[SourceIndex].VolumeController);

   (*slSources[SourceIndex].VolumeController)->SetVolumeLevel(slSources[SourceIndex].VolumeController, linearToMillibel(sSources[SourceIndex].Volume));
   (*slSources[SourceIndex].PlaybackState)->SetPlayState(slSources[SourceIndex].PlaybackState, SL_PLAYSTATE_PLAYING);
}

void MCOpenSLSourceManager::stopSource(unsigned int SourceIndex)
{
   (*slSources[SourceIndex].PlaybackState)->SetPlayState(slSources[SourceIndex].PlaybackState, SL_PLAYSTATE_STOPPED);
   (*slSources[SourceIndex].AudioPlayer)->Destroy(slSources[SourceIndex].AudioPlayer);
   slSources[SourceIndex].AudioPlayer = 0;
}

bool MCOpenSLSourceManager::isSourcePlaying(unsigned int SourceIndex)
{
   if(!slSources[SourceIndex].AudioPlayer)
      return false;

   SLuint32 iPlaybackState = 0;
   (*slSources[SourceIndex].PlaybackState)->GetPlayState(slSources[SourceIndex].PlaybackState, &iPlaybackState);
   return iPlaybackState == SL_PLAYSTATE_PLAYING;
}

void MCOpenSLSourceManager::setSourceVolume(unsigned int SourceIndex, float Volume)
{
   sSources[SourceIndex].Volume = Volume;

   if(isSourcePlaying(SourceIndex))
      (*slSources[SourceIndex].VolumeController)->SetVolumeLevel(slSources[SourceIndex].VolumeController, linearToMillibel(Volume));
}

void MCOpenSLSourceManager::setSourcePitch(unsigned int SourceIndex, int Cents)
{

}

void MCOpenSLSourceManager::setSourcePan(unsigned int SourceIndex, float Pan)
{

}

void MCOpenSLSourceManager::setSourcePosition(unsigned int SourceIndex, float X, float Y, float Z)
{
    sSources[SourceIndex].Position[0] = X;
    sSources[SourceIndex].Position[1] = Y;
    sSources[SourceIndex].Position[2] = Z;


}

void MCOpenSLSourceManager::setSourceDirection(unsigned int SourceIndex, float X, float Y, float Z)
{
    sSources[SourceIndex].Direction[0] = X;
    sSources[SourceIndex].Direction[1] = Y;
    sSources[SourceIndex].Direction[2] = Z;


}

SLmillibel MCOpenSLSourceManager::linearToMillibel(float fGain)
{
   if(fGain >= 1.0f)
      return 0;

   if(fGain <= 0.0f)
      return SL_MILLIBEL_MIN;

   return (SLmillibel)(2000 * log10(fGain));
}

SLpermille MCOpenSLSourceManager::floatToPermille(float fPanning)
{
   return (SLpermille)(1000 * fPanning);
}



//
//  Sound generator
//
unsigned int MCSoundGenOpenSL::iNumberOfInstances = 0;

MCSoundGenOpenSL::MCSoundGenOpenSL(unsigned int ID, unsigned int NumberOfChannels, bool Sound3D)
    : MCSoundGenAudioMultipleChannel(ID, NumberOfChannels, Sound3D)
    , slBuffers(0)
{
    if(!cManager)
    {
        cManager = new MCOpenSLSourceManager(OPENSL_SOURCES);
        cManager->allocateSources();
    }
     
    iNumberOfInstances++;
}

MCSoundGenOpenSL::~MCSoundGenOpenSL()
{
    unloadSamples();

    iNumberOfInstances--;

    if(iNumberOfInstances == 0)
    {
        cManager->releaseSources();
        delete cManager;
        cManager = 0;
    }
}

void MCSoundGenOpenSL::loadSamples()
{
    if(sSampleSet.empty())
        return;

    char sFilename[512];
    unsigned int i, j;

    slBuffers = new MSAudioBuffer*[sSampleSet.size()];

    for(i = 0; i < sSampleSet.size(); i++)
    {
        slBuffers[i] = new MSAudioBuffer[iNumberOfSamples[i]];

        for(j = 0; j < iNumberOfSamples[i]; j++)
        {
            sprintf(sFilename, "%s%02d_%03d.%s", sSampleSet[i].Path, 
                                                 sSampleSet[i].ID, 
                                                 j + sSampleSet[i].Range.LowestNote,
                                                 sSampleSet[i].Format);
            strtolower(sSampleSet[i].Format);
            slBuffers[i][j].FileDescriptor = getFileDescriptor(sFilename, &slBuffers[i][j].Start, &slBuffers[i][j].Length);
        }
    }
}

void MCSoundGenOpenSL::loadSamplePack(std::istream& Stream, 
                                      void (*callback)(unsigned int TotalSamples, unsigned int Loaded, void* Data),
                                      void* Data)
{
}

void MCSoundGenOpenSL::unloadSamples()
{
    if(!slBuffers)
        return;

    for(unsigned int i = 0; i < sSampleSet.size(); i++)
        delete[] slBuffers[i];

    sSampleSet.clear();
    iNumberOfSamples.clear();

    delete[] slBuffers;
    slBuffers = NULL;
}

void MCSoundGenOpenSL::playAudioSample(unsigned int SourceIndex, int SampleSet, int SampleIndex)
{
    cManager->playSource(SourceIndex, &slBuffers[SampleSet][SampleIndex], b3DSound);
}
