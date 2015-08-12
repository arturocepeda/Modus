
////////////////////////////////////////////////////////////////////////
//
//  Modus
//  C++ Music Library
//  (Examples)
//
//  Arturo Cepeda
//  July 2012
//
//  --- audio.openal.cpp ---
//
//  --------------------------------------------------------------------
//
//  Audio engine: 
//
//  OpenAL - Open Audio Library
//
////////////////////////////////////////////////////////////////////////

#include "audio.h"
#include <stdio.h>

#include "OpenAL/mxsoundgenopenal.h"

#ifdef _MSC_VER
#pragma comment(lib, ".\\..\\..\\..\\soundgen\\externals\\OpenAL\\lib.win32\\OpenAL32.lib")
#ifdef _DEBUG
#pragma comment(lib, ".\\..\\..\\..\\soundgen\\externals\\OggVorbis\\lib.win32\\libogg_static_d.lib")
#pragma comment(lib, ".\\..\\..\\..\\soundgen\\externals\\OggVorbis\\lib.win32\\libvorbis_static_d.lib")
#pragma comment(lib, ".\\..\\..\\..\\soundgen\\externals\\OggVorbis\\lib.win32\\libvorbisfile_static_d.lib")
#else
#pragma comment(lib, ".\\..\\..\\..\\soundgen\\externals\\OggVorbis\\lib.win32\\libogg_static.lib")
#pragma comment(lib, ".\\..\\..\\..\\soundgen\\externals\\OggVorbis\\lib.win32\\libvorbis_static.lib")
#pragma comment(lib, ".\\..\\..\\..\\soundgen\\externals\\OggVorbis\\lib.win32\\libvorbisfile_static.lib")
#endif
#endif

using namespace Modus;

ALCcontext* alContext;
ALCdevice* alDevice;

void CAudio::init()
{
    alDevice = alcOpenDevice(NULL);

    if(alDevice) 
    {
        alContext = alcCreateContext(alDevice, NULL);
        alcMakeContextCurrent(alContext);
        alListener3f(AL_POSITION, 0.0f, 0.0f, 0.0f);
    }
}

void CAudio::release()
{
    alContext = alcGetCurrentContext();
    alDevice = alcGetContextsDevice(alContext);
    alcMakeContextCurrent(NULL);
    alcDestroyContext(alContext);
    alcCloseDevice(alDevice);
}

void CAudio::update()
{
}

MCSoundGenAudio* CAudio::createSoundGen(unsigned int ID, unsigned int NumberOfChannels, bool Sound3D)
{
    return new MCSoundGenOpenAL(ID, NumberOfChannels, Sound3D);
}
