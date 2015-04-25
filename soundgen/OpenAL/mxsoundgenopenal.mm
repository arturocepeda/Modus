
////////////////////////////////////////////////////////////////////////
//
//  Modus
//  C++ Music Library
//  [Sound Generator]
//
//  Copyright (c) 2012-2014 Arturo Cepeda Pérez
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
            
            loadAudioFile([NSString stringWithUTF8String:sFilename], alBuffers[i][j]);
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

            // load sample file
            sSampleData = new char[iSampleSize];
            Stream.read(sSampleData, iSampleSize);
            loadAudioData(sSampleData, iSampleSize, alBuffers[i][j]);

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

void MCSoundGenOpenAL::loadAudioData(const char* sData, unsigned int iSize, ALuint alBuffer)
{
    NSArray* aAppDocumentPaths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString* sDocumentsDirectory = [aAppDocumentPaths objectAtIndex:0];
    NSString* sAudioFile = [sDocumentsDirectory stringByAppendingPathComponent:@"audiofile.tmp"];
    
    // create temporary file with audio data
    NSData* sAudioFileData = [NSData dataWithBytes:(const void*)sData length:iSize];
    
    if(![sAudioFileData writeToFile:sAudioFile atomically:YES])
        return;
    
    // load audio data from temporary file
    loadAudioFile(sAudioFile, alBuffer, false);    

    // remove temporary file
    NSFileManager* nsFileManager = [NSFileManager defaultManager];   
    [nsFileManager removeItemAtPath:sAudioFile error:NULL];
}

void MCSoundGenOpenAL::loadAudioFile(NSString* sFilename, ALuint alBuffer, bool bFromResources)
{
    CFURLRef cfURL;
    
    if(bFromResources)
    {
        NSString* sSoundFilePath = [[[NSBundle mainBundle] resourcePath] stringByAppendingPathComponent: sFilename];   
        cfURL = (CFURLRef)[NSURL fileURLWithPath:sSoundFilePath];
    }
    else
    {        
        cfURL = (CFURLRef)[NSURL fileURLWithPath:sFilename];
    }
    
    ExtAudioFileRef sAudioFileRef;
    OSStatus osError;
        
    // get file reference
    osError = ExtAudioFileOpenURL(cfURL, &sAudioFileRef);
    
    if(osError != noErr)
        return;
    
    // get the number of audio frames from the file
    UInt32 iSize;
    UInt64 iFileLengthFrames; 
    
    iSize = sizeof(UInt64);
    
    osError = ExtAudioFileGetProperty(sAudioFileRef, kExtAudioFileProperty_FileLengthFrames, 
                                      &iSize, (void*)&iFileLengthFrames);
    
    // get the description of the audio file
    AudioStreamBasicDescription sDescription;
    
    iSize = sizeof(AudioStreamBasicDescription);
    
    osError = ExtAudioFileGetProperty(sAudioFileRef, kExtAudioFileProperty_FileDataFormat, 
                                      &iSize, (void*)&sDescription);
    
    // define the new PCM format
    sDescription.mFormatID = kAudioFormatLinearPCM;
    sDescription.mFormatFlags = kAudioFormatFlagsNativeEndian |
                                kAudioFormatFlagIsSignedInteger |
                                kAudioFormatFlagIsPacked;
    
    if(sDescription.mChannelsPerFrame > 2)
        sDescription.mChannelsPerFrame = 2;
    
    sDescription.mBitsPerChannel = 16;   
    sDescription.mBytesPerFrame = sDescription.mChannelsPerFrame * sDescription.mBitsPerChannel / 8;
    sDescription.mFramesPerPacket = 1;
    sDescription.mBytesPerPacket = sDescription.mBytesPerFrame * sDescription.mFramesPerPacket;
    
    // apply the new format
    osError = ExtAudioFileSetProperty(sAudioFileRef, kExtAudioFileProperty_ClientDataFormat,
                                      iSize, &sDescription);
    if(osError != noErr)
        return;
    
    // load audio data from the file
    UInt32 iStreamSizeInBytes = (UInt32)(sDescription.mBytesPerFrame * iFileLengthFrames);
    
    char* alData = (char*)malloc(iStreamSizeInBytes);
    
    if(!alData)
        return;
    
    AudioBufferList sBufferList;
    UInt32 iNumFramesRead = (UInt32)iFileLengthFrames;
    
    sBufferList.mNumberBuffers = 1;
    sBufferList.mBuffers[0].mNumberChannels = sDescription.mChannelsPerFrame;  
    sBufferList.mBuffers[0].mDataByteSize = sDescription.mBytesPerFrame * iNumFramesRead;
    sBufferList.mBuffers[0].mData = alData;
    
    osError = ExtAudioFileRead(sAudioFileRef, &iNumFramesRead, &sBufferList);
    
    if(osError != noErr)
    {
        free(alData);
        return;
    }
    
    // information for OpenAL buffer
    ALenum alFormat = (sDescription.mChannelsPerFrame == 2)? AL_FORMAT_STEREO16: AL_FORMAT_MONO16;
    ALuint alFrequency = (ALsizei)sDescription.mSampleRate;
    ALuint alSize = iNumFramesRead * sDescription.mBytesPerFrame;

    // fill OpenAL buffer
    alBufferData(alBuffer, alFormat, alData, alSize, alFrequency);
    
    // release PCM audio data
    free(alData);
}
