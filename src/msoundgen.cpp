
////////////////////////////////////////////////////////////////////////
//
//  Modus v0.54
//  C++ Music Library
//
//  Copyright (c) 2012-2014 Arturo Cepeda Pérez
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

#include "msoundgen.h"
#include "mutils.h"
#include <fstream>

//
//  MCSoundGen
//
void MCSoundGen::releaseDamper()
{
    while(!vChannelsSustained.empty())
    {
        releaseNote(vChannelsSustained[vChannelsSustained.size() - 1]);
        vChannelsSustained.removeLast();
    }

    // in case the derived class implements some kind of resonance when the damper is used, we must
    // release it
    releaseResonance();
}

void MCSoundGen::releaseAllNotes()
{
    bDamper = false;
    vChannelsSustained.clear();

    for(unsigned char i = 0; i < iNumberOfChannels; i++)
        releaseNote(i);
}

void MCSoundGen::setVolume(float Volume)
{
    if(Volume >= 0.0f && Volume <= 1.0f)
        fVolume = Volume;
}

void MCSoundGen::setPan(float Pan)
{
    if(Pan >= -1.0f && Pan <= 1.0f)
        fPan = Pan;
}

void MCSoundGen::setReleaseSpeed(float ReleaseSpeed)
{
    if(ReleaseSpeed > 0.0f)
        fReleaseSpeed = ReleaseSpeed;
}

void MCSoundGen::setDamper(bool DamperState)
{
    bDamper = DamperState;

    // release sustained notes
    if(!bDamper)
        releaseDamper();
}


//
//  MCSoundGenAudio
//
int MCSoundGenAudio::findSampleSet(MSNote& Note)
{
    for(unsigned int i = 0; i < sSampleSet.size(); i++)
    {
        if(sSampleSet[i].Mode == Note.Mode && 
           sSampleSet[i].Range.LowestNote <= Note.Pitch &&
           sSampleSet[i].Range.HighestNote >= Note.Pitch &&
           sSampleSet[i].MinimumIntensity <= Note.Intensity && 
           sSampleSet[i].MaximumIntensity >= Note.Intensity)
        {
            return i;
        }
    }

    return -1;
}

float MCSoundGenAudio::calculateNoteVolume(unsigned char iIntensity, int iSampleSet)
{
    float fNoteVolume = fVolume;
    double fSampleSetMinVolume = (double)sSampleSet[iSampleSet].MinimumIntensity / 127.0;

    CLine cLine((double)sSampleSet[iSampleSet].MinimumIntensity, fSampleSetMinVolume, 
                (double)sSampleSet[iSampleSet].MaximumIntensity, 1.0);

    if(cLine.can_calculate_y())
        fNoteVolume *= (float)cLine.y((double)iIntensity);

    return fNoteVolume;
}

void MCSoundGenAudio::addSampleSet(MSSampleSet& SampleSet)
{
    int iSamples = SampleSet.Range.getSize();

    if(iSamples <= 0)
        return;

    sSampleSet.push_back(SampleSet);
    iNumberOfSamples.push_back(iSamples);
}

bool MCSoundGenAudio::loadSamplePack(const char* Filename, 
                                     void (*callback)(unsigned int TotalSamples, unsigned int Loaded, void* Data),
                                     void* Data)
{
    std::ifstream fSamplePack(Filename, std::ios::in | std::ios::binary);

    if(!fSamplePack.is_open())
        return false;

    loadSamplePack(fSamplePack, callback, Data);
    fSamplePack.close();

    return true;
}



//
//  MCSoundGenAudioDoubleChannel
//
void MCSoundGenAudioDoubleChannel::initChannelData()
{
    bRelease = new bool[iNumberOfChannels];
    bQuickRelease = new bool[iNumberOfChannels];

    memset(bRelease, 0, iNumberOfChannels);
    memset(bQuickRelease, 0, iNumberOfChannels);

    fInitialReleaseVolume = new float[iNumberOfChannels];
    fCurrentReleaseVolume = new float[iNumberOfChannels];
}

void MCSoundGenAudioDoubleChannel::releaseChannelData()
{
    delete[] bRelease;
    delete[] bQuickRelease;
    delete[] fInitialReleaseVolume;
    delete[] fCurrentReleaseVolume;
}

void MCSoundGenAudioDoubleChannel::releaseResonance()
{
    while(!vSustainedChannelsToRelease.empty())
    {
        vChannelsToRelease.add(vSustainedChannelsToRelease[vSustainedChannelsToRelease.size() - 1]);
        vSustainedChannelsToRelease.removeLast();
    }
}

void MCSoundGenAudioDoubleChannel::releaseNote(unsigned char Channel)
{
    // damper on: don't release the note, but put it in the sustained list
    if(bDamper)
        vChannelsSustained.add(Channel);
    else
        releaseChannel(Channel, false);
}
