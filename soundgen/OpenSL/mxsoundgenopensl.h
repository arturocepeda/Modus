
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

#ifndef _MXSOUNDGENOPENSL_H_
#define _MXSOUNDGENOPENSL_H_

#include "msoundgen.h"
#include "mthreads.h"
#include <vector>

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

#define OPENSL_SOURCES 24
#undef  MSOUNDGEN_STREAMING


//
//  Ogg Vorbis
//
#include "Externals/Tremor/ivorbisfile.h"

#define OV_BUFFER_SIZE 4096

namespace Modus
{
   struct MSOGGFile
   {
       char* Data;
       unsigned int Size;
       unsigned int Cursor;
   };

   size_t ovRead(void* pDestination, size_t iSize, size_t iNumMembers, void* pDataSource);
   int ovSeek(void* pDataSource, ogg_int64_t iOffset, int iWhence);
   long ovTell(void* pDataSource);
   int ovClose(void* pDataSource);


   //
   //  Audio buffer
   //
   struct MSAudioBuffer
   {
      char* Data;
      unsigned int Size;

      MSAudioBuffer() : Data(0), Size(0) {}
   };


   //
   //  Audio source manager
   //
   class MCOpenSLSourceManager : public MCAudioSourceManager
   {
   private:
      SLObjectItf slEngineObject;
      SLEngineItf slEngine;
      SLObjectItf slOutputMix;

      struct OpenSLSource
      {
         SLObjectItf AudioPlayer;
         SLBufferQueueItf BufferQueue;
         SLPlayItf PlaybackState;
         SLVolumeItf VolumeController;
      };

      OpenSLSource* slSources;

      SLmillibel linearToMillibel(float fGain);
      SLpermille floatToPermille(float fPanning);

   public:
      MCOpenSLSourceManager(unsigned int NumSources);
      ~MCOpenSLSourceManager();

      virtual void allocateSources() override;
      virtual void releaseSources() override;

      virtual void playSource(unsigned int SourceIndex, void* Sound, bool Sound3D) override;
      virtual void stopSource(unsigned int SourceIndex) override;

      virtual bool isSourcePlaying(unsigned int SourceIndex) override;

      virtual void setSourceVolume(unsigned int SourceIndex, float Volume) override;
      virtual void setSourcePitch(unsigned int SourceIndex, int Cents) override;
      virtual void setSourcePan(unsigned int SourceIndex, float Pan) override;
      virtual void setSourcePosition(unsigned int SourceIndex, float X, float Y, float Z) override;
      virtual void setSourceDirection(unsigned int SourceIndex, float X, float Y, float Z) override;
   };



   //
   //  Sound generator
   //
   class MCSoundGenOpenSL : public MCSoundGenAudioMultipleChannel
   {
   private:
       MSAudioBuffer** slBuffers;
       unsigned int iSamplesLoaded;
       MSemaphore mSemaphore;
       MMutex mMutex;

       static unsigned int iNumberOfInstances;
       static unsigned int iFrameBufferSize;

       static MThreadFunction(sampleLoadThread);
       static void loadWAVData(const char* sData, unsigned int iSize, MSAudioBuffer* slBuffer);

   protected:
       void playAudioSample(unsigned int SourceIndex, int SampleSet, int SampleIndex);

   public:
       MCSoundGenOpenSL(unsigned int ID, unsigned int NumberOfChannels, bool Sound3D);
       ~MCSoundGenOpenSL();
    
       static void setFrameBufferSize(unsigned int FrameBufferSize);
    
       void addSampleSet(MSSampleSet& SampleSet);
       void loadSamples();
       void loadSamplePack(std::istream& Stream, 
                           void (*callback)(unsigned int TotalSamples, unsigned int Loaded, void* Data) = NULL,
                           void* Data = NULL);
       void unloadSamples();
   };


   //
   //  Sample loading data
   //
   struct SSampleLoadData
   {
      char* SampleData;
      unsigned int SampleDataSize;
      MSAudioBuffer* AudioBuffer;

      MCSoundGenOpenSL* SoundGenInstance;
      unsigned int TotalSamples;
      void (*Callback)(unsigned int TotalSamples, unsigned int Loaded, void* Data);
      void* CallbackData;
   };
}

#endif
