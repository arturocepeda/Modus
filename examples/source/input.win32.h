
////////////////////////////////////////////////////////////////////////
//
//  Modus
//  C++ Music Library
//  (Examples)
//
//  Arturo Cepeda Pérez
//
//  --- input.win32.h ---
//
////////////////////////////////////////////////////////////////////////

#ifndef _INPUT_H_
#define _INPUT_H_

#include <conio.h>

class Input
{
public:
    static void init() {}
    static void close() {}
    static bool keyPressed() { return (_kbhit() != 0); }
    static char getKey() { return (char)_getch(); }
};

#endif
