
////////////////////////////////////////////////////////////////////////
//
//  Modus
//  C++ Music Library
//  (Examples)
//
//  Arturo Cepeda
//  July 2012
//
//  --- audio.h ---
//
////////////////////////////////////////////////////////////////////////

#ifndef _AUDIO_H_
#define _AUDIO_H_

#include "msoundgen.h"

class CAudio
{
public:
    static void init();
    static void release();
    static void update();

    static Modus::MCSoundGenAudio* createSoundGen(unsigned int ID, unsigned int NumberOfChannels, bool Sound3D);
};

#endif
