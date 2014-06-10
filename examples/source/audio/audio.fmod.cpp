
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

#include "audio.h"
#include "./../soundgen/FMOD/mxsoundgenfmod.h"
#include <stdio.h>
#include <stdlib.h>

// try with higher values in case it doesn't sound properly
#define FMOD_DSP_BUFFER_SIZE 512
#define FMOD_CHANNELS 32

#ifdef _MSC_VER
#pragma comment(lib, ".\\..\\..\\..\\soundgen\\externals\\FMOD\\lib.win32\\fmodex_vc.lib")
#endif

FMOD::System* fSystem;

void ERRCHECK(FMOD_RESULT result)
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
    result = fSystem->init(FMOD_CHANNELS, FMOD_INIT_NORMAL, NULL);
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

MCSoundGenAudio* CAudio::createSoundGen(unsigned int ID, unsigned int NumberOfChannels, bool Sound3D)
{
    return new MCSoundGenFMOD(NumberOfChannels, Sound3D, fSystem);
}
