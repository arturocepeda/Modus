
////////////////////////////////////////////////////////////////////////
//
//  Modus
//  C++ Music Library
//  (Examples)
//
//  Arturo Cepeda
//  July 2012
//
//  --- audio.irrklang.cpp ---
//
//  --------------------------------------------------------------------
//
//  Audio engine: 
//
//  irrKlang
//  Copyright (c) Nikolaus Gebhardt / Ambiera 2001-2009  
//
////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include "audio.h"

#include "irrKlang/mxsoundgenirrklang.h"

#ifdef _MSC_VER
#pragma comment(lib, ".\\..\\..\\..\\soundgen\\externals\\irrKlang\\lib.win32\\irrKlang.lib")
#endif

using namespace Modus;

irrklang::ISoundEngine* ikEngine;

void CAudio::init()
{
    ikEngine = irrklang::createIrrKlangDevice();
}

void CAudio::release()
{
    ikEngine->drop();
}

void CAudio::update()
{
    ikEngine->update();
}

MCSoundGenAudio* CAudio::createSoundGen(unsigned int ID, unsigned int NumberOfChannels, bool Sound3D)
{
    return new MCSoundGenirrKlang(ID, NumberOfChannels, Sound3D, ikEngine);
}
