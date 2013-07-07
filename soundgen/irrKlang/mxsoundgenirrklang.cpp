
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

#include "mxsoundgenirrklang.h"
#include <fstream>
#include <cmath>
#include "mutils.h"

MCSoundGenirrKlang::MCSoundGenirrKlang(unsigned int NumberOfChannels, bool Sound3D, irrklang::ISoundEngine* Engine)
{
    iEngine = Engine;
    iNumberOfChannels = NumberOfChannels;
    iSound = NULL;
    b3DSound = Sound3D;
    fVolume = 1.0f;
    fPan = 0.0f;
    fReleaseSpeed = M_DEFAULT_RELEASE;
    bDamper = false;

    i3DPosition.X = 0.0f;
    i3DPosition.Y = 0.0f;
    i3DPosition.Z = 0.0f;

    iChannel = new irrklang::ISound**[iNumberOfChannels];
    iSoundEffectControl = new irrklang::ISoundEffectControl*[iNumberOfChannels];
    iSampleSet = new int[iNumberOfChannels];

    for(unsigned int i = 0; i < iNumberOfChannels; i++)
    {
        iChannel[i] = new irrklang::ISound*[2];

        iChannel[i][0] = NULL;
        iChannel[i][1] = NULL;

        iSoundEffectControl[i] = NULL;
        iSampleSet[i] = -1;
    }

    initChannelData();
}

MCSoundGenirrKlang::~MCSoundGenirrKlang()
{
    unsigned int i;
    unsigned int j;

    for(i = 0; i < iEffectParameter.size(); i++)
        delete[] iEffectParameter[i];

    for(i = 0; i < iNumberOfChannels; i++)
    {
        for(j = 0; j < 2; j++)
        {
            if(iChannel[i][j])
                iChannel[i][j]->drop();
        }

        delete[] iChannel[i];
    }

    delete[] iChannel;
    delete[] iSoundEffectControl;
    delete[] iSampleSet;

    unloadSamples();
    releaseChannelData();
}

unsigned int MCSoundGenirrKlang::addEffect(IRRKLANG_EFFECT Effect)
{
    unsigned int i;

    for(i = 0; i < iEffect.size(); i++)
    {
        if(iEffect[i] == Effect)
            return i;
    }

    // effect
    iEffect.push_back(Effect);
    
    // parameters array with their default values
    irrklang::ik_f32 *fParam;
    unsigned int iIndex;

    iEffectParameter.push_back(fParam);
    iIndex = iEffectParameter.size() - 1;

    iEffectParameter[iIndex] = new irrklang::ik_f32[IRRKLANG_EFFECT_PARAMETERS];

    switch(Effect)
    {
    case IRRKLANG_EFFECT_CHORUS:
        iEffectParameter[iIndex][0] = 50;
        iEffectParameter[iIndex][1] = 10;
        iEffectParameter[iIndex][2] = 25;
        iEffectParameter[iIndex][3] = 1.1f;
        iEffectParameter[iIndex][4] = true;
        iEffectParameter[iIndex][5] = 16;
        iEffectParameter[iIndex][6] = 90;
        break;

    case IRRKLANG_EFFECT_COMPRESSOR:
        iEffectParameter[iIndex][0] = 0;
        iEffectParameter[iIndex][1] = 10;
        iEffectParameter[iIndex][2] = 200;
        iEffectParameter[iIndex][3] = -20;
        iEffectParameter[iIndex][4] = 3;
        iEffectParameter[iIndex][5] = 4;
        break;

    case IRRKLANG_EFFECT_DISTORTION:
        iEffectParameter[iIndex][0] = -18;
        iEffectParameter[iIndex][1] = 15;
        iEffectParameter[iIndex][2] = 2400;
        iEffectParameter[iIndex][3] = 2400;
        iEffectParameter[iIndex][4] = 8000;
        break;

    case IRRKLANG_EFFECT_ECHO:
        iEffectParameter[iIndex][0] = 50;
        iEffectParameter[iIndex][1] = 50;
        iEffectParameter[iIndex][2] = 500;
        iEffectParameter[iIndex][3] = 500;
        iEffectParameter[iIndex][4] = 0;
        break;

    case IRRKLANG_EFFECT_FLANGER:
        iEffectParameter[iIndex][0] = 50;
        iEffectParameter[iIndex][1] = 100;
        iEffectParameter[iIndex][2] = -50;
        iEffectParameter[iIndex][3] = 0.25f;
        iEffectParameter[iIndex][4] = true;
        iEffectParameter[iIndex][5] = 2;
        iEffectParameter[iIndex][6] = 0;
        break;

    case IRRKLANG_EFFECT_GARGLE:
        iEffectParameter[iIndex][0] = 20;
        iEffectParameter[iIndex][1] = true;
        break;

    case IRRKLANG_EFFECT_I3DL2REVERB:
        iEffectParameter[iIndex][0] = -1000;
        iEffectParameter[iIndex][1] = -100;
        iEffectParameter[iIndex][2] = 0;
        iEffectParameter[iIndex][3] = 1.49f;
        iEffectParameter[iIndex][4] = 0.83f;
        iEffectParameter[iIndex][5] = -2602;
        iEffectParameter[iIndex][6] = 0.007f;
        iEffectParameter[iIndex][7] = 200;
        iEffectParameter[iIndex][8] = 0.011f;
        iEffectParameter[iIndex][9] = 100.0f;
        iEffectParameter[iIndex][10] = 100.0f;
        iEffectParameter[iIndex][11] = 5000.0f;
        break;

    case IRRKLANG_EFFECT_PARAMEQ:
        iEffectParameter[iIndex][0] = 8000;
        iEffectParameter[iIndex][1] = 12;
        iEffectParameter[iIndex][2] = 0;
        break;

    case IRRKLANG_EFFECT_WAVESREVERB:
        iEffectParameter[iIndex][0] = 0;
        iEffectParameter[iIndex][1] = 0;
        iEffectParameter[iIndex][2] = 1000;
        iEffectParameter[iIndex][3] = 0.001f;
        break;
    }
    
    return (iEffect.size() - 1);
}

void MCSoundGenirrKlang::loadSamples()
{
    if(sSampleSet.size() == 0)
        return;

    char sFilename[512];
    unsigned int i, j;

    iSound = new irrklang::ISoundSource**[sSampleSet.size()];

    for(i = 0; i < sSampleSet.size(); i++)
    {
        iSound[i] = new irrklang::ISoundSource*[iNumberOfSamples[i]];

        for(j = 0; j < iNumberOfSamples[i]; j++)
        {
            sprintf(sFilename, "%s%02d_%03d.%s", sSampleSet[i].Path, 
                                                 sSampleSet[i].ID, 
                                                 j + sSampleSet[i].Range.LowestNote,
                                                 sSampleSet[i].Format);

            iSound[i][j] = iEngine->addSoundSourceFromFile(sFilename);
        }
    }
}

void MCSoundGenirrKlang::loadSamplePack(const char* Filename, 
                                        void (*callback)(unsigned int TotalSamples, unsigned int Loaded, void* Data),
                                        void* Data)
{
    std::ifstream fSamplePack(Filename, std::ios::in | std::ios::binary);

    if(!fSamplePack.is_open())
        return;

    unsigned int iTotalSamples = 0;
    unsigned int iLoaded = 0;

    // read total number of samples
    if(callback)
    {
        fSamplePack.seekg(-(std::ios::off_type)sizeof(unsigned int), std::ios::end);
        fSamplePack.read((char*)&iTotalSamples, sizeof(unsigned int));
        fSamplePack.seekg(0, std::ios::beg);
    }

    char sVersion[16];
    char sFormat[8];
    char sReserved[4];

    unsigned int iInput;
    unsigned int iNumSampleSets;

    fSamplePack.read(sVersion, 16);                                     // Version
    fSamplePack.read(sFormat, 8);                                       // Format
    fSamplePack.read((char*)&iNumSampleSets, sizeof(unsigned int));     // Number of sample sets
    fSamplePack.read(sReserved, 4);                                     // (Reserved)

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
    char sSoundName[64];

    iSound = new irrklang::ISoundSource**[iNumSampleSets];

    for(i = 0; i < iNumSampleSets; i++)
    {
        // sample set
        strcpy(mSampleSet.Path, Filename);
        strcpy(mSampleSet.Format, sFormat);

        fSamplePack.read((char*)&iInput, sizeof(unsigned int));        // SampleSet: ID
        mSampleSet.ID = (unsigned char)iInput;

        fSamplePack.read((char*)&iInput, sizeof(unsigned int));        // SampleSet: Mode
        mSampleSet.Mode = (unsigned char)iInput;

        fSamplePack.read((char*)&iInput, sizeof(unsigned int));        // SampleSet: Minimum intensity
        mSampleSet.MinimumIntensity = (unsigned char)iInput;

        fSamplePack.read((char*)&iInput, sizeof(unsigned int));        // SampleSet: Maximum intensity
        mSampleSet.MaximumIntensity = (unsigned char)iInput;

        fSamplePack.read((char*)&iInput, sizeof(unsigned int));        // SampleSet: Lowest pitch
        mSampleSet.Range.LowestNote = (unsigned char)iInput;

        fSamplePack.read((char*)&iInput, sizeof(unsigned int));        // SampleSet: Highest pitch
        mSampleSet.Range.HighestNote = (unsigned char)iInput;

        sSampleSet.push_back(mSampleSet);

        // samples inside the sample set
        fSamplePack.read((char*)&iNumSamples, sizeof(unsigned int));   // SampleSet: Number of samples
        iNumberOfSamples.push_back((unsigned char)iNumSamples);

        fSamplePack.read(sReserved, 4);                                // SampleSet: (Reserved)

        iSound[i] = new irrklang::ISoundSource*[iNumSamples];

        unsigned int iSampleSize;
        char* sSampleData;

        for(j = 0; j < iNumSamples; j++)
        {
            fSamplePack.read((char*)&iSampleSize, sizeof(unsigned int));
            iSound[i][j] = NULL;

            if(iSampleSize == 0)
                continue;

            sSampleData = new char[iSampleSize];
            fSamplePack.read(sSampleData, iSampleSize);

            sprintf(sSoundName, "%s%02d%02d", Filename, i, j);
            iSound[i][j] = iEngine->addSoundSourceFromMemory(sSampleData, iSampleSize, sSoundName);

            // callback
            if(callback)
                callback(iTotalSamples, ++iLoaded, Data);

            delete[] sSampleData;
        }
    }

    fSamplePack.close();
}

void MCSoundGenirrKlang::unloadSamples()
{
    if(!iSound)
        return;

    unsigned int i;
    unsigned int j;

    for(i = 0; i < sSampleSet.size(); i++)
    {
        for(j = 0; j < iNumberOfSamples[i]; j++)
            iEngine->removeSoundSource(iSound[i][j]);

        delete[] iSound[i];
    }

    sSampleSet.clear();
    iNumberOfSamples.clear();

    delete[] iSound;
    iSound = NULL;
}

void MCSoundGenirrKlang::playNote(MSNote& Note)
{
    if(!iSound)
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
    if(b3DSound)
    {
        iChannel[Note.Channel][0] = iEngine->play3D(iSound[iSampleSet[Note.Channel]][iSample], i3DPosition, false, true, false, true);

        if(!iChannel[Note.Channel][0])
            return;
    }
    else
    {
        iChannel[Note.Channel][0] = iEngine->play2D(iSound[iSampleSet[Note.Channel]][iSample], false, true, false, true);

        if(!iChannel[Note.Channel][0])
            return;

        iChannel[Note.Channel][0]->setPan(fPan);
    }

    // apply effects
    if(iEffect.size() > 0)
    {
        iSoundEffectControl[Note.Channel] = iChannel[Note.Channel][0]->getSoundEffectControl();
        applyEffects(Note.Channel);
    }

    iChannel[Note.Channel][0]->setVolume(calculateNoteVolume(Note.Intensity, iSampleSet[Note.Channel]));
    iChannel[Note.Channel][0]->setIsPaused(false);
}

void MCSoundGenirrKlang::releaseChannel(unsigned char iNumChannel, bool bQuickly)
{
    if(!iSound || !iChannel[iNumChannel][0])
        return;

    // if the primary channel is playing a sound, change the pointer to the secondary channel and notify
    // the generator to release it
    if(!iChannel[iNumChannel][0]->isFinished())
    {
        // we need two values: the volume of the channel at the moment (initial) and another value that
        // always goes from 1.0f to 0.0f, in order to do a fade out that doesn't depend on the initial
        // volume
        fInitialReleaseVolume[iNumChannel] = iChannel[iNumChannel][0]->getVolume();
        fCurrentReleaseVolume[iNumChannel] = 1.0f;

        // if the secondary channel is already busy, set it free
        if(iChannel[iNumChannel][1])
            iChannel[iNumChannel][1]->stop();

        iChannel[iNumChannel][1] = iChannel[iNumChannel][0];
        iChannel[iNumChannel][0] = NULL;

        bQuickRelease[iNumChannel] = bQuickly;
        bRelease[iNumChannel] = !bQuickly;

        // add the current channel to the list to be released
        vChannelsToRelease.add(iNumChannel);
    }
}

void MCSoundGenirrKlang::applyEffects(unsigned char iNumChannel)
{
    for(unsigned int i = 0; i < iEffect.size(); i++)
    {
        switch(iEffect[i])
        {
        case IRRKLANG_EFFECT_CHORUS:
            iSoundEffectControl[iNumChannel]->enableChorusSoundEffect(iEffectParameter[i][0], 
                                                                      iEffectParameter[i][1],
                                                                      iEffectParameter[i][2], 
                                                                      iEffectParameter[i][3],
                                                                      iEffectParameter[i][4] != 0.0f, 
                                                                      iEffectParameter[i][5],
                                                                      (int)iEffectParameter[i][6]);
            break;

        case IRRKLANG_EFFECT_COMPRESSOR:
            iSoundEffectControl[iNumChannel]->enableCompressorSoundEffect(iEffectParameter[i][0],
                                                                          iEffectParameter[i][1],
                                                                          iEffectParameter[i][2], 
                                                                          iEffectParameter[i][3], 
                                                                          iEffectParameter[i][4], 
                                                                          iEffectParameter[i][5]); 
            break;

        case IRRKLANG_EFFECT_DISTORTION:
            iSoundEffectControl[iNumChannel]->enableDistortionSoundEffect(iEffectParameter[i][0],
                                                                          iEffectParameter[i][1],
                                                                          iEffectParameter[i][2], 
                                                                          iEffectParameter[i][3], 
                                                                          iEffectParameter[i][4]);
            break;

        case IRRKLANG_EFFECT_ECHO:
            iSoundEffectControl[iNumChannel]->enableEchoSoundEffect(iEffectParameter[i][0],
                                                                    iEffectParameter[i][1],
                                                                    iEffectParameter[i][2], 
                                                                    iEffectParameter[i][3], 
                                                                    (int)iEffectParameter[i][4]);
            break;

        case IRRKLANG_EFFECT_FLANGER:
            iSoundEffectControl[iNumChannel]->enableFlangerSoundEffect(iEffectParameter[i][0], 
                                                                       iEffectParameter[i][1],
                                                                       iEffectParameter[i][2], 
                                                                       iEffectParameter[i][3],
                                                                       iEffectParameter[i][4] != 0.0f, 
                                                                       iEffectParameter[i][5],
                                                                       (int)iEffectParameter[i][6]);
            break;

        case IRRKLANG_EFFECT_GARGLE:
            iSoundEffectControl[iNumChannel]->enableGargleSoundEffect((int)iEffectParameter[i][0], 
                                                                      iEffectParameter[i][1] != 0.0f);
            break;

        case IRRKLANG_EFFECT_I3DL2REVERB:
            iSoundEffectControl[iNumChannel]->enableI3DL2ReverbSoundEffect((int)iEffectParameter[i][0],
                                                                           (int)iEffectParameter[i][1],
                                                                           iEffectParameter[i][2], 
                                                                           iEffectParameter[i][3], 
                                                                           iEffectParameter[i][4], 
                                                                           (int)iEffectParameter[i][5],
                                                                           iEffectParameter[i][6],
                                                                           (int)iEffectParameter[i][7],
                                                                           iEffectParameter[i][8],
                                                                           iEffectParameter[i][9],
                                                                           iEffectParameter[i][10],
                                                                           iEffectParameter[i][11]);
            break;

        case IRRKLANG_EFFECT_PARAMEQ:
            iSoundEffectControl[iNumChannel]->enableParamEqSoundEffect(iEffectParameter[i][0],
                                                                       iEffectParameter[i][1],
                                                                       iEffectParameter[i][2]);
            break;

        case IRRKLANG_EFFECT_WAVESREVERB:
            iSoundEffectControl[iNumChannel]->enableWavesReverbSoundEffect(iEffectParameter[i][0],
                                                                           iEffectParameter[i][1],
                                                                           iEffectParameter[i][2], 
                                                                           iEffectParameter[i][3]);
            break;
        }
    }
}

void MCSoundGenirrKlang::update()
{
    unsigned int iNumChannel;

    // a fade out will be done to the secondary channels that must be released
    for(unsigned int i = 0; i < vChannelsToRelease.size(); i++)
    {
        iNumChannel = vChannelsToRelease[i];

        if(!iChannel[iNumChannel][1])
            continue;

        // next step for the fade-out
        if(fCurrentReleaseVolume[iNumChannel] > 0.0f)
        {
            fCurrentReleaseVolume[iNumChannel] -= (bQuickRelease[iNumChannel])? M_QUICK_RELEASE: fReleaseSpeed;
            iChannel[iNumChannel][1]->setVolume(fCurrentReleaseVolume[iNumChannel] * fInitialReleaseVolume[iNumChannel]);

            // check damper
            if(bDamper)
            {
                vSustainedChannelsToRelease.add(iNumChannel);
                vChannelsToRelease.remove(i);
                i--;
            }
        }

        // fade-out completed: stop source
        else
        {
            bRelease[iNumChannel] = false;
            bQuickRelease[iNumChannel] = false;

            iChannel[iNumChannel][1]->stop();
            iChannel[iNumChannel][1]->drop();
            iChannel[iNumChannel][1] = NULL;

            vChannelsToRelease.remove(i);
            i--;
        }
    }
}

void MCSoundGenirrKlang::get3DPosition(float* X, float* Y, float* Z)
{
    *X = i3DPosition.X;
    *Y = i3DPosition.Y;
    *Z = i3DPosition.Z;
}

void MCSoundGenirrKlang::setBending(unsigned char Channel, int Cents)
{
    if(!iEngine)
        return;

    float fFrequencyRatio = (float)pow(2, Cents / 1200.0f);

    if(iChannel[Channel][0])
        iChannel[Channel][0]->setPlaybackSpeed(fFrequencyRatio);
}

void MCSoundGenirrKlang::setIntensity(unsigned char Channel, unsigned char Intensity)
{
    if(iSampleSet[Channel] > -1)
        iChannel[Channel][0]->setVolume(calculateNoteVolume(Intensity, iSampleSet[Channel]));
}

void MCSoundGenirrKlang::set3DPosition(float X, float Y, float Z)
{
    unsigned int i;
    unsigned int j;

    i3DPosition.X = X;
    i3DPosition.Y = Y;
    i3DPosition.Z = Z;

    // take effect on the channels that are sounding
    for(i = 0; i < iNumberOfChannels; i++)
    {
        for(j = 0; j < 2 && iChannel[i][j]; j++)
        {
            if(!iChannel[i][j]->isFinished())
                iChannel[i][j]->setPosition(i3DPosition);
        }
    }
}

void MCSoundGenirrKlang::setEffectParameter(unsigned int EffectIndex, unsigned int ParameterIndex, float Value)
{
    if(EffectIndex < iEffect.size() && ParameterIndex < IRRKLANG_EFFECT_PARAMETERS)
        iEffectParameter[EffectIndex][ParameterIndex] = Value;
}
