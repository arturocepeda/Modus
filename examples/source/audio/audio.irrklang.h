
////////////////////////////////////////////////////////////////////////
//
//  Modus
//  C++ Music Library
//  (Examples)
//
//  Arturo Cepeda
//  July 2012
//
//  --- audio.irrklang.h ---
//
//  --------------------------------------------------------------------
//
//  Audio engine: 
//
//  irrKlang
//  Copyright (c) Nikolaus Gebhardt / Ambiera 2001-2009  
//
////////////////////////////////////////////////////////////////////////

#ifndef _AUDIO_IRRKLANG_H_
#define _AUDIO_IRRKLANG_H_

#include "externals/irrKlang/include/irrKlang.h"
#include "mtypes.h"

#ifdef _MSC_VER
#pragma comment(lib, ".\\..\\..\\..\\soundgen\\externals\\irrKlang\\lib.win32\\irrKlang.lib")
#endif

class CAudio
{
private:
    static irrklang::ISoundEngine* iEngine;

public:
    static void init();
    static void release();
    static void update();

    static irrklang::ISoundEngine* getSoundSystem();
};

#endif
