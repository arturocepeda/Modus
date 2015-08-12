
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

#ifndef _MXSOUNDGENXAUDIO2_H_
#define _MXSOUNDGENXAUDIO2_H_

#include "msoundgen.h"
#include <xaudio2.h>

#define MODUS_XAUDIO2_SOURCES 32


namespace Modus
{
    class MCSourceVoiceCallback;
    

    struct MSSourceVoice
    {
        IXAudio2SourceVoice* Handle;
        MCSourceVoiceCallback* Callback;
        bool Playing;

        MSSourceVoice() : Handle(0), Callback(0), Playing(false) {}
    };


    class MCSourceVoiceCallback : public IXAudio2VoiceCallback
    {
    private:
        MSSourceVoice& sSourceVoiceRef;

    public:
        MCSourceVoiceCallback(MSSourceVoice& SourceVoiceRef);
        ~MCSourceVoiceCallback();

        void OnStreamEnd();
        void OnVoiceProcessingPassEnd();
        void OnVoiceProcessingPassStart(UINT32 SamplesRequired);
        void OnBufferEnd(void* pBufferContext);
        void OnBufferStart(void* pBufferContext);
        void OnLoopEnd(void* pBufferContext);
        void OnVoiceError(void* pBufferContext, HRESULT Error);
    };


    //
    //  Audio source manager
    //
    class MCXAudio2SourceManager : public MCAudioSourceManager
    {
    private:
        IXAudio2* xaEngine;
        MSSourceVoice* xaSourceVoices;

        float* fFrequencies;

    public:
        MCXAudio2SourceManager(IXAudio2* Engine, unsigned int NumSources);
        ~MCXAudio2SourceManager();

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
    class MCSoundGenXAudio2 : public MCSoundGenAudioMultipleChannel
    {
    private:
       XAUDIO2_BUFFER* xaBuffers;

       float* fFrequency;

       static unsigned int iNumberOfInstances;

       void releaseChannel(unsigned char iChannel, bool bQuickly);

    public:
       MCSoundGenXAudio2(unsigned int ID, unsigned int NumberOfChannels, bool Sound3D, IXAudio2* Engine);
       ~MCSoundGenXAudio2();

       void addSampleSet(MSSampleSet& SampleSet);
       void loadSamples();
       void loadSamplePack(std::istream& Stream, 
                           void (*callback)(unsigned int TotalSamples, unsigned int Loaded, void* Data) = NULL,
                           void* Data = NULL);
       void unloadSamples();

       void playAudioSample(unsigned int SourceIndex, int SampleSet, int SampleIndex);
    };
}

#endif
