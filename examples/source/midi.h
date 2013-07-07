
////////////////////////////////////////////////////////////////////////
//
//  Modus
//  C++ Music Library
//  (Examples)
//
//  Arturo Cepeda Pérez
//
//  --- midi.h ---
//
////////////////////////////////////////////////////////////////////////

#ifndef _MIDI_H_
#define _MIDI_H_

#include "externals/RtMidi/RtMidi.h"

//
//  Win32
//
#ifdef _WIN32

//
//  NOTE: it is necessary to add the __WINDOWS_MM__ preprocessor definition to the project in order
//  to let RtMidi know which library we would like to use, as well as link with the winmm.lib
//  library
//
RtMidi::Api rtMidiApi = RtMidi::WINDOWS_MM;

#ifdef _MSC_VER
#pragma comment(lib, "winmm.lib")
#endif

#endif



//
//  MacOS X
//
#ifdef __APPLE__

//
//  NOTE: it is necessary to add the __MACOSX_CORE__ preprocessor definition to the project in order
//  to let RtMidi know which library we would like to use, as well as link with the CoreMidi,
//  CoreAudio and CoreFoundation frameworks
//
RtMidi::Api rtMidiApi = RtMidi::MACOSX_CORE;

#endif



//
//  Linux
//
#ifdef __linux__

//
//  NOTE: it is necessary to add the __LINUX_ALSA__ preprocessor definition to the project in order
//  to let RtMidi know which library we would like to use, as well as link with the pthread and
//  asound libraries
//
RtMidi::Api rtMidiApi = RtMidi::LINUX_ALSA;

#endif


#endif
