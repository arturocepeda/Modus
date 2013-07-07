
////////////////////////////////////////////////////////////////////////
//
//  Modus v0.52
//  C++ Music Library
//  [Sound Generator]
//
//  Copyright (c) 2012 Arturo Cepeda
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

    iSource = new int*[iNumberOfChannels];
    iSampleSet = new int[iNumberOfChannels];

    for(unsigned int i = 0; i < iNumberOfChannels; i++)
    {
        iSource[i] = new int[2];
        iSource[i][0] = 0;
        iSource[i][1] = 0;

        iSampleSet[i] = -1;
    }

    initChannelData();
}

MCSoundGenOpenAL::~MCSoundGenOpenAL()
{
    for(unsigned int i = 0; i < iNumberOfChannels; i++)
        delete[] iSource[i];

    delete[] iSource;
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
            
            loadAudioFile([NSString stringWithUTF8String:sFilename], alBuffer[i][j]);
        }
    }
}

void MCSoundGenOpenAL::loadSamplePack(const char* Filename, 
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

    alBuffer = new ALuint*[iNumSampleSets];

    for(i = 0; i < iNumSampleSets; i++)
    {
        // sample set
        strcpy(mSampleSet.Path, Filename);
        strcpy(mSampleSet.Format, sFormat);

        fSamplePack.read((char*)&iInput, sizeof(unsigned int));         // SampleSet: ID
        mSampleSet.ID = (unsigned char)iInput;

        fSamplePack.read((char*)&iInput, sizeof(unsigned int));         // SampleSet: Mode
        mSampleSet.Mode = (unsigned char)iInput;

        fSamplePack.read((char*)&iInput, sizeof(unsigned int));         // SampleSet: Minimum intensity
        mSampleSet.MinimumIntensity = (unsigned char)iInput;

        fSamplePack.read((char*)&iInput, sizeof(unsigned int));         // SampleSet: Maximum intensity
        mSampleSet.MaximumIntensity = (unsigned char)iInput;

        fSamplePack.read((char*)&iInput, sizeof(unsigned int));         // SampleSet: Lowest pitch
        mSampleSet.Range.LowestNote = (unsigned char)iInput;

        fSamplePack.read((char*)&iInput, sizeof(unsigned int));         // SampleSet: Highest pitch
        mSampleSet.Range.HighestNote = (unsigned char)iInput;

        sSampleSet.push_back(mSampleSet);

        // samples inside the sample set
        fSamplePack.read((char*)&iNumSamples, sizeof(unsigned int));    // SampleSet: Number of samples
        iNumberOfSamples.push_back((unsigned char)iNumSamples);

        fSamplePack.read(sReserved, 4);                                 // SampleSet: (Reserved)

        alBuffer[i] = new ALuint[iNumSamples];
        alGenBuffers(iNumSamples, &alBuffer[i][0]);

        unsigned int iSampleSize;
        char *sSampleData;

        for(j = 0; j < iNumSamples; j++)
        {
            fSamplePack.read((char*)&iSampleSize, sizeof(unsigned int));

            if(iSampleSize == 0)
                continue;

            // load sample file
            sSampleData = new char[iSampleSize];
            fSamplePack.read(sSampleData, iSampleSize);
            loadAudioData(sSampleData, iSampleSize, alBuffer[i][j]);

            // callback
            if(callback)
                callback(iTotalSamples, ++iLoaded, Data);

            delete[] sSampleData;
        }
    }

    fSamplePack.close();
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

    // set the channel free
    releaseChannel(Note.Channel, true);

    // if the channel is in any sustained list, remove it
    vChannelsSustained.removeValue(Note.Channel);
    vSustainedChannelsToRelease.removeValue(Note.Channel);

    // play the note
    iSource[Note.Channel][0] = alManager->assignSource(iID, Note.Channel);
    ALuint alSource = alManager->getSource(iSource[Note.Channel][0]);

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
    if(!alBuffer || iChannel >= iNumberOfChannels)
        return;

    // make sure that the source is still ours
    if(alManager->getEntityID(iSource[iChannel][0]) != iID || alManager->getEntityChannel(iSource[iChannel][0]) != iChannel)
        return;

    // get primary channel's source
    ALuint alSource = alManager->getSource(iSource[iChannel][0]);

    // check whether the primary channel is playing a sound
    ALint alSourceState;
    alGetSourcei(alSource, AL_SOURCE_STATE, &alSourceState);

    // if the primary channel is playing a sound, change the handler to the secondary channel and notify
    // the generator to apply a fade out to it
    if(alSourceState == AL_PLAYING)
    {
        // we need two values: the volume of the channel at the moment (initial) and another value that
        // always goes from 1.0f to 0.0f, in order to apply a fade out which doesn't depend on the initial
        // volume
        alGetSourcef(alSource, AL_GAIN, &fInitialReleaseVolume[iChannel]);
        fCurrentReleaseVolume[iChannel] = 1.0f;

        // if the secondary channel is already doing a fade out, stop it
        if(alManager->getEntityID(iSource[iChannel][1]) == iID && 
           alManager->getEntityChannel(iSource[iChannel][1]) == iChannel &&
           (bQuickRelease[iChannel] || bRelease[iChannel]))
        {
            alSourceStop(alManager->getSource(iSource[iChannel][1]));
        }

        // swap source pointers
        iSource[iChannel][1] = iSource[iChannel][0];

        // set fade out settings
        bQuickRelease[iChannel] = bQuickly;
        bRelease[iChannel] = !bQuickly;

        // add the current channel to the list to be released
        vChannelsToRelease.add(iChannel);
    }

    // if not, just release the source
    else
    {
        alManager->releaseSource(iSource[iChannel][0]);
    }
}

void MCSoundGenOpenAL::update()
{
    unsigned int iChannel;

    // a fade out will be done to the secondary channels that must be released
    for(unsigned int i = 0; i < vChannelsToRelease.size(); i++)
    {
        iChannel = vChannelsToRelease[i];

        // the source manager has given this source to another sample
        if(alManager->getEntityID(iSource[iChannel][1]) != iID || alManager->getEntityChannel(iSource[iChannel][1]) != iChannel)
        {
            bRelease[iChannel] = false;
            bQuickRelease[iChannel] = false;

            vChannelsToRelease.remove(i);
            i--;

            continue;
        }

        // next step for the fade-out
        if(fCurrentReleaseVolume[iChannel] > 0.0f)
        {
            fCurrentReleaseVolume[iChannel] -= (bQuickRelease[iChannel])? M_QUICK_RELEASE: fReleaseSpeed;
            alSourcef(alManager->getSource(iSource[iChannel][1]), AL_GAIN, 
                      fCurrentReleaseVolume[iChannel] * fInitialReleaseVolume[iChannel]);

            // check damper
            if(bDamper)
            {
                vSustainedChannelsToRelease.add(iChannel);
                vChannelsToRelease.remove(i);
                i--;
            }
        }

        // fade-out completed: stop and release the source
        else
        {
            alSourceStop(alManager->getSource(iSource[iChannel][1]));
            alManager->releaseSource(iSource[iChannel][1]);

            bRelease[iChannel] = false;
            bQuickRelease[iChannel] = false;

            vChannelsToRelease.remove(i);
            i--;
        }
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
    if(alManager->getEntityID(iSource[Channel][0]) == iID && alManager->getEntityChannel(iSource[Channel][0]) == Channel)
        alSourcef(alManager->getSource(iSource[Channel][0]), AL_PITCH, pow(2, Cents / 1200.0f));
}

void MCSoundGenOpenAL::setIntensity(unsigned char Channel, unsigned char Intensity)
{
    if(alManager->getEntityID(iSource[Channel][0]) == iID && alManager->getEntityChannel(iSource[Channel][0]) == Channel &&
       iSampleSet[Channel] > -1)
    {
        alSourcef(alManager->getSource(iSource[Channel][0]), AL_GAIN, 
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
        for(j = 0; j < 2; j++)
        {
            if(alManager->getEntityID(iSource[i][j]) == iID && alManager->getEntityChannel(iSource[i][j]) == i)
            {
                alSource = alManager->getSource(iSource[i][j]);
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
        for(j = 0; j < 2; j++)
        {
            if(alManager->getEntityID(iSource[i][j]) == iID && alManager->getEntityChannel(iSource[i][j]) == i)
            {
                alSource = alManager->getSource(iSource[i][j]);
                alGetSourcei(alSource, AL_SOURCE_STATE, &alSourceState);

                if(alSourceState == AL_PLAYING)
                    alSource3f(alSource, AL_DIRECTION, al3DDirection[0], al3DDirection[1], al3DDirection[2]);
            }
        }
    }
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
