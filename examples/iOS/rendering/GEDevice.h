
//////////////////////////////////////////////////////////////////
//
//  Arturo Cepeda Pérez
//  iOS Game Engine
//
//  Device static class
//
//  --- GEDevice.h ---
//
//////////////////////////////////////////////////////////////////


#ifndef _GEDEVICE_H_
#define _GEDEVICE_H_

class GEDevice
{
private:
   
public:
   static bool iPhone();         // iPhone or iPad
   static bool displayRetina();  // iPad (3rd generation)
   
   static int getScreenSizeX();
   static int getScreenSizeY();
   
   static int getTouchPadSizeX();
   static int getTouchPadSizeY();
   
   static float getAspectRatio();
   
   static const char* getResourcePath(NSString* ResourceName);
};

#endif
