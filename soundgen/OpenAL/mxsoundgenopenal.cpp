
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
//  OpenAL - Open Audio Library
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

#include "mxsoundgenopenal.h"
#include <fstream>
#include <cmath>
#include "mutils.h"

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


//
//  Sound generator
//
MCSoundGenOpenAL::MCSoundGenOpenAL(unsigned int NumberOfChannels, bool Sound3D, int ID, 
                                   MCOpenALSourceManager* OpenALSourceManager)
{
    iID = ID;
    alManager = OpenALSourceManager;

    iNumberOfChannels = NumberOfChannels;
    b3DSound = Sound3D;
    alBuffer = NULL;
    fVolume = 1.0f;
    fPan = 0.0f;
    fReleaseSpeed = M_DEFAULT_RELEASE;
    bDamper = false;

    al3DPosition[0] = 0.0f;
    al3DPosition[1] = 0.0f;
    al3DPosition[2] = 0.0f;

    iSampleSet = new int[iNumberOfChannels];

    for(unsigned int i = 0; i < iNumberOfChannels; i++)
        iSampleSet[i] = -1;

    initChannelData();
}

MCSoundGenOpenAL::~MCSoundGenOpenAL()
{
    delete[] iSampleSet;

    unloadSamples();
    releaseChannelData();
}

void MCSoundGenOpenAL::loadSamples()
{
    if(sSampleSet.size() == 0)
        return;

    char sFilename[512];
    unsigned int i, j;

    alBuffer = new ALuint*[sSampleSet.size()];

    for(i = 0; i < sSampleSet.size(); i++)
    {
        alBuffer[i] = new ALuint[iNumberOfSamples[i]];
        alGenBuffers(iNumberOfSamples[i], &alBuffer[i][0]);

        for(j = 0; j < iNumberOfSamples[i]; j++)
        {
            sprintf(sFilename, "%s%02d_%03d.%s", sSampleSet[i].Path, 
                                                 sSampleSet[i].ID, 
                                                 j + sSampleSet[i].Range.LowestNote,
                                                 sSampleSet[i].Format);

            strtolower(sSampleSet[i].Format);

            // OGG Vorbis format
            if(strcmp(sSampleSet[i].Format, "ogg") == 0)
                loadOGGFile(sFilename, alBuffer[i][j]);

            // WAV format
            else if(strcmp(sSampleSet[i].Format, "wav") == 0)
                loadWAVFile(sFilename, alBuffer[i][j]);
        }
    }
}

void MCSoundGenOpenAL::loadSamplePack(std::istream& Stream, 
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

    alBuffer = new ALuint*[iNumSampleSets];

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

        alBuffer[i] = new ALuint[iNumSamples];
        alGenBuffers(iNumSamples, &alBuffer[i][0]);

        unsigned int iSampleSize;
        char *sSampleData;

        for(j = 0; j < iNumSamples; j++)
        {
            Stream.read((char*)&iSampleSize, sizeof(unsigned int));

            if(iSampleSize == 0)
                continue;

            sSampleData = new char[iSampleSize];
            Stream.read(sSampleData, iSampleSize);

            strtolower(sFormat);

            if(strcmp(sFormat, "wav") == 0)
                loadWAVData(sSampleData, iSampleSize, alBuffer[i][j]);
            else if(strcmp(sFormat, "ogg") == 0)
                loadOGGData(sSampleData, iSampleSize, alBuffer[i][j]);

            // callback
            if(callback)
                callback(iTotalSamples, ++iLoaded, Data);

            delete[] sSampleData;
        }
    }
}

void MCSoundGenOpenAL::unloadSamples()
{
    if(!alBuffer)
        return;

    for(unsigned int i = 0; i < sSampleSet.size(); i++)
    {
        alDeleteBuffers(iNumberOfSamples[i], &alBuffer[i][0]);
        delete[] alBuffer[i];
    }

    sSampleSet.clear();
    iNumberOfSamples.clear();

    delete[] alBuffer;
    alBuffer = NULL;
}

void MCSoundGenOpenAL::playNote(MSNote& Note)
{
    if(!alBuffer)
        return;

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
    AudioChannel sNewChannel;
    sNewChannel.Index = alManager->assignSource(iID, Note.Channel);

    if(sInstrumentChannels[Note.Channel].AudioChannels.empty())
        sInstrumentChannels[Note.Channel].AudioChannels.push_back(sNewChannel);
    else
        sInstrumentChannels[Note.Channel].AudioChannels.insert(sInstrumentChannels[Note.Channel].AudioChannels.begin(), sNewChannel);

    // play the note
    ALuint alSource = alManager->getSource(sNewChannel.Index);

    alSourcei(alSource, AL_BUFFER, alBuffer[iSampleSet[Note.Channel]][iSample]);

    if(b3DSound)
        alSource3f(alSource, AL_POSITION, al3DPosition[0], al3DPosition[1], al3DPosition[2]);
    else
        alSource3f(alSource, AL_POSITION, fPan, 0.0f, 0.0f);

    alSourcef(alSource, AL_PITCH, 1.0f);
    alSourcef(alSource, AL_GAIN, calculateNoteVolume(Note.Intensity, iSampleSet[Note.Channel]));
    alSourcePlay(alSource);
}

void MCSoundGenOpenAL::releaseChannel(unsigned char iChannel, bool bQuickly)
{
    if(!alBuffer || iChannel >= iNumberOfChannels || sInstrumentChannels[iChannel].AudioChannels.empty())
        return;

    // make sure that the source is still ours
    unsigned int iAudioChannelIndex = sInstrumentChannels[iChannel].AudioChannels[0].Index;

    if(alManager->getEntityID(iAudioChannelIndex) != iID || alManager->getEntityChannel(iAudioChannelIndex) != iChannel)
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

    // get primary audio source
    ALuint alSource = alManager->getSource(sInstrumentChannels[iChannel].AudioChannels[0].Index);

    // check whether the primary audio channel is playing a sound
    ALint alSourceState;
    alGetSourcei(alSource, AL_SOURCE_STATE, &alSourceState);

    // if the primary channel is playing a sound, change the handler to the secondary channel and notify
    // the generator to apply a fade out to it
    if(alSourceState == AL_PLAYING)
    {
        // we need two values: the volume of the channel at the moment (initial) and another value that
        // always goes from 1.0f to 0.0f, in order to apply a fade out which doesn't depend on the initial
        // volume
        alGetSourcef(alSource, AL_GAIN, &sInstrumentChannels[iChannel].AudioChannels[0].InitialReleaseVolume);
        sInstrumentChannels[iChannel].AudioChannels[0].CurrentReleaseState = 1.0f;
        sInstrumentChannels[iChannel].AudioChannels[0].Release = true;
        sInstrumentChannels[iChannel].AudioChannels[0].QuickRelease = bQuickly;

        // add the current channel to the list to be released
        if(bDamper)
            vChannelsSustained.add(iChannel);
        else
            vChannelsToRelease.add(iChannel);
    }

    // if not, just release the source
    else
    {
        alManager->releaseSource(sInstrumentChannels[iChannel].AudioChannels[0].Index);
        sInstrumentChannels[iChannel].AudioChannels.clear();
    }
}

void MCSoundGenOpenAL::update()
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
            AudioChannel& sAudioChannel = sInstrumentChannels[iChannel].AudioChannels[iCurrentAudioChannel];

            // the source manager has given this source to another sample
            if(alManager->getEntityID(sAudioChannel.Index) != iID || alManager->getEntityChannel(sAudioChannel.Index) != iChannel)
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
                sAudioChannel.CurrentReleaseState -= sAudioChannel.QuickRelease ? M_QUICK_RELEASE : fReleaseSpeed;
                float fCurrentGain = std::max(sAudioChannel.CurrentReleaseState * sAudioChannel.InitialReleaseVolume, 0.0f);
                alSourcef(alManager->getSource(sAudioChannel.Index), AL_GAIN, fCurrentGain);
            }

            // fade-out completed: stop and release the source
            else
            {
                alSourceStop(alManager->getSource(sAudioChannel.Index));
                alManager->releaseSource(sAudioChannel.Index);
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
            AudioChannel& sAudioChannel = sInstrumentChannels[iChannel].AudioChannels[iCurrentAudioChannel];

            // the source manager has given this source to another sample
            if(alManager->getEntityID(sAudioChannel.Index) != iID || alManager->getEntityChannel(sAudioChannel.Index) != iChannel)
            {
                sInstrumentChannels[iChannel].AudioChannels.erase(sInstrumentChannels[iChannel].AudioChannels.begin() + iCurrentAudioChannel);
                continue;
            }

            // next step for the fade-out
            if(sAudioChannel.CurrentReleaseState > 0.0f)
            {
                sAudioChannel.CurrentReleaseState -= sAudioChannel.QuickRelease ? M_QUICK_RELEASE : fReleaseSpeed;
                float fCurrentGain = std::max(sAudioChannel.CurrentReleaseState * sAudioChannel.InitialReleaseVolume, 0.0f);
                alSourcef(alManager->getSource(sAudioChannel.Index), AL_GAIN, fCurrentGain);
            }

            // fade-out completed: stop and release the source
            else
            {
                alSourceStop(alManager->getSource(sAudioChannel.Index));
                alManager->releaseSource(sAudioChannel.Index);
                sInstrumentChannels[iChannel].AudioChannels.erase(sInstrumentChannels[iChannel].AudioChannels.begin() + iCurrentAudioChannel);
                continue;
            }

            iCurrentAudioChannel++;
        }

        iCurrentChannelToSustain++;
    }
}

void MCSoundGenOpenAL::get3DPosition(float* X, float* Y, float* Z)
{
    *X = al3DPosition[0];
    *Y = al3DPosition[1];
    *Z = al3DPosition[2];
}

void MCSoundGenOpenAL::get3DDirection(float* X, float* Y, float* Z)
{
    *X = al3DDirection[0];
    *Y = al3DDirection[1];
    *Z = al3DDirection[2];
}

void MCSoundGenOpenAL::setBending(unsigned char Channel, int Cents)
{
    unsigned int iAudioChannelIndex = sInstrumentChannels[Channel].AudioChannels[0].Index;

    if(alManager->getEntityID(iAudioChannelIndex) == iID && alManager->getEntityChannel(iAudioChannelIndex) == Channel)
        alSourcef(alManager->getSource(iAudioChannelIndex), AL_PITCH, (float)pow(2, Cents / 1200.0f));
}

void MCSoundGenOpenAL::setIntensity(unsigned char Channel, unsigned char Intensity)
{
    unsigned int iAudioChannelIndex = sInstrumentChannels[Channel].AudioChannels[0].Index;

    if(alManager->getEntityID(iAudioChannelIndex) == iID &&
       alManager->getEntityChannel(iAudioChannelIndex) == Channel &&
       iSampleSet[Channel] > -1)
    {
        alSourcef(alManager->getSource(iAudioChannelIndex), AL_GAIN, 
                  calculateNoteVolume(Intensity, iSampleSet[Channel]));
    }
}

void MCSoundGenOpenAL::set3DPosition(float X, float Y, float Z)
{
    unsigned int i;
    unsigned int j;

    ALuint alSource;
    ALint alSourceState;

    al3DPosition[0] = X;
    al3DPosition[1] = Y;
    al3DPosition[2] = Z;

    // take effect on the channels that are sounding
    for(i = 0; i < iNumberOfChannels; i++)
    {
        for(j = 0; j < sInstrumentChannels[i].AudioChannels.size(); j++)
        {
            AudioChannel& sAudioChannel = sInstrumentChannels[i].AudioChannels[j];

            if(alManager->getEntityID(sAudioChannel.Index) == iID && alManager->getEntityChannel(sAudioChannel.Index) == i)
            {
                alSource = alManager->getSource(sAudioChannel.Index);
                alGetSourcei(alSource, AL_SOURCE_STATE, &alSourceState);

                if(alSourceState == AL_PLAYING)
                    alSource3f(alSource, AL_POSITION, al3DPosition[0], al3DPosition[1], al3DPosition[2]);
            }
        }
    }
}

void MCSoundGenOpenAL::set3DDirection(float X, float Y, float Z)
{
    unsigned int i;
    unsigned int j;

    ALuint alSource;
    ALint alSourceState;

    al3DDirection[0] = X;
    al3DDirection[1] = Y;
    al3DDirection[2] = Z;

    // take effect on the channels that are sounding
    for(i = 0; i < iNumberOfChannels; i++)
    {
        for(j = 0; j < sInstrumentChannels[i].AudioChannels.size(); j++)
        {
            AudioChannel& sAudioChannel = sInstrumentChannels[i].AudioChannels[j];

            if(alManager->getEntityID(sAudioChannel.Index) == iID && alManager->getEntityChannel(sAudioChannel.Index) == i)
            {
                alSource = alManager->getSource(sAudioChannel.Index);
                alGetSourcei(alSource, AL_SOURCE_STATE, &alSourceState);

                if(alSourceState == AL_PLAYING)
                    alSource3f(alSource, AL_DIRECTION, al3DDirection[0], al3DDirection[1], al3DDirection[2]);
            }
        }
    }
}

void MCSoundGenOpenAL::loadWAVData(const char* sData, unsigned int iSize, ALuint alBuffer)
{
    const int BUFFER_SIZE = 8;
    char sBuffer[BUFFER_SIZE];

    // "RIFF"
    strncpy(sBuffer, sData, 4);
    sBuffer[4] = '\0';
    sData += 4;

    if(strcmp(sBuffer, "RIFF") != 0)
        return;

    // "RIFF" chunk size
    sData += 4;

    // "WAVE"
    strncpy(sBuffer, sData, 4);
    sBuffer[4] = '\0';
    sData += 4;

    if(strcmp(sBuffer, "WAVE") != 0)
        return;

    // "fmt "
    strncpy(sBuffer, sData, 4);
    sBuffer[4] = '\0';
    sData += 4;

    if(strcmp(sBuffer, "fmt ") != 0)
        return;

    // "fmt " chunk size
    sData += 4;

    // audio format (2 bytes)
    sData += 2;

    // channels (2 bytes)
    short iChannels = *(short*)sData;
    sData += 2;

    // sample rate (4 bytes)
    int iSampleRate = *(int*)sData;
    sData += 4;

    // byte rate (4 bytes)
    sData += 4;

    // block align (2 bytes)
    sData += 2;

    // bits per sample (2 bytes)
    sData += 2;

    // "data" 
    // (there may be some bytes as extension for non PCM formats before)
    do
    {
        strncpy(sBuffer, sData, 4);
        sBuffer[4] = '\0';
        sData += 2;

    } while(strcmp(sBuffer, "data") != 0);

    sData += 2;

    // data chunk size
    int iDataSize = *(int*)sData;
    sData += 4;

    alBufferData(alBuffer, (iChannels == 1)? AL_FORMAT_MONO16: AL_FORMAT_STEREO16, sData, iDataSize, iSampleRate);
}

void MCSoundGenOpenAL::loadWAVFile(const char* sFilename, ALuint alBuffer)
{
    // open the file
    FILE* fFile = fopen(sFilename, "rb");

    if(!fFile)
        return;

    // file size
    fseek(fFile, 0L, SEEK_END);
    unsigned int iFileSize = ftell(fFile);
    fseek(fFile, 0L, SEEK_SET);

    // read file data
    char* sData = (char*)malloc(iFileSize);
    fread(sData, 1, iFileSize, fFile);

    // load sound buffer
    loadWAVData(sData, iFileSize, alBuffer);

    // release data and close the file
    free(sData);
    fclose(fFile);
}

void MCSoundGenOpenAL::loadOGGData(const char* sData, unsigned int iSize, ALuint alBuffer)
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
    ovCallbacks.close_func = NULL;

    // attach audio file data with the ovFile struct
    ov_open_callbacks(&sFile, &ovFile, NULL, 0, ovCallbacks);

    // check format and frequency
    vorbis_info* pVorbisInfo = ov_info(&ovFile, -1);
    ALenum eBufferFormat = (pVorbisInfo->channels == 1)? AL_FORMAT_MONO16: AL_FORMAT_STEREO16;
    ALsizei iFrequency = pVorbisInfo->rate;

    // read file data
    int iBitStream;
    int iReadedBytes;

    char sBufferArray[OV_BUFFER_SIZE];
    std::vector<char> vBufferData;

    do
    {
        iReadedBytes = ov_read(&ovFile, sBufferArray, OV_BUFFER_SIZE, 0, 2, 1, &iBitStream);
        vBufferData.insert(vBufferData.end(), sBufferArray, sBufferArray + iReadedBytes);

    } while(iReadedBytes > 0);

    ov_clear(&ovFile);

    // add sound data to the OpenAL buffer
    alBufferData(alBuffer, eBufferFormat, &vBufferData[0], (ALsizei)vBufferData.size(), iFrequency);
}

void MCSoundGenOpenAL::loadOGGFile(const char* sFilename, ALuint alBuffer)
{
    // open the file
    FILE* fFile = fopen(sFilename, "rb");

    if(!fFile)
        return;

    // file size
    fseek(fFile, 0L, SEEK_END);
    unsigned int iFileSize = ftell(fFile);
    fseek(fFile, 0L, SEEK_SET);

    // read file data
    char* sData = (char*)malloc(iFileSize);
    fread(sData, 1, iFileSize, fFile);

    // load sound buffer
    loadOGGData(sData, iFileSize, alBuffer);

    // release data and close the file
    free(sData);
    fclose(fFile);
}
