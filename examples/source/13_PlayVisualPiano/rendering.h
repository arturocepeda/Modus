
////////////////////////////////////////////////////////////////////////
//
//  Modus
//  C++ Music Library
//  (Examples)
//
//  Arturo Cepeda Pérez
//  July 2012
//
////////////////////////////////////////////////////////////////////////

#ifndef _RENDERING_H_
#define _RENDERING_H_

#include "main.h"

#define WHITEKEY_WIDTH  17
#define WHITEKEY_HEIGHT 69

#define BLACKKEY_WIDTH  14
#define BLACKKEY_HEIGHT 42

#define PEDAL_WIDTH     66
#define PEDAL_HEIGHT    32

#define PIANO_Y         168
                        
class CRendering
{
private:
    static SGDI* sGDI;
    static char sHeader[32];
    static char sDevice[128];

    static void drawText(int X, int Y, const char* sText, HFONT hFont, COLORREF cColor);
    static void drawBMP(int X, int Y, int iWidth, int iHeight, HDC hdc, HBITMAP bmp);

public:
    static void init(SGDI* GDI);
    static void render(SGlobal* Global);
    static void release();
};

#endif
