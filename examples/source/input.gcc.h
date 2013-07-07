
////////////////////////////////////////////////////////////////////////
//
//  Modus
//  C++ Music Library
//  (Examples)
//
//  Arturo Cepeda Pérez
//
//  --- input.gcc.h ---
//
//  --------------------------------------------------------------------
//
//  Keyboard input management functions for gcc
//
//  Original code from the book "Beginning Linux Programming" (Wrox)
//  Modified by Arturo Cepeda Pérez
//
////////////////////////////////////////////////////////////////////////

#ifndef _INPUT_H_
#define _INPUT_H_

#include <termios.h>
#include <unistd.h>

static struct termios initial_settings, new_settings;
static int peek_character = -1;

class Input
{
public:
    static void init()
    {
        tcgetattr(0, &initial_settings);

        new_settings = initial_settings;
        new_settings.c_lflag &= ~ICANON;
        new_settings.c_lflag &= ~ECHO;
        new_settings.c_lflag &= ~ISIG;
        new_settings.c_cc[VMIN] = 1;
        new_settings.c_cc[VTIME] = 0;

        tcsetattr(0, TCSANOW, &new_settings);
    }

    static void close()
    {
        tcsetattr(0, TCSANOW, &initial_settings);
    }

    static bool keyPressed()
    {
        unsigned char ch;
        int nread;
    
        if(peek_character != -1) 
            return true;
    
        new_settings.c_cc[VMIN] = 0;
        tcsetattr(0, TCSANOW, &new_settings);
    
        nread = read(0, &ch, 1);
    
        new_settings.c_cc[VMIN] = 1;
        tcsetattr(0, TCSANOW, &new_settings);
    
        if(nread == 1) 
        {
            peek_character = ch;
            return true;
        }
    
        return false;
    }

    static char getKey()
    {
        char ch;
    
        if(peek_character != -1) 
        {
            ch = peek_character;
            peek_character = -1;        
            return ch;
        }
    
        read(0, &ch, 1);
    
        return ch;
    }
};

#endif
