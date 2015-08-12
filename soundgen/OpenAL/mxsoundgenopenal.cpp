
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

using namespace Modus;

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
//  Audio source manager
//
MCOpenALSourceManager::MCOpenALSourceManager(unsigned int NumSources)
    : MCAudioSourceManager(NumSources)
{
}

void MCOpenALSourceManager::allocateSources()
{
    for(unsigned int i = 0; i < iNumSources; i++)
    {
        sSources[i].Source = new ALuint();
        alGenSources(1, reinterpret_cast<ALuint*>(sSources[i].Source));
    }
}

void MCOpenALSourceManager::releaseSources()
{
    for(unsigned int i = 0; i < iNumSources; i++)
    {
        ALuint* alSourceAlloc = reinterpret_cast<ALuint*>(sSources[i].Source);
        alDeleteSources(1, alSourceAlloc);
        delete alSourceAlloc;
    }
}

void MCOpenALSourceManager::playSource(unsigned int SourceIndex, void* Sound, bool Sound3D)
{
    ALuint alSource = *(reinterpret_cast<ALuint*>(sSources[SourceIndex].Source));
    ALint alBuffer = *(reinterpret_cast<ALint*>(Sound));

    alSourcei(alSource, AL_BUFFER, alBuffer);
    alSourcef(alSource, AL_PITCH, 1.0f);
    alSourcePlay(alSource);
}

void MCOpenALSourceManager::stopSource(unsigned int SourceIndex)
{
    ALuint alSource = *(reinterpret_cast<ALuint*>(sSources[SourceIndex].Source));
    alSourceStop(alSource);
}

bool MCOpenALSourceManager::isSourcePlaying(unsigned int SourceIndex)
{
    ALuint alSource = *(reinterpret_cast<ALuint*>(sSources[SourceIndex].Source));
    ALint alSourceState;

    alGetSourcei(alSource, AL_SOURCE_STATE, &alSourceState);

    return alSourceState == AL_PLAYING;
}

void MCOpenALSourceManager::setSourceVolume(unsigned int SourceIndex, float Volume)
{
    sSources[SourceIndex].Volume = Volume;

    ALuint alSource = *(reinterpret_cast<ALuint*>(sSources[SourceIndex].Source));
    alSourcef(alSource, AL_GAIN, Volume);
}

void MCOpenALSourceManager::setSourcePitch(unsigned int SourceIndex, int Cents)
{
    ALuint alSource = *(reinterpret_cast<ALuint*>(sSources[SourceIndex].Source));
    alSourcef(alSource, AL_PITCH, (float)pow(2, Cents / 1200.0f));
}

void MCOpenALSourceManager::setSourcePan(unsigned int SourceIndex, float Pan)
{
    sSources[SourceIndex].Position[0] = Pan;

    ALuint alSource = *(reinterpret_cast<ALuint*>(sSources[SourceIndex].Source));
    alSource3f(alSource, AL_POSITION, Pan, 0.0f, 0.0f);
}

void MCOpenALSourceManager::setSourcePosition(unsigned int SourceIndex, float X, float Y, float Z)
{
    sSources[SourceIndex].Position[0] = X;
    sSources[SourceIndex].Position[1] = Y;
    sSources[SourceIndex].Position[2] = Z;

    ALuint alSource = *(reinterpret_cast<ALuint*>(sSources[SourceIndex].Source));
    alSource3f(alSource, AL_POSITION, X, Y, Z);
}

void MCOpenALSourceManager::setSourceDirection(unsigned int SourceIndex, float X, float Y, float Z)
{
    sSources[SourceIndex].Direction[0] = X;
    sSources[SourceIndex].Direction[1] = Y;
    sSources[SourceIndex].Direction[2] = Z;

    ALuint alSource = *(reinterpret_cast<ALuint*>(sSources[SourceIndex].Source));
    alSource3f(alSource, AL_DIRECTION, X, Y, Z);
}



//
//  Sound generator
//
unsigned int MCSoundGenOpenAL::iNumberOfInstances = 0;

MCSoundGenOpenAL::MCSoundGenOpenAL(unsigned int ID, unsigned int NumberOfChannels, bool Sound3D)
    : MCSoundGenAudioMultipleChannel(ID, NumberOfChannels, Sound3D)
{
    if(!cManager)
    {
        cManager = new MCOpenALSourceManager(OPENAL_SOURCES);
        cManager->allocateSources();
    }

    iNumberOfInstances++;

    alBuffers = NULL;
}

MCSoundGenOpenAL::~MCSoundGenOpenAL()
{
    unloadSamples();

    iNumberOfInstances--;

    if(iNumberOfInstances == 0)
    {
        cManager->releaseSources();
        delete cManager;
        cManager = NULL;
    }
}

void MCSoundGenOpenAL::loadSamples()
{
    if(sSampleSet.size() == 0)
        return;

    char sFilename[512];
    unsigned int i, j;

    alBuffers = new ALuint*[sSampleSet.size()];

    for(i = 0; i < sSampleSet.size(); i++)
    {
        alBuffers[i] = new ALuint[iNumberOfSamples[i]];
        alGenBuffers(iNumberOfSamples[i], &alBuffers[i][0]);

        for(j = 0; j < iNumberOfSamples[i]; j++)
        {
            sprintf(sFilename, "%s%02d_%03d.%s", sSampleSet[i].Path, 
                                                 sSampleSet[i].ID, 
                                                 j + sSampleSet[i].Range.LowestNote,
                                                 sSampleSet[i].Format);

            strtolower(sSampleSet[i].Format);

            // OGG Vorbis format
            if(strcmp(sSampleSet[i].Format, "ogg") == 0)
                loadOGGFile(sFilename, alBuffers[i][j]);

            // WAV format
            else if(strcmp(sSampleSet[i].Format, "wav") == 0)
                loadWAVFile(sFilename, alBuffers[i][j]);
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

    alBuffers = new ALuint*[iNumSampleSets];

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

        alBuffers[i] = new ALuint[iNumSamples];
        alGenBuffers(iNumSamples, &alBuffers[i][0]);

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
                loadWAVData(sSampleData, iSampleSize, alBuffers[i][j]);
            else if(strcmp(sFormat, "ogg") == 0)
                loadOGGData(sSampleData, iSampleSize, alBuffers[i][j]);

            // callback
            if(callback)
                callback(iTotalSamples, ++iLoaded, Data);

            delete[] sSampleData;
        }
    }
}

void MCSoundGenOpenAL::unloadSamples()
{
    if(!alBuffers)
        return;

    for(unsigned int i = 0; i < sSampleSet.size(); i++)
    {
        alDeleteBuffers(iNumberOfSamples[i], &alBuffers[i][0]);
        delete[] alBuffers[i];
    }

    sSampleSet.clear();
    iNumberOfSamples.clear();

    delete[] alBuffers;
    alBuffers = NULL;
}

void MCSoundGenOpenAL::playAudioSample(unsigned int SourceIndex, int SampleSet, int SampleIndex)
{
    cManager->playSource(SourceIndex, (ALint*)alBuffers[SampleSet] + SampleIndex, b3DSound);
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
