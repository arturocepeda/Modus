
////////////////////////////////////////////////////////////////////////
//
//  Modus v0.60
//  C++ Music Library
//
//  Copyright (c) 2012-2015 Arturo Cepeda Pérez
//
//  --------------------------------------------------------------------
//
//  This file is part of Modus. Permission is hereby granted, free 
//  of charge, to any person obtaining a copy of this software and 
//  associated documentation files (the "Software"), to deal in the
//  Software without restriction, including without limitation the 
//  rights to use, copy, modify, merge, publish, distribute, 
//  sublicense, and/or sell copies of the Software, and to permit 
//  persons to whom the Software is furnished to do so, subject to 
//  the following conditions:
//
//  The above copyright notice and this permission notice shall be 
//  included in all copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY 
//  KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE 
//  WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR 
//  PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS 
//  OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR 
//  OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR 
//  OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
//  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
////////////////////////////////////////////////////////////////////////

#include "mutils.h"
#include <string.h>
#include <ctype.h>

using namespace Modus;

//
//  CLine
//
CLine::CLine(double x0, double y0, double x1, double y1)
    : Dx(x1 - x0)
    , Dy(y1 - y0)
    , vertical(false)
    , horizontal(false)
{
    if(Dx == 0.0)
    {
        vertical = true;
        px0 = x0;
    }
    else if(Dy == 0.0)
    {
        horizontal = true;
        py0 = y0;
    }
    else
    {
        m = Dy / Dx;
        b = y0 - (m * x0);
    }
}

CLine::~CLine()
{
}

bool CLine::can_calculate_y()
{
    // it doesn't exist any solution if Dx = 0 (no solution or infinite)
    if(vertical)
        return false;
    else
        return true;
}

double CLine::y(double x)
{
    if(horizontal)
        return py0;

    return (m * x) + b;
}

bool CLine::can_calculate_x()
{
    // it doesn't exist any solution if Dy = 0 (no solution or infinite)
    if(horizontal)
        return false;
    else
        return true;
}

double CLine::x(double y)
{
    if(vertical)
        return px0;

    return (y - b) / m;
}


// 
//  Simple numerical functions
//
bool Modus::positive(float number)
{
    return (number >= 0)? true: false;
}

bool Modus::same_sign(float a, float b)
{
    if(a >= 0.0 && b >= 0.0)
        return true;
    else if(a < 0.0 && b < 0.0)
        return true;
    else
        return false;
}


//
//  String functions
//
void Modus::strtoupper(char* string)
{
    for(unsigned int i = 0; i < strlen(string); i++)
        string[i] = toupper(string[i]);
}

void Modus::strtolower(char* string)
{
    for(unsigned int i = 0; i < strlen(string); i++)
        string[i] = tolower(string[i]);
}


//
//  Random functions
//
bool Modus::eventOccurs(int Percentage)
{
    return ((rand() % 100) < Percentage);
}
