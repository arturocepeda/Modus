
////////////////////////////////////////////////////////////////////////
//
//  Modus
//  C++ Music Library
//  [Sound Generator]
//
//  Copyright (c) 2012-2013 Arturo Cepeda
//
//  --------------------------------------------------------------------
//
//  Audio engine: 
//
//  irrKlang
//  Copyright (c) Nikolaus Gebhardt / Ambiera 2001-2009  
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

#ifndef _MXSOUNDGENIRRKLANG_H_
#define _MXSOUNDGENIRRKLANG_H_

#include "msoundgen.h"
#include "./../externals/irrKlang/include/irrKlang.h"

#define IRRKLANG_EFFECT_PARAMETERS 12

enum IRRKLANG_EFFECT
{
    IRRKLANG_EFFECT_CHORUS,
    IRRKLANG_EFFECT_COMPRESSOR,
    IRRKLANG_EFFECT_DISTORTION,
    IRRKLANG_EFFECT_ECHO,
    IRRKLANG_EFFECT_FLANGER,
    IRRKLANG_EFFECT_GARGLE,
    IRRKLANG_EFFECT_I3DL2REVERB,
    IRRKLANG_EFFECT_PARAMEQ,
    IRRKLANG_EFFECT_WAVESREVERB
};

class MCSoundGenirrKlang : public MCSoundGenAudioDoubleChannel
{
private:
    irrklang::ISoundEngine* iEngine;
    irrklang::ISoundSource*** iSound;
    irrklang::ISound*** iChannel;
    irrklang::ISoundEffectControl** iSoundEffectControl;
    std::vector<IRRKLANG_EFFECT> iEffect;
    std::vector<irrklang::ik_f32*> iEffectParameter;

    irrklang::vec3df i3DPosition;

    void releaseChannel(unsigned char iChannel, bool bQuickly);
    void applyEffects(unsigned char iNumChannel);

public:
    MCSoundGenirrKlang(unsigned int NumberOfChannels, bool Sound3D, irrklang::ISoundEngine* Engine);
    ~MCSoundGenirrKlang();

    void addSampleSet(MSSampleSet& SampleSet);
    unsigned int addEffect(IRRKLANG_EFFECT Effect);

    void loadSamples();
    void loadSamplePack(std::istream& Stream, 
                        void (*callback)(unsigned int TotalSamples, unsigned int Loaded, void* Data) = NULL,
                        void* Data = NULL);
    void unloadSamples();

    void playNote(MSNote& Note);
    void update();

    void get3DPosition(float* X, float* Y, float* Z);

    void setBending(unsigned char Channel, int Cents);
    void setIntensity(unsigned char Channel, unsigned char Intensity);
    void set3DPosition(float X, float Y, float Z);
    void setEffectParameter(unsigned int EffectIndex, unsigned int ParameterIndex, float Value);
};

#endif
