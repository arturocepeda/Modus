
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

#include "audio.openal.h"
#include <stdio.h>

ALCcontext* CAudio::alContext;
ALCdevice* CAudio::alDevice;

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

MCSoundGenAudio* CAudio::createSoundGen(unsigned int ID, unsigned int NumberOfChannels, bool Sound3D)
{
    return new MCSoundGenOpenAL(ID, NumberOfChannels, Sound3D);
}
