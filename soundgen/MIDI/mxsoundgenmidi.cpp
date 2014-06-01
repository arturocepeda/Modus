
////////////////////////////////////////////////////////////////////////
//
//  Modus
//  C++ Music Library
//  [Sound Generator]
//
//  Copyright (c) 2012-2014 Arturo Cepeda
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

#include "mxsoundgenmidi.h"
#include "mutils.h"

MCSoundGenMIDI::MCSoundGenMIDI(unsigned int NumberOfChannels, RtMidiOut* MIDIDevice, 
                               unsigned char MIDIChannel, unsigned char MIDIProgram)
{
    iNumberOfChannels = NumberOfChannels;
    mDevice = MIDIDevice;
    iMIDIChannel = MIDIChannel - 1;
    fVolume = 1.0f;
    fPan = 0.0f;
    bDamper = false;

    // initialize vector for MIDI messages (3 bytes)
    mMessage.clear();
    mMessage.push_back(0);
    mMessage.push_back(0);
    mMessage.push_back(0);

    iPitch = new unsigned char[iNumberOfChannels];
    memset(iPitch, 0, iNumberOfChannels);

    mMessage[0] = 0xC0 + iMIDIChannel;      // program change message
    mMessage[1] = MIDIProgram - 1;          // program
    mDevice->sendMessage(&mMessage);
}

MCSoundGenMIDI::~MCSoundGenMIDI()
{
    delete[] iPitch;
}

void MCSoundGenMIDI::playNote(MSNote& Note)
{
    if(Note.Channel >= iNumberOfChannels)
        return;

    // if the channel is already sounding, release it
    if(iPitch[Note.Channel] > 0)
    {
        mMessage[0] = 0x90 + iMIDIChannel;  // note-on message
        mMessage[1] = iPitch[Note.Channel]; // pitch
        mMessage[2] = 0;                    // velocity
        mDevice->sendMessage(&mMessage);
    }

    // reset pitch wheel
    mMessage[0] = 0xE0 + iMIDIChannel;      // pitch wheel change message
    mMessage[1] = 0;                        // least significant 7 bits
    mMessage[2] = 0x40;                     // more significant 7 bits
    mDevice->sendMessage(&mMessage);
    
    // volume -> value [0.0f, 1.0f]
    mMessage[0] = 0xB0 + iMIDIChannel;      // channel mode messages
    mMessage[1] = 0x07;                     // volume
    mMessage[2] = (unsigned char)(fVolume * 127.0f);
    mDevice->sendMessage(&mMessage);

    // pan -> value [-1.0f, 1.0f]
    mMessage[1] = 0x0A;                     // pan
    mMessage[2] = (unsigned char)((fPan + 1.0f) * 63.5f);
    mDevice->sendMessage(&mMessage);

    // play the note
    mMessage[0] = 0x90 + iMIDIChannel;      // note-on message
    mMessage[1] = Note.Pitch;               // pitch
    mMessage[2] = Note.Intensity;           // velocity
    mDevice->sendMessage(&mMessage);

    // attach the new pitch to the note channel
    iPitch[Note.Channel] = Note.Pitch;
}

void MCSoundGenMIDI::releaseNote(unsigned char Channel)
{
    if(Channel >= iNumberOfChannels)
        return;

    // damper on: don't release the note, but put it in the sustained list
    if(bDamper)
    {
        vChannelsSustained.add(Channel);
        return;
    }

    // release the note
    mMessage[0] = 0x90 + iMIDIChannel;      // note-on message
    mMessage[1] = iPitch[Channel];          // pitch
    mMessage[2] = 0;                        // velocity
    mDevice->sendMessage(&mMessage);

    // clean pitch register
    iPitch[Channel] = 0;
}

void MCSoundGenMIDI::releaseResonance()
{
}

void MCSoundGenMIDI::update()
{
}

void MCSoundGenMIDI::setBending(unsigned char Channel, int Cents)
{
    unsigned int iValue = 8192;

    CLine cLine(-200, 0, 200, 16383);

    if(cLine.can_calculate_y())
        iValue = (unsigned int)cLine.y(Cents);

    mMessage[0] = 0xE0 + iMIDIChannel;      // pitch wheel change message
    mMessage[1] = iValue & 0x007f;          // least significant 7 bits
    mMessage[2] = (iValue & 0x3f80) >> 7;   // more significant 7 bits
    mDevice->sendMessage(&mMessage);
}

void MCSoundGenMIDI::setIntensity(unsigned char Channel, unsigned char Intensity)
{
    mMessage[0] = 0xA0 + iMIDIChannel;      // polyphonic key pressure (aftertouch)
    mMessage[1] = iPitch[Channel];          // pitch
    mMessage[2] = Intensity;                // new intensity
    mDevice->sendMessage(&mMessage);
}