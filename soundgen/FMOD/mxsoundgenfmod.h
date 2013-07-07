
////////////////////////////////////////////////////////////////////////
//
//  Modus v0.51
//  C++ Music Library
//  [Sound Generator]
//
//  Copyright (c) 2012 Arturo Cepeda
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

#ifndef _MXSOUNDGENFMOD_H_
#define _MXSOUNDGENFMOD_H_

#include "msoundgen.h"
#include "./../externals/FMOD/include/fmod.hpp"

class MCSoundGenFMOD : public MCSoundGenAudioDoubleChannel
{
private:
    FMOD::System* fmodSystem;
    FMOD::Sound*** fmodSound;
    FMOD::Channel*** fmodChannel;
    FMOD::ChannelGroup* fmodChannelGroup;
    std::vector<FMOD::DSP*> fmodDSP;

    float* fFrequency;
    FMOD_VECTOR fmod3DPosition;

    void releaseChannel(unsigned char iChannel, bool bQuickly);

public:
    MCSoundGenFMOD(unsigned int NumberOfChannels, bool Sound3D, FMOD::System* FMODSystem);
    ~MCSoundGenFMOD();

    void addSampleSet(MSSampleSet& SampleSet);
    unsigned int addEffect(FMOD_DSP_TYPE fmodDSPType);
    void loadSamples();
    void loadSamplePack(const char* Filename, 
                        void (*callback)(unsigned int TotalSamples, unsigned int Loaded, void* Data) = NULL,
                        void* Data = NULL);
    void unloadSamples();

    void playNote(MSNote& Note);
    void update();

    void get3DPosition(float* X, float* Y, float* Z);

    void setBending(unsigned char Channel, int Cents);
    void setIntensity(unsigned char Channel, unsigned char Intensity);
    void set3DPosition(float X, float Y, float Z);
    void setEffectParameter(unsigned int EffectIndex, int ParameterIndex, float Value);
};

#endif
