
////////////////////////////////////////////////////////////////////////
//
//  Modus
//  C++ Music Library
//  Sample Application
//
//  Arturo Cepeda
//
////////////////////////////////////////////////////////////////////////

#include "audio.h"

CAudio::CAudio()
{
   alDevice = alcOpenDevice(NULL);
   alContext = alcCreateContext(alDevice, NULL);
   alcMakeContextCurrent(alContext);
}

CAudio::~CAudio()
{
   alcMakeContextCurrent(NULL);
   alcDestroyContext(alContext);
   alcCloseDevice(alDevice);
}
