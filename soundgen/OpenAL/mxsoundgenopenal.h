
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

#ifndef _MXSOUNDGENOPENAL_H_
#define _MXSOUNDGENOPENAL_H_

#include "msoundgen.h"
#include "mxopenalsourcemanager.h"

#ifdef __APPLE__
#include "TargetConditionals.h"
#if TARGET_OS_IPHONE    
#define __iOS__
#elif TARGET_IPHONE_SIMULATOR
#define __iOS__
#endif
#endif


//
//  Ogg Vorbis (Win32, MacOSX, Linux)
//
#ifndef __iOS__
#include "./../externals/OggVorbis/include/vorbisfile.h"

#define OV_BUFFER_SIZE 1024

struct SOGGFile
{
    char* Data;
    unsigned int Size;
    unsigned int Cursor;
};

size_t ovRead(void* pDestination, size_t iSize, size_t iNumMembers, void* pDataSource);
int ovSeek(void* pDataSource, ogg_int64_t iOffset, int iWhence);
long ovTell(void* pDataSource);


//
//  AudioToolbox (iOS)
//
#else
#include <AudioToolbox/AudioToolbox.h>
#include <AudioToolbox/ExtendedAudioFile.h>
#endif


//
//  Sound generator
//
class MCSoundGenOpenAL : public MCSoundGenAudioMultipleChannel
{
private:
    int iID;
    MCOpenALSourceManager* alManager;

    ALuint** alBuffer;

    ALfloat al3DPosition[3];
    ALfloat al3DDirection[3];

    void releaseChannel(unsigned char iChannel, bool bQuickly);

#ifndef __iOS__
    void loadWAVData(const char* sData, unsigned int iSize, ALuint alBuffer);
    void loadWAVFile(const char* sFilename, ALuint alBuffer);

    void loadOGGData(const char* sData, unsigned int iSize, ALuint alBuffer);
    void loadOGGFile(const char* sFilename, ALuint alBuffer);
#else
    void loadAudioData(const char* sData, unsigned int iSize, ALuint alBuffer);
    void loadAudioFile(NSString* sFilename, ALuint alBuffer, bool bFromResources = true);
#endif

public:
    MCSoundGenOpenAL(unsigned int NumberOfChannels, bool Sound3D, int ID, MCOpenALSourceManager* OpenALSourceManager);
    ~MCSoundGenOpenAL();

    void addSampleSet(MSSampleSet& SampleSet);
    void loadSamples();
    void loadSamplePack(std::istream& Stream, 
                        void (*callback)(unsigned int TotalSamples, unsigned int Loaded, void* Data) = NULL,
                        void* Data = NULL);
    void unloadSamples();

    void playNote(MSNote& Note);
    void update();

    void get3DPosition(float* X, float* Y, float* Z);
    void get3DDirection(float* X, float* Y, float* Z);

    void setBending(unsigned char Channel, int Cents);
    void setIntensity(unsigned char Channel, unsigned char Intensity);
    void set3DPosition(float X, float Y, float Z);
    void set3DDirection(float X, float Y, float Z);
};

#endif
