
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
#include <cpu-features.h>


//
//  Ogg Vorbis
//
size_t ovRead(void* pDestination, size_t iSize, size_t iNumMembers, void* pDataSource)
{
    SOGGFile* sFile = (SOGGFile*)pDataSource;

    // calculate number of bytes to read
    unsigned int iBytesToRead = std::min((unsigned int)(iSize * iNumMembers), sFile->Size - sFile->Cursor);

    // read the data
    memcpy(pDestination, sFile->Data + sFile->Cursor, iBytesToRead);
    sFile->Cursor += iBytesToRead;

    return iBytesToRead;
}

int ovSeek(void* pDataSource, ogg_int64_t iOffset, int iWhence)
{
    SOGGFile* sFile = (SOGGFile*)pDataSource;

    switch(iWhence)
    {
    case SEEK_SET:
        sFile->Cursor = std::min((unsigned int)iOffset, sFile->Size);
        break;

    case SEEK_CUR:
        sFile->Cursor = std::min(sFile->Cursor + (unsigned int)iOffset, sFile->Size);
        break;

    case SEEK_END:
        sFile->Cursor = sFile->Size;
        break;

    default:
        return -1;
    }

    return 0;
}

long ovTell(void* pDataSource)
{
    SOGGFile* sFile = (SOGGFile*)pDataSource;
    return sFile->Cursor;
}

int ovClose(void* pDataSource)
{
    return 0;
}



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
   // define format
   SLDataLocator_AndroidSimpleBufferQueue slDataLocatorIn;
   slDataLocatorIn.locatorType = SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE;
   slDataLocatorIn.numBuffers = 1;

   SLDataFormat_PCM slDataFormat;
   slDataFormat.formatType = SL_DATAFORMAT_PCM;
   slDataFormat.numChannels = 1;
   slDataFormat.samplesPerSec = SL_SAMPLINGRATE_44_1;
   slDataFormat.bitsPerSample = SL_PCMSAMPLEFORMAT_FIXED_16;
   slDataFormat.containerSize = SL_PCMSAMPLEFORMAT_FIXED_16;
   slDataFormat.channelMask = SL_SPEAKER_FRONT_CENTER;
   slDataFormat.endianness = SL_BYTEORDER_LITTLEENDIAN;

   SLDataSource slDataSource;
   slDataSource.pLocator = &slDataLocatorIn;
   slDataSource.pFormat = &slDataFormat;

   SLDataLocator_OutputMix slDataLocatorOut;
   slDataLocatorOut.locatorType = SL_DATALOCATOR_OUTPUTMIX;
   slDataLocatorOut.outputMix = slOutputMix;

   SLDataSink slDataSink;
   slDataSink.pLocator = &slDataLocatorOut;
   slDataSink.pFormat = NULL;

   // create audio players (sources)
   const SLuint32 slAudioPlayerIdCount = 3;
   const SLInterfaceID slAudioPlayerIds[] = { SL_IID_PLAY, SL_IID_BUFFERQUEUE, SL_IID_VOLUME };
   const SLboolean slAudioPlayerReq[] = { SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE };

   slSources = new OpenSLSource[iNumSources];

   for(unsigned int i = 0; i < iNumSources; i++)
   {
      (*slEngine)->CreateAudioPlayer(slEngine, &slSources[i].AudioPlayer, &slDataSource, &slDataSink, slAudioPlayerIdCount, slAudioPlayerIds, slAudioPlayerReq);
      (*slSources[i].AudioPlayer)->Realize(slSources[i].AudioPlayer, SL_BOOLEAN_FALSE);

      (*slSources[i].AudioPlayer)->GetInterface(slSources[i].AudioPlayer, SL_IID_PLAY, &slSources[i].PlaybackState);
      (*slSources[i].AudioPlayer)->GetInterface(slSources[i].AudioPlayer, SL_IID_BUFFERQUEUE, &slSources[i].BufferQueue);
      (*slSources[i].AudioPlayer)->GetInterface(slSources[i].AudioPlayer, SL_IID_VOLUME, &slSources[i].VolumeController);

      (*slSources[i].BufferQueue)->Enqueue(slSources[i].BufferQueue, 0, 0);

      (*slSources[i].VolumeController)->EnableStereoPosition(slSources[i].VolumeController, true);
      (*slSources[i].VolumeController)->SetVolumeLevel(slSources[i].VolumeController, 0.0f);
      (*slSources[i].VolumeController)->SetStereoPosition(slSources[i].VolumeController, 0.0f);
   }
}

void MCOpenSLSourceManager::releaseSources()
{
   for(unsigned int i = 0; i < iNumSources; i++)
      (*slSources[i].AudioPlayer)->Destroy(slSources[i].AudioPlayer);

   delete[] slSources;
}

void MCOpenSLSourceManager::playSource(unsigned int SourceIndex, void* Sound, bool Sound3D)
{
   SAudioBuffer* slBuffer = reinterpret_cast<SAudioBuffer*>(Sound);

   (*slSources[SourceIndex].BufferQueue)->Clear(slSources[SourceIndex].BufferQueue);
   (*slSources[SourceIndex].BufferQueue)->Enqueue(slSources[SourceIndex].BufferQueue, slBuffer->Data, slBuffer->Size);
   (*slSources[SourceIndex].PlaybackState)->SetPlayState(slSources[SourceIndex].PlaybackState, SL_PLAYSTATE_PLAYING);
}

void MCOpenSLSourceManager::stopSource(unsigned int SourceIndex)
{
   (*slSources[SourceIndex].PlaybackState)->SetPlayState(slSources[SourceIndex].PlaybackState, SL_PLAYSTATE_STOPPED);
}

bool MCOpenSLSourceManager::isSourcePlaying(unsigned int SourceIndex)
{
   SLuint32 iPlaybackState = 0;
   (*slSources[SourceIndex].PlaybackState)->GetPlayState(slSources[SourceIndex].PlaybackState, &iPlaybackState);
   return iPlaybackState == SL_PLAYSTATE_PLAYING;
}

void MCOpenSLSourceManager::setSourceVolume(unsigned int SourceIndex, float Volume)
{
   sSources[SourceIndex].Volume = Volume;

   (*slSources[SourceIndex].VolumeController)->SetVolumeLevel(slSources[SourceIndex].VolumeController, linearToMillibel(Volume));
}

void MCOpenSLSourceManager::setSourcePitch(unsigned int SourceIndex, int Cents)
{
   //TODO
}

void MCOpenSLSourceManager::setSourcePan(unsigned int SourceIndex, float Pan)
{
   //TODO
}

void MCOpenSLSourceManager::setSourcePosition(unsigned int SourceIndex, float X, float Y, float Z)
{
    sSources[SourceIndex].Position[0] = X;
    sSources[SourceIndex].Position[1] = Y;
    sSources[SourceIndex].Position[2] = Z;

    //TODO
}

void MCOpenSLSourceManager::setSourceDirection(unsigned int SourceIndex, float X, float Y, float Z)
{
    sSources[SourceIndex].Direction[0] = X;
    sSources[SourceIndex].Direction[1] = Y;
    sSources[SourceIndex].Direction[2] = Z;

    //TODO
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
    int iCoresCount = android_getCpuCount();

    MInitMutex(mMutex);
    MInitSemaphore(mSemaphore, iCoresCount);

    if(!cManager)
    {
        cManager = new MCOpenSLSourceManager(OPENSL_SOURCES);
        cManager->allocateSources();
        iSamplesLoaded = 0;
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
}

MThreadFunction(MCSoundGenOpenSL::sampleLoadThread)
{
   SSampleLoadData* sData = static_cast<SSampleLoadData*>(pData);

   MWaitSemaphore(sData->SoundGenInstance->mSemaphore);
   loadOGGData(sData->SampleData, sData->SampleDataSize, sData->AudioBuffer);
   delete[] sData->SampleData;
   MPostSemaphore(sData->SoundGenInstance->mSemaphore);

   MLockMutex(sData->SoundGenInstance->mMutex);
   if(sData->Callback)
      sData->Callback(sData->TotalSamples, ++sData->SoundGenInstance->iSamplesLoaded, sData->CallbackData);
   MUnlockMutex(sData->SoundGenInstance->mMutex);

   return 0;
}

void MCSoundGenOpenSL::loadSamplePack(std::istream& Stream, 
                                      void (*callback)(unsigned int TotalSamples, unsigned int Loaded, void* Data),
                                      void* Data)
{
    unsigned int iTotalSamples = 0;

    iSamplesLoaded = 0;

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

    slBuffers = new SAudioBuffer*[iNumSampleSets];

    MThread** mSampleLoadThreads = new MThread*[iNumSampleSets];
    SSampleLoadData** sSampleLoadData = new SSampleLoadData*[iNumSampleSets];

    for(i = 0; i < iNumSampleSets; i++)
    {
        // sample set
        strcpy(mSampleSet.Path, "");
        strcpy(mSampleSet.Format, sFormat);

        Stream.read((char*)&iInput, sizeof(unsigned int));         // SampleSet: ID
        mSampleSet.ID = (unsigned char)iInput;

        Stream.read((char*)&iInput, sizeof(unsigned int));         // SampleSet: Mode
        mSampleSet.Mode = (unsigned char)iInput;

        Stream.read((char*)&iInput, sizeof(unsigned int));         // SampleSet: Minimum intensity
        mSampleSet.MinimumIntensity = (unsigned char)iInput;

        Stream.read((char*)&iInput, sizeof(unsigned int));         // SampleSet: Maximum intensity
        mSampleSet.MaximumIntensity = (unsigned char)iInput;

        Stream.read((char*)&iInput, sizeof(unsigned int));         // SampleSet: Lowest pitch
        mSampleSet.Range.LowestNote = (unsigned char)iInput;

        Stream.read((char*)&iInput, sizeof(unsigned int));         // SampleSet: Highest pitch
        mSampleSet.Range.HighestNote = (unsigned char)iInput;

        sSampleSet.push_back(mSampleSet);

        // samples inside the sample set
        Stream.read((char*)&iNumSamples, sizeof(unsigned int));    // SampleSet: Number of samples
        iNumberOfSamples.push_back((unsigned char)iNumSamples);

        Stream.read(sReserved, 4);                                 // SampleSet: (Reserved)

        slBuffers[i] = new SAudioBuffer[iNumSamples];
        mSampleLoadThreads[i] = new MThread[iNumSamples];
        sSampleLoadData[i] = new SSampleLoadData[iNumSamples];

        unsigned int iSampleSize;
        char *sSampleData;

        for(j = 0; j < iNumSamples; j++)
        {
            Stream.read((char*)&iSampleSize, sizeof(unsigned int));

            if(iSampleSize == 0)
                continue;

            sSampleData = new char[iSampleSize];
            Stream.read(sSampleData, iSampleSize);

            sSampleLoadData[i][j].SampleData = sSampleData;
            sSampleLoadData[i][j].SampleDataSize = iSampleSize;
            sSampleLoadData[i][j].AudioBuffer = &slBuffers[i][j];
            sSampleLoadData[i][j].SoundGenInstance = this;
            sSampleLoadData[i][j].TotalSamples = iTotalSamples;
            sSampleLoadData[i][j].Callback = callback;
            sSampleLoadData[i][j].CallbackData = Data;

            MCreateThread(mSampleLoadThreads[i][j], sampleLoadThread, &sSampleLoadData[i][j]);
        }
    }

    for(i = 0; i < iNumSampleSets; i++)
    {
        for(j = 0; j < iNumSamples; j++)
        {
           MWaitForThread(mSampleLoadThreads[i][j]);
           MCloseThread(mSampleLoadThreads[i][j]);
        }

        delete[] mSampleLoadThreads[i];
        delete[] sSampleLoadData[i];
    }

    delete[] mSampleLoadThreads;
    delete[] sSampleLoadData;
}

void MCSoundGenOpenSL::unloadSamples()
{
    if(!slBuffers)
        return;

    for(unsigned int i = 0; i < sSampleSet.size(); i++)
    {
        for(unsigned int j = 0; j < iNumberOfSamples[i]; j++)
            delete[] slBuffers[i][j].Data;

        delete[] slBuffers[i];
    }

    sSampleSet.clear();
    iNumberOfSamples.clear();

    delete[] slBuffers;
    slBuffers = NULL;
}

void MCSoundGenOpenSL::playAudioSample(unsigned int SourceIndex, int SampleSet, int SampleIndex)
{
    cManager->playSource(SourceIndex, &slBuffers[SampleSet][SampleIndex], b3DSound);
}

void MCSoundGenOpenSL::loadOGGData(const char* sData, unsigned int iSize, SAudioBuffer* slBuffer)
{
    SOGGFile sFile;
    OggVorbis_File ovFile;
    ov_callbacks ovCallbacks;

    // audio file info
    sFile.Data = (char*)sData;
    sFile.Size = iSize;
    sFile.Cursor = 0;

    // set our functions to handle Vorbis OGG data
    ovCallbacks.read_func = ovRead;
    ovCallbacks.seek_func = ovSeek;
    ovCallbacks.tell_func = ovTell;
    ovCallbacks.close_func = ovClose;

    // attach audio file data with the ovFile struct
    ov_open_callbacks(&sFile, &ovFile, NULL, 0, ovCallbacks);

    // get info about the audio data
    vorbis_info* pVorbisInfo = ov_info(&ovFile, -1);
    unsigned int iPCMSamplesCount = (unsigned int)ov_pcm_total(&ovFile, -1);

    // decode data
    int iBitStream;
    int iReadedBytes;

    slBuffer->Size = iPCMSamplesCount * pVorbisInfo->channels * sizeof(short);
    slBuffer->Data = new char[slBuffer->Size];

    unsigned int iCurrentPosition = 0;

    do
    {
        iReadedBytes = ov_read(&ovFile, slBuffer->Data + iCurrentPosition, OV_BUFFER_SIZE, 0);
        iCurrentPosition += iReadedBytes;

    } while(iReadedBytes > 0);

    ov_clear(&ovFile);
}
