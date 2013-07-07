
////////////////////////////////////////////////////////////////////////
//
//  Modus
//  C++ Music Library
//  (Examples)
//
//  Arturo Cepeda
//  July 2012
//
//  --- audio.openal.h ---
//
//  --------------------------------------------------------------------
//
//  Audio engine: 
//
//  OpenAL - Open Audio Library
//
////////////////////////////////////////////////////////////////////////

#ifndef _AUDIO_OPENAL_H_
#define _AUDIO_OPENAL_H_

#include "externals/OpenAL/include/al.h"
#include "externals/OpenAL/include/alc.h"

// try with a lower value in case it doesn't work properly
#define OPENAL_SOURCES 32

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

class CAudio
{
private:
    static ALCcontext* alContext;
    static ALCdevice* alDevice;

public:
    static void init();
    static void release();
};

#endif
