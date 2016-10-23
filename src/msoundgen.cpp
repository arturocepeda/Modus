
////////////////////////////////////////////////////////////////////////
//
//  Modus v0.60
//  C++ Music Library
//
//  Copyright (c) 2012-2015 Arturo Cepeda Pérez
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
#include <algorithm>

using namespace Modus;

//
//  MCSoundGen
//
MCSoundGen::MCSoundGen(unsigned int ID, unsigned char NumberOfChannels)
    : iID(ID)
    , iNumberOfChannels(NumberOfChannels)
    , fVolume(1.0f)
    , fPan(0.0f)
    , fReleaseSpeed(MODUS_DEFAULT_RELEASE)
    , bDamper(false)
{
}

void MCSoundGen::releaseDamper()
{
    while(!vChannelsSustained.empty())
    {
        releaseNote(vChannelsSustained[vChannelsSustained.size() - 1]);
        vChannelsSustained.removeLast();
    }
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
MCSoundGenAudio::MCSoundGenAudio(unsigned int ID, unsigned char NumberOfChannels, bool Sound3D)
    : MCSoundGen(ID, NumberOfChannels)
    , b3DSound(Sound3D)
{
    f3DPosition[0] = 0.0f;  f3DPosition[1] = 0.0f;  f3DPosition[2] = 0.0f;
    f3DDirection[0] = 0.0f; f3DDirection[1] = 0.0f; f3DDirection[2] = 1.0f;

    iSampleSet = new int[iNumberOfChannels];

    for(unsigned int i = 0; i < iNumberOfChannels; i++)
        iSampleSet[i] = -1;
}

MCSoundGenAudio::~MCSoundGenAudio()
{
    delete[] iSampleSet;
}

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

void MCSoundGenAudio::get3DPosition(float* X, float* Y, float* Z)
{
    *X = f3DPosition[0];
    *Y = f3DPosition[1];
    *Z = f3DPosition[2];
}

void MCSoundGenAudio::get3DDirection(float* X, float* Y, float* Z)
{
    *X = f3DDirection[0];
    *Y = f3DDirection[1];
    *Z = f3DDirection[2];
}



//
//  MSAudioSource
//
MSAudioSource::MSAudioSource()
    : Source(NULL)
    , EntityID(0)
    , EntityChannel(0)
    , Assignment(0)
    , Volume(1.0f)
    , Free(true)
{
    memset(Position, 0, sizeof(float) * 3);
    memset(Direction, 0, sizeof(float) * 3);
}



//
//  MCAudioSourceManager
//
MCAudioSourceManager::MCAudioSourceManager(unsigned int NumSources)
    : iNumSources(NumSources)
    , iCurrentAssignment(0)
{
    sSources = new MSAudioSource[iNumSources];
}

MCAudioSourceManager::~MCAudioSourceManager()
{
    delete[] sSources;
}

unsigned int MCAudioSourceManager::assignSource(unsigned int EntityID, unsigned int EntityChannel)
{
    unsigned int iIndexOldestAssignment = 0;
    unsigned int iOldestAssignment = sSources[0].Assignment;
    
    for(unsigned int i = 0; i < iNumSources; i++)
    {
        // this source is free
        if(sSources[i].Free)
        {
            assignSource(i, EntityID, EntityChannel);
            return i;
        }

        if(sSources[i].Assignment < iOldestAssignment)
        {
            iOldestAssignment = sSources[i].Assignment;
            iIndexOldestAssignment = i;
        }
    }

    // there are no free sources
    assignSource(iIndexOldestAssignment, EntityID, EntityChannel);
    stopSource(iIndexOldestAssignment);

    return iIndexOldestAssignment;
}

void MCAudioSourceManager::assignSource(unsigned int SourceIndex, unsigned int EntityID, unsigned int EntityChannel)
{
    sSources[SourceIndex].EntityID = EntityID;
    sSources[SourceIndex].EntityChannel = EntityChannel;
    sSources[SourceIndex].Assignment = ++iCurrentAssignment;
    sSources[SourceIndex].Free = false;
}

void MCAudioSourceManager::freeSource(unsigned int SourceIndex)
{
    if(SourceIndex < iNumSources)
    {
        sSources[SourceIndex].EntityID = 0;
        sSources[SourceIndex].EntityChannel = 0;
        sSources[SourceIndex].Free = true;
    }
}

const MSAudioSource& MCAudioSourceManager::getSource(unsigned int SourceIndex)
{
    return sSources[SourceIndex];
}

float MCAudioSourceManager::getSourceVolume(unsigned int SourceIndex)
{
    return sSources[SourceIndex].Volume;
}

float MCAudioSourceManager::getSourcePan(unsigned int SourceIndex)
{
    return sSources[SourceIndex].Position[0];
}

void MCAudioSourceManager::getSourcePosition(unsigned int SourceIndex, float* X, float* Y, float* Z)
{
    *X = sSources[SourceIndex].Position[0];
    *Y = sSources[SourceIndex].Position[1];
    *Z = sSources[SourceIndex].Position[2];
}

void MCAudioSourceManager::getSourceDirection(unsigned int SourceIndex, float* X, float* Y, float* Z)
{
    *X = sSources[SourceIndex].Direction[0];
    *Y = sSources[SourceIndex].Direction[1];
    *Z = sSources[SourceIndex].Direction[2];
}



//
//  MCSoundGenAudioMultipleChannel
//
MCAudioSourceManager* MCSoundGenAudioMultipleChannel::cManager = NULL;

MCSoundGenAudioMultipleChannel::MCSoundGenAudioMultipleChannel(unsigned int ID, unsigned char NumberOfChannels, bool Sound3D)
    : MCSoundGenAudio(ID, NumberOfChannels, Sound3D)
{
    sInstrumentChannels = new MSInstrumentChannel[iNumberOfChannels];
}

MCSoundGenAudioMultipleChannel::~MCSoundGenAudioMultipleChannel()
{
    delete[] sInstrumentChannels;
}

void MCSoundGenAudioMultipleChannel::playNote(MSNote& Note)
{
    // find the appropriate sample set for this note
    iSampleSet[Note.Channel] = findSampleSet(Note);

    if(iSampleSet[Note.Channel] == -1)
        return;

    // calculate sample number
    int iSample = Note.Pitch - sSampleSet[iSampleSet[Note.Channel]].Range.LowestNote;

    // release current primary audio channel
    releaseChannel(Note.Channel, true);

    // if the channel is in the sustained list, remove it
    vChannelsSustained.removeValue(Note.Channel);

    // put the new audio channel at the beginning of the list
    MSAudioChannel sNewChannel;
    sNewChannel.Index = cManager->assignSource(iID, Note.Channel);

    if(sInstrumentChannels[Note.Channel].AudioChannels.empty())
        sInstrumentChannels[Note.Channel].AudioChannels.push_back(sNewChannel);
    else
        sInstrumentChannels[Note.Channel].AudioChannels.insert(sInstrumentChannels[Note.Channel].AudioChannels.begin(), sNewChannel);

    // play the note
    cManager->setSourceVolume(sNewChannel.Index, calculateNoteVolume(Note.Intensity, iSampleSet[Note.Channel]));

    if(b3DSound)
        cManager->setSourcePosition(sNewChannel.Index, f3DPosition[0], f3DPosition[1], f3DPosition[2]);
    else
        cManager->setSourcePan(sNewChannel.Index, fPan);

    playAudioSample(sNewChannel.Index, iSampleSet[Note.Channel], iSample);
}

void MCSoundGenAudioMultipleChannel::releaseNote(unsigned char Channel)
{
    // damper on: don't release the note, but put it in the sustained list
    if(bDamper)
        vChannelsSustained.add(Channel);
    else
        releaseChannel(Channel, false);
}

void MCSoundGenAudioMultipleChannel::releaseChannel(unsigned char iChannel, bool bQuickly)
{
    if(sInstrumentChannels[iChannel].AudioChannels.empty())
        return;

    // make sure that the source is still ours
    unsigned int iAudioChannelIndex = sInstrumentChannels[iChannel].AudioChannels[0].Index;
    const MSAudioSource& sSource = cManager->getSource(iAudioChannelIndex);

    if(sSource.EntityID != iID || sSource.EntityChannel != iChannel)
    {
        sInstrumentChannels[iChannel].AudioChannels.clear();
        return;
    }

    // update the "quickly" flag, make sure that the channel is in the list of channels to release
    // and leave in case the audio channel is already marked to be released
    if(sInstrumentChannels[iChannel].AudioChannels[0].Release)
    {
        sInstrumentChannels[iChannel].AudioChannels[0].QuickRelease = bQuickly;
        vChannelsToRelease.add(iChannel);
        return;
    }

    // if the primary channel is playing a sound, set it to be faded out
    if(cManager->isSourcePlaying(iAudioChannelIndex))
    {
        // we need two values: the volume of the channel at the moment (initial) and another value that
        // always goes from 1.0f to 0.0f, in order to apply a fade out which doesn't depend on the initial
        // volume
        sInstrumentChannels[iChannel].AudioChannels[0].InitialReleaseVolume = cManager->getSourceVolume(iAudioChannelIndex);
        sInstrumentChannels[iChannel].AudioChannels[0].CurrentReleaseState = 1.0f;
        sInstrumentChannels[iChannel].AudioChannels[0].Release = true;
        sInstrumentChannels[iChannel].AudioChannels[0].QuickRelease = bQuickly;

        // add the current channel to the list to be released
        if(bDamper)
            vChannelsSustained.add(iChannel);
        else
            vChannelsToRelease.add(iChannel);
    }

    // if not, just set the source free
    else
    {
        cManager->freeSource(iAudioChannelIndex);
        sInstrumentChannels[iChannel].AudioChannels.clear();
    }
}

void MCSoundGenAudioMultipleChannel::update()
{
    unsigned int iChannel;

    // a fade out will be done to the secondary channels that must be released
    unsigned int iCurrentChannelToRelease = 0;

    while(iCurrentChannelToRelease < vChannelsToRelease.size())
    {
        iChannel = vChannelsToRelease[iCurrentChannelToRelease];

        // no audio channels to fade/release
        if(sInstrumentChannels[iChannel].AudioChannels.empty())
        {
            vChannelsToRelease.removeValue(iChannel);
            continue;
        }

        // check damper
        if(bDamper)
        {
            vChannelsToRelease.removeValue(iChannel);
            vChannelsSustained.add(iChannel);
            continue;
        }

        unsigned int iCurrentAudioChannel = 0;

        while(iCurrentAudioChannel < sInstrumentChannels[iChannel].AudioChannels.size())
        {
            MSAudioChannel& sAudioChannel = sInstrumentChannels[iChannel].AudioChannels[iCurrentAudioChannel];
            const MSAudioSource& sSource = cManager->getSource(sAudioChannel.Index);

            // the source manager has given this source to another sample
            if(sSource.EntityID != iID || sSource.EntityChannel != iChannel)
            {
                sInstrumentChannels[iChannel].AudioChannels.erase(sInstrumentChannels[iChannel].AudioChannels.begin() + iCurrentAudioChannel);
                continue;
            }

            // the current audio channel must not be released
            if(!sAudioChannel.Release)
            {
                iCurrentAudioChannel++;
                continue;
            }

            // next step for the fade-out
            if(sAudioChannel.CurrentReleaseState > 0.0f)
            {
                sAudioChannel.CurrentReleaseState -= sAudioChannel.QuickRelease ? fReleaseSpeed * 2.0f : fReleaseSpeed;
                sAudioChannel.CurrentReleaseState = std::max(sAudioChannel.CurrentReleaseState, 0.0f);
                float fCurrentGain = sAudioChannel.CurrentReleaseState * sAudioChannel.InitialReleaseVolume;
                cManager->setSourceVolume(sAudioChannel.Index, fCurrentGain);
            }

            // fade-out completed: stop and set the source free
            else
            {
                cManager->stopSource(sAudioChannel.Index);
                cManager->freeSource(sAudioChannel.Index);
                sInstrumentChannels[iChannel].AudioChannels.erase(sInstrumentChannels[iChannel].AudioChannels.begin() + iCurrentAudioChannel);
                continue;
            }

            iCurrentAudioChannel++;
        }

        iCurrentChannelToRelease++;
    }

    // and the same for all the secondary audio channels that are sustained (only the primary ones must stay sounding)
    unsigned int iCurrentChannelToSustain = 0;
    
    while(iCurrentChannelToSustain < vChannelsSustained.size())
    {
        iChannel = vChannelsSustained[iCurrentChannelToSustain];

        unsigned int iCurrentAudioChannel = 1;

        while(iCurrentAudioChannel < sInstrumentChannels[iChannel].AudioChannels.size())
        {
            MSAudioChannel& sAudioChannel = sInstrumentChannels[iChannel].AudioChannels[iCurrentAudioChannel];
            const MSAudioSource& sSource = cManager->getSource(sAudioChannel.Index);

            // the source manager has given this source to another sample
            if(sSource.EntityID != iID || sSource.EntityChannel != iChannel)
            {
                sInstrumentChannels[iChannel].AudioChannels.erase(sInstrumentChannels[iChannel].AudioChannels.begin() + iCurrentAudioChannel);
                continue;
            }

            // next step for the fade-out
            if(sAudioChannel.CurrentReleaseState > 0.0f)
            {
                sAudioChannel.CurrentReleaseState -= sAudioChannel.QuickRelease ? fReleaseSpeed * 2.0f : fReleaseSpeed;
                sAudioChannel.CurrentReleaseState = std::max(sAudioChannel.CurrentReleaseState, 0.0f);
                float fCurrentGain = sAudioChannel.CurrentReleaseState * sAudioChannel.InitialReleaseVolume;
                cManager->setSourceVolume(sAudioChannel.Index, fCurrentGain);
            }

            // fade-out completed: stop and set the source free
            else
            {
                cManager->stopSource(sAudioChannel.Index);
                cManager->freeSource(sAudioChannel.Index);
                sInstrumentChannels[iChannel].AudioChannels.erase(sInstrumentChannels[iChannel].AudioChannels.begin() + iCurrentAudioChannel);
                continue;
            }

            iCurrentAudioChannel++;
        }

        iCurrentChannelToSustain++;
    }
}

void MCSoundGenAudioMultipleChannel::setBending(unsigned char Channel, int Cents)
{
    if(sInstrumentChannels[Channel].AudioChannels.empty())
        return;

    unsigned int iAudioChannelIndex = sInstrumentChannels[Channel].AudioChannels[0].Index;
    const MSAudioSource& sSource = cManager->getSource(iAudioChannelIndex);

    if(sSource.EntityID == iID && sSource.EntityChannel == Channel)
        cManager->setSourcePitch(iAudioChannelIndex, Cents);
}

void MCSoundGenAudioMultipleChannel::setIntensity(unsigned char Channel, unsigned char Intensity)
{
    if(sInstrumentChannels[Channel].AudioChannels.empty())
        return;

    unsigned int iAudioChannelIndex = sInstrumentChannels[Channel].AudioChannels[0].Index;
    const MSAudioSource& sSource = cManager->getSource(iAudioChannelIndex);

    if(sSource.EntityID == iID && sSource.EntityChannel == Channel && iSampleSet[Channel] > -1)
        cManager->setSourceVolume(iAudioChannelIndex, calculateNoteVolume(Intensity, iSampleSet[Channel]));
}

void MCSoundGenAudioMultipleChannel::set3DPosition(float X, float Y, float Z)
{
    f3DPosition[0] = X;
    f3DPosition[1] = Y;
    f3DPosition[2] = Z;

    // take effect on the channels that are sounding
    for(unsigned int i = 0; i < iNumberOfChannels; i++)
    {
        for(unsigned int j = 0; j < sInstrumentChannels[i].AudioChannels.size(); j++)
        {
            MSAudioChannel& sAudioChannel = sInstrumentChannels[i].AudioChannels[j];
            const MSAudioSource& sSource = cManager->getSource(sAudioChannel.Index);

            if(sSource.EntityID == iID && sSource.EntityChannel == i)
            {
                if(cManager->isSourcePlaying(sAudioChannel.Index))
                    cManager->setSourcePosition(sAudioChannel.Index, f3DPosition[0], f3DPosition[1], f3DPosition[2]);
            }
        }
    }
}

void MCSoundGenAudioMultipleChannel::set3DDirection(float X, float Y, float Z)
{
    f3DDirection[0] = X;
    f3DDirection[1] = Y;
    f3DDirection[2] = Z;

    // take effect on the channels that are sounding
    for(unsigned int i = 0; i < iNumberOfChannels; i++)
    {
        for(unsigned int j = 0; j < sInstrumentChannels[i].AudioChannels.size(); j++)
        {
            MSAudioChannel& sAudioChannel = sInstrumentChannels[i].AudioChannels[j];
            const MSAudioSource& sSource = cManager->getSource(sAudioChannel.Index);            

            if(sSource.EntityID == iID && sSource.EntityChannel == i)
            {
                if(cManager->isSourcePlaying(sAudioChannel.Index))
                    cManager->setSourceDirection(sAudioChannel.Index, f3DDirection[0], f3DDirection[1], f3DDirection[2]);
            }
        }
    }
}
