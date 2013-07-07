
//////////////////////////////////////////////////////////////////
//
//  Arturo Cepeda Pérez
//  iOS Game Engine
//
//  Device static class
//
//  --- GEDevice.mm ---
//
//////////////////////////////////////////////////////////////////


#include "GEDevice.h"

bool GEDevice::iPhone()
{
   return [[UIDevice currentDevice].model hasPrefix:@"iPhone"];
}

bool GEDevice::displayRetina()
{
   return ([[UIScreen mainScreen] respondsToSelector:@selector(scale)] && 
           [[UIScreen mainScreen] scale] == 2.0);
}

int GEDevice::getScreenSizeX()
{
   if(iPhone())
      return 640;
   else 
      return displayRetina()? 1536: 768;  
}

int GEDevice::getScreenSizeY()
{
   if(iPhone())
      return 960;
   else
      return displayRetina()? 2048: 1024;
}

int GEDevice::getTouchPadSizeX()
{
   if(iPhone())
      return 320;
   else
      return 768;
}

int GEDevice::getTouchPadSizeY()
{
   if(iPhone())
      return 480;
   else
      return 1024;
}

float GEDevice::getAspectRatio()
{
   if(iPhone())
      return 1.5f;
   else
      return 1.33f;
}

const char* GEDevice::getResourcePath(NSString* ResourceName)
{
   NSString* sFilePath = [[[NSBundle mainBundle] resourcePath] stringByAppendingPathComponent:ResourceName];   
   return [sFilePath UTF8String];   
}
