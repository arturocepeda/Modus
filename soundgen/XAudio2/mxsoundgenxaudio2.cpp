
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

#include "mxsoundgenxaudio2.h"
#include <fstream>
#include <cmath>
#include "mutils.h"

using namespace Modus;


//
//  MCSourceVoiceCallback
//
MCSourceVoiceCallback::MCSourceVoiceCallback(MSSourceVoice& SourceVoiceRef)
    : sSourceVoiceRef(SourceVoiceRef)
{
}

MCSourceVoiceCallback::~MCSourceVoiceCallback()
{
}

void MCSourceVoiceCallback::OnStreamEnd()
{
    sSourceVoiceRef.Playing = false;
}

void MCSourceVoiceCallback::OnVoiceProcessingPassEnd()
{
}

void MCSourceVoiceCallback::OnVoiceProcessingPassStart(UINT32 SamplesRequired)
{
}

void MCSourceVoiceCallback::OnBufferEnd(void* pBufferContext)
{
}

void MCSourceVoiceCallback::OnBufferStart(void* pBufferContext)
{
}

void MCSourceVoiceCallback::OnLoopEnd(void* pBufferContext)
{
}

void MCSourceVoiceCallback::OnVoiceError(void* pBufferContext, HRESULT Error)
{
}


//
//  Audio source manager
//
MCXAudio2SourceManager::MCXAudio2SourceManager(IXAudio2* Engine, unsigned int NumSources)
   : MCAudioSourceManager(NumSources)
   , xaEngine(Engine)
{
}

MCXAudio2SourceManager::~MCXAudio2SourceManager()
{
}

void MCXAudio2SourceManager::allocateSources()
{
    WAVEFORMATEX xaWaveFormat;
    xaWaveFormat.wFormatTag = WAVE_FORMAT_PCM;
    xaWaveFormat.nChannels = 1;
    xaWaveFormat.nSamplesPerSec = InputSampleRate;
    xaWaveFormat.nAvgBytesPerSec = InputSampleRate * 2;
    xaWaveFormat.nBlockAlign = 2;
    xaWaveFormat.wBitsPerSample = 16;
    xaWaveFormat.cbSize = 0;

    xaSourceVoices = new MSSourceVoice[iNumSources];

    for(unsigned int i = 0; i < iNumSources; i++)
    {
        xaSourceVoices[i].Callback = new MCSourceVoiceCallback(xaSourceVoices[i]);
        xaEngine->CreateSourceVoice(&xaSourceVoices[i].Handle, &xaWaveFormat, 0, 2.0f, xaSourceVoices[i].Callback);
    }

    fFrequencies = new float[iNumSources];
}

void MCXAudio2SourceManager::releaseSources()
{
    for(unsigned int i = 0; i < iNumSources; i++)
    {
        xaSourceVoices[i].Handle->DestroyVoice();
        delete xaSourceVoices[i].Callback;
    }

    delete[] fFrequencies;
    delete[] xaSourceVoices;
}

void MCXAudio2SourceManager::playSource(unsigned int SourceIndex, void* Sound, bool Sound3D)
{
    stopSource(SourceIndex);

    XAUDIO2_BUFFER* xaBuffer = static_cast<XAUDIO2_BUFFER*>(Sound);
    xaSourceVoices[SourceIndex].Handle->SubmitSourceBuffer(xaBuffer);
    xaSourceVoices[SourceIndex].Handle->Start();
    xaSourceVoices[SourceIndex].Playing = true;
}

void MCXAudio2SourceManager::stopSource(unsigned int SourceIndex)
{
    xaSourceVoices[SourceIndex].Handle->Stop();
    xaSourceVoices[SourceIndex].Handle->FlushSourceBuffers();
    xaSourceVoices[SourceIndex].Playing = false;
}

bool MCXAudio2SourceManager::isSourcePlaying(unsigned int SourceIndex)
{
    return xaSourceVoices[SourceIndex].Playing;
}

void MCXAudio2SourceManager::setSourceVolume(unsigned int SourceIndex, float Volume)
{
    sSources[SourceIndex].Volume = Volume;
    xaSourceVoices[SourceIndex].Handle->SetVolume(Volume);
}

void MCXAudio2SourceManager::setSourcePitch(unsigned int SourceIndex, int Cents)
{
    //TODO
    //float fFrequencyRatio = (float)pow(2, Cents / 1200.0f);
}

void MCXAudio2SourceManager::setSourcePan(unsigned int SourceIndex, float Pan)
{
    //TODO
}

void MCXAudio2SourceManager::setSourcePosition(unsigned int SourceIndex, float X, float Y, float Z)
{
    sSources[SourceIndex].Position[0] = X;
    sSources[SourceIndex].Position[1] = Y;
    sSources[SourceIndex].Position[2] = Z;
    //TODO
}

void MCXAudio2SourceManager::setSourceDirection(unsigned int SourceIndex, float X, float Y, float Z)
{
    sSources[SourceIndex].Direction[0] = X;
    sSources[SourceIndex].Direction[1] = Y;
    sSources[SourceIndex].Direction[2] = Z;
    //TODO
}


//
//  Sound generator
//
unsigned int MCSoundGenXAudio2::iNumberOfInstances = 0;

MCSoundGenXAudio2::MCSoundGenXAudio2(unsigned int ID, unsigned int NumberOfChannels, bool Sound3D, IXAudio2* Engine)
   : MCSoundGenAudioMultipleChannel(ID, NumberOfChannels, Sound3D)
   , xaBuffers(NULL)
{
    if(!cManager)
    {
        cManager = new MCXAudio2SourceManager(Engine, MODUS_XAUDIO2_SOURCES);
        cManager->allocateSources();
    }

    iNumberOfInstances++;

    iSampleSet = new int[iNumberOfChannels];
    fFrequency = new float[iNumberOfChannels];

    for(unsigned int i = 0; i < iNumberOfChannels; i++)
        iSampleSet[i] = -1;
}

MCSoundGenXAudio2::~MCSoundGenXAudio2()
{
    delete[] fFrequency;

    unloadSamples();

    iNumberOfInstances--;

    if(iNumberOfInstances == 0)
    {
        cManager->releaseSources();
        delete cManager;
        cManager = NULL;
    }
}

wchar_t* MCSoundGenXAudio2::charToWChar(const char* text)
{
    size_t size = strlen(text) + 1;
    wchar_t* wa = new wchar_t[size];
    mbstowcs(wa, text, size);
    return wa;
}

std::wstring MCSoundGenXAudio2::getFullPath(const char* Filename)
{
    auto folder = Windows::ApplicationModel::Package::Current->InstalledLocation; 
    std::wstring dir = folder->Path->ToString()->Data(); 
    return dir.append(L"\\").append(charToWChar(Filename));
}

unsigned int MCSoundGenXAudio2::getFileLength(const char* Filename)
{
    std::ifstream file(getFullPath(Filename), std::ios::in | std::ios::binary);

    if(file.is_open())
    {
        file.seekg(0, std::ios::end);
        std::streamsize size = file.tellg();
        file.close();
        return (unsigned int)size;
    }

    return 0;
}

unsigned int MCSoundGenXAudio2::readFile(const char* Filename, unsigned char* ReadBuffer, unsigned int BufferSize)
{
    std::ifstream file(getFullPath(Filename), std::ios::in | std::ios::binary);

    if(file.is_open())
    {
        file.seekg(0, std::ios::end);
        std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);

        file.read((char*)ReadBuffer, size);
        file.close();
        return (unsigned int)size;
    }

    return 0;
}

void MCSoundGenXAudio2::loadWAVFile(unsigned int iSampleSetIndex, unsigned int iSampleIndex, const char* sFileName)
{
    unsigned int iFileLength = getFileLength(sFileName);

    if(iFileLength == 0)
        return;

    char* sData = new char[iFileLength];
    readFile(sFileName, (unsigned char*)sData, iFileLength);

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

    // load wav data
    loadWAVData(iSampleSetIndex, iSampleIndex, iDataSize, sData);
    delete[] sData;
}

void MCSoundGenXAudio2::loadWAVData(unsigned int iSampleSetIndex, unsigned int iSampleIndex, unsigned int iDataSize, const char* pData)
{
    XAUDIO2_BUFFER& xaBuffer = xaBuffers[iSampleSetIndex][iSampleIndex];
    memset(&xaBuffer, 0, sizeof(XAUDIO2_BUFFER));

    xaBuffer.AudioBytes = iDataSize;
    xaBuffer.pAudioData = new BYTE[iDataSize];
    memcpy((void*)xaBuffer.pAudioData, pData, iDataSize);
    xaBuffer.Flags = XAUDIO2_END_OF_STREAM;
}

void MCSoundGenXAudio2::loadSamples()
{
    if(sSampleSet.size() == 0)
        return;

    char sFilename[512];
    unsigned int i, j;

    xaBuffers = new XAUDIO2_BUFFER*[sSampleSet.size()];

    for(i = 0; i < sSampleSet.size(); i++)
    {
        xaBuffers[i] = new XAUDIO2_BUFFER[iNumberOfSamples[i]];

        for(j = 0; j < iNumberOfSamples[i]; j++)
        {
            sprintf(sFilename, "%s%02d_%03d.%s", sSampleSet[i].Path, 
                                                 sSampleSet[i].ID, 
                                                 j + sSampleSet[i].Range.LowestNote,
                                                 sSampleSet[i].Format);
            loadWAVFile(i, j, sFilename);
        }
    }
}

void MCSoundGenXAudio2::loadSamplePack(std::istream& Stream, 
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

    xaBuffers = new XAUDIO2_BUFFER*[iNumSampleSets];

    for(i = 0; i < iNumSampleSets; i++)
    {
        // sample set
        strcpy(mSampleSet.Path, "");
        strcpy(mSampleSet.Format, sFormat);

        Stream.read((char*)&iInput, sizeof(unsigned int));        // SampleSet: ID
        mSampleSet.ID = (unsigned char)iInput;

        Stream.read((char*)&iInput, sizeof(unsigned int));        // SampleSet: Mode
        mSampleSet.Mode = (unsigned char)iInput;

        Stream.read((char*)&iInput, sizeof(unsigned int));        // SampleSet: Minimum intensity
        mSampleSet.MinimumIntensity = (unsigned char)iInput;

        Stream.read((char*)&iInput, sizeof(unsigned int));        // SampleSet: Maximum intensity
        mSampleSet.MaximumIntensity = (unsigned char)iInput;

        Stream.read((char*)&iInput, sizeof(unsigned int));        // SampleSet: Lowest pitch
        mSampleSet.Range.LowestNote = (unsigned char)iInput;

        Stream.read((char*)&iInput, sizeof(unsigned int));        // SampleSet: Highest pitch
        mSampleSet.Range.HighestNote = (unsigned char)iInput;

        sSampleSet.push_back(mSampleSet);

        // samples inside the sample set
        Stream.read((char*)&iNumSamples, sizeof(unsigned int));   // SampleSet: Number of samples
        iNumberOfSamples.push_back((unsigned char)iNumSamples);

        Stream.read(sReserved, 4);                                // SampleSet: (Reserved)

        xaBuffers[i] = new XAUDIO2_BUFFER[iNumSamples];

        unsigned int iSampleSize;
        char* sSampleData;

        for(j = 0; j < iNumSamples; j++)
        {
            Stream.read((char*)&iSampleSize, sizeof(unsigned int));

            if(iSampleSize == 0)
                continue;

            sSampleData = new char[iSampleSize];
            Stream.read(sSampleData, iSampleSize);

            loadWAVData(i, j, iSampleSize, sSampleData);

            // callback
            if(callback)
                callback(iTotalSamples, ++iLoaded, Data);

            delete[] sSampleData;
        }
    }
}

void MCSoundGenXAudio2::unloadSamples()
{
    if(!xaBuffers)
        return;

    unsigned int i;
    unsigned int j;

    for(i = 0; i < sSampleSet.size(); i++)
    {
        for(j = 0; j < iNumberOfSamples[i]; j++)
            delete[] xaBuffers[i][j].pAudioData;

        delete[] xaBuffers[i];
    }

    sSampleSet.clear();
    iNumberOfSamples.clear();

    delete[] xaBuffers;
    xaBuffers = NULL;
}

void MCSoundGenXAudio2::playAudioSample(unsigned int SourceIndex, int SampleSet, int SampleIndex)
{
   cManager->playSource(SourceIndex, &xaBuffers[SampleSet][SampleIndex], b3DSound);
}
