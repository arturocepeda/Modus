
////////////////////////////////////////////////////////////////////////
//
//  Modus v0.54
//  C++ Music Library
//
//  Copyright (c) 2012-2014 Arturo Cepeda Pérez
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

///\file

#ifndef _MUTILS_H_
#define _MUTILS_H_

#include <stdlib.h>
#include <vector>

//
//  CUniqueVector
//
template<typename T>
class CUniqueVector
{
private:
    std::vector<T> vVector;

public:
    T& operator[](const unsigned int Index)
    {
        return vVector[Index];
    }

    void add(const T& Value)
    {
        // the vector is empty
        if(vVector.empty())
        {
            vVector.push_back(Value);
            return;    
        }

        int iFirst = 0;
        int iLast = vVector.size() - 1;

        if(Value == vVector[iFirst] || Value == vVector[iLast])
            return;

        // first element
        if(Value < vVector[iFirst])
        {
            vVector.insert(vVector.begin(), Value);
        }

        // last element
        else if(Value > vVector[iLast])
        {
            vVector.push_back(Value);
        }
       
        // another position in the middle
        else
        {
            int iMiddle;

            do
            {
                iMiddle = (iFirst + iLast) / 2;

                if(Value == vVector[iMiddle])
                    return;
                else if(Value < vVector[iMiddle])
                    iLast = iMiddle - 1;
                else
                    iFirst = iMiddle + 1;

            } while(iFirst <= iLast);

            vVector.insert(vVector.begin() + iFirst, Value);
        }
    }

    void remove(unsigned int Index)
    {
        vVector.erase(vVector.begin() + Index);
    }

    void removeLast()
    {
        vVector.pop_back();
    }

    void removeValue(const T& Value)
    {
        // the vector is empty
        if(vVector.empty())
            return;    

        int iFirst = 0;
        int iLast = vVector.size() - 1;

        // first element
        if(Value == vVector[iFirst])
        {
            vVector.erase(vVector.begin());
        }

        // last element
        else if(Value == vVector[iLast])
        {
            vVector.pop_back();
        }
       
        // another position in the middle
        else
        {
            int iMiddle;

            do
            {
                iMiddle = (iFirst + iLast) / 2;

                if(Value == vVector[iMiddle])
                {
                    vVector.erase(vVector.begin() + iMiddle);
                    return;
                }
                else if(Value < vVector[iMiddle])
                {
                    iLast = iMiddle - 1;
                }
                else
                {
                    iFirst = iMiddle + 1;
                }

            } while(iFirst <= iLast);
        }
    }

    void clear()
    {
        vVector.clear();
    }

    bool empty()
    {
        return (vVector.size() == 0);
    }

    unsigned int size()
    {
        return vVector.size();
    }
};


// 
//  CLine
//
class CLine
{
private:
    double px0;
    double py0;

    double Dx;
    double Dy;

    double m;
    double b;

    bool vertical;
    bool horizontal;

public:
    CLine(double x0, double y0, double x1, double y1);
    ~CLine();

    bool can_calculate_y();
    double y(double x);

    bool can_calculate_x();
    double x(double y);
};


//
//  Simple numerical functions
//
bool positive(float number);
bool same_sign(float a, float b);


//
//  String functions
//
void strtolower(char* string);


//
//  Random functions
//
bool eventOccurs(int Percentage);


#endif
