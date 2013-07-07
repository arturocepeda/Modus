
////////////////////////////////////////////////////////////////////////
//
//  Modus v0.51
//  C++ Music Library
//  [Sound Generator]
//
//  Copyright (c) 2012 Arturo Cepeda
//
//  --------------------------------------------------------------------
//
//  External MIDI library: 
//
//  RtMidi - Realtime MIDI i/o C++ classes
//  Copyright (c) 2003-2012 Gary P. Scavone
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

#ifndef _MXSOUNDGENMIDI_H_
#define _MXSOUNDGENMIDI_H_

#include "msoundgen.h"
#include "externals/RtMidi/RtMidi.h"

class MCSoundGenMIDI : public MCSoundGen
{
private:
    RtMidiOut* mDevice;
    std::vector<unsigned char> mMessage;
    unsigned char iMIDIChannel;
    unsigned char* iPitch;

    void releaseResonance();

public:
    MCSoundGenMIDI(unsigned int NumberOfChannels, RtMidiOut* MIDIDevice, 
                   unsigned char MIDIChannel, unsigned char MIDIProgram);
    ~MCSoundGenMIDI();

    void playNote(MSNote& Note);
    void releaseNote(unsigned char Channel);
    void update();

    void setBending(unsigned char Channel, int Cents);
    void setIntensity(unsigned char Channel, unsigned char Intensity);
};

#endif
