
////////////////////////////////////////////////////////////////////////
//
//  Modus
//  C++ Music Library
//  (Examples)
//
//  Arturo Cepeda
//  July 2012
//
//  --- audio.fmod.cpp ---
//
//  --------------------------------------------------------------------
//
//  Audio engine: 
//
//  FMOD Sound System
//  Copyright (c) Firelight Technologies Pty, Ltd., 1994-2012
//
////////////////////////////////////////////////////////////////////////

#include "audio.fmod.h"
#include <stdio.h>
#include <stdlib.h>

FMOD::System* CAudio::fSystem;

void CAudio::ERRCHECK(FMOD_RESULT result)
{
   if(result != FMOD_OK)
   {
      printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
      exit(-1);
   }
}

void CAudio::init()
{
    FMOD_RESULT result;
   
    // create a system object and initialize.
    result = FMOD::System_Create(&fSystem);
    ERRCHECK(result);

    // set a small DSP buffer size to achieve low latency
    fSystem->setDSPBufferSize(FMOD_DSP_BUFFER_SIZE, 4);

#ifdef __linux__
    // in Linux systems: set ALSA as output type
    fSystem->setOutput(FMOD_OUTPUTTYPE_ALSA);
#endif

    // initialize FMOD system
    result = fSystem->init(FMOD_MAX_CHANNELS, FMOD_INIT_NORMAL, NULL);
    ERRCHECK(result);
}

void CAudio::update()
{
    fSystem->update();
}

void CAudio::release()
{    
    fSystem->close();
    fSystem->release();
}

FMOD::System* CAudio::getSoundSystem()
{
    return fSystem;
}
