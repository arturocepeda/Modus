
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
#include "audio.irrklang.h"

irrklang::ISoundEngine* CAudio::iEngine;

void CAudio::init()
{
    iEngine = irrklang::createIrrKlangDevice();
}

void CAudio::release()
{
    iEngine->drop();
}

void CAudio::update()
{
    iEngine->update();
}

irrklang::ISoundEngine* CAudio::getSoundSystem()
{
    return iEngine;
}
