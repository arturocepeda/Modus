
////////////////////////////////////////////////////////////////////////
//
//  Modus
//  C++ Music Library
//  Sample Application
//
//  Arturo Cepeda
//
////////////////////////////////////////////////////////////////////////

#ifndef _CAUDIO_H_
#define _CAUDIO_H_

#include <OpenAL/al.h>
#include <OpenAL/alc.h>

class CAudio
{
private:
   ALCdevice* alDevice;
   ALCcontext* alContext;
   
public:
   CAudio();
   ~CAudio();
};

#endif
