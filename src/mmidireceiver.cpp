
////////////////////////////////////////////////////////////////////////
//
//  Modus v0.53
//  C++ Music Library
//
//  Copyright (c) 2012-2013 Arturo Cepeda Pérez
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

#include "mmidireceiver.h"
#include "mutils.h"

MCMIDIReceiver::MCMIDIReceiver(MCInstrument* Instrument)
    : mInstrument(Instrument)
    , iMinPitch(Instrument->getRange().LowestNote)
    , iMaxPitch(Instrument->getRange().HighestNote)
    , iMode(0)
    , iRangeSize(Instrument->getRangeSize())
{
    iAttachedChannel = new unsigned char[iRangeSize];

    memset(iAttachedChannel, 0, iRangeSize);
    memset(bMIDIChannel, 0, 16);
}

MCMIDIReceiver::MCMIDIReceiver(MCInstrument* Instrument, unsigned char LowestPitch, unsigned char HighestPitch)
    : mInstrument(Instrument)
    , iMode(0)
{
    if(LowestPitch < HighestPitch)
    {
        iMinPitch = LowestPitch;
        iMaxPitch = HighestPitch;
    }
    else
    {
        iMinPitch = HighestPitch;
        iMaxPitch = LowestPitch;
    }

    iRangeSize = iMaxPitch - iMinPitch + 1;
    iAttachedChannel = new unsigned char[iRangeSize];

    memset(iAttachedChannel, 0, iRangeSize);
    memset(bMIDIChannel, 0, 16);
}

MCMIDIReceiver::~MCMIDIReceiver()
{
    delete[] iAttachedChannel;
}

void MCMIDIReceiver::listenToMIDIChannel(unsigned char MIDIChannel)
{
    if(MIDIChannel >= 1 && MIDIChannel <= 16)
        bMIDIChannel[MIDIChannel - 1] = true;
}

void MCMIDIReceiver::listenToAllMIDIChannels()
{
    memset(bMIDIChannel, 1, 16);
}

void MCMIDIReceiver::ignoreMIDIChannel(unsigned char MIDIChannel)
{
    if(MIDIChannel >= 1 && MIDIChannel <= 16)
        bMIDIChannel[MIDIChannel - 1] = false;
}

void MCMIDIReceiver::ignoreAllMIDIChannels()
{
    memset(bMIDIChannel, 0, 16);
}

void MCMIDIReceiver::attachPitchToChannel(unsigned char Pitch, unsigned char Channel)
{
    if(Pitch >= iMinPitch && Pitch <= iMaxPitch)
        iAttachedChannel[Pitch - iMinPitch] = Channel;
}

void MCMIDIReceiver::attachPitchesToChannel(unsigned char FirstPitch, unsigned char LastPitch, unsigned char Channel)
{
    for(int i = FirstPitch; i <= LastPitch; i++)
        attachPitchToChannel(i, Channel);
}

void MCMIDIReceiver::attachAllPitchesToChannel(unsigned char Channel)
{
    for(int i = 0; i < iRangeSize; i++)
        iAttachedChannel[i] = Channel;
}

void MCMIDIReceiver::attachAllPitchesToDifferentChannels()
{
    unsigned char iChannel = 0;

    for(int i = 0; i < iRangeSize; i++)
        iAttachedChannel[i] = iChannel++;
}

void MCMIDIReceiver::message(unsigned char Byte0, unsigned char Byte1, unsigned char Byte2)
{
    unsigned char iMIDIChannel = Byte0 & 0x0F;
    Byte0 = Byte0 & 0xF0;
    
    if(!bMIDIChannel[iMIDIChannel])
        return;

    // NOTE ON
    if(Byte0 == 0x90)
    {
        unsigned char iPitch = Byte1;

        if(iPitch < iMinPitch || iPitch > iMaxPitch)
            return;

        unsigned char iVelocity = Byte2;

        // note on with velocity 0 --> note off
        if(iVelocity == 0)
        {
            mInstrument->release(iAttachedChannel[iPitch - iMinPitch]);
            return;
        }

        mNote.Pitch = iPitch;
        mNote.Channel = iAttachedChannel[iPitch - iMinPitch];
        mNote.Intensity = iVelocity;
        mNote.Mode = iMode;
        mNote.Duration = 0;

        mInstrument->playNote(mNote);
    }
    
    // NOTE OFF
    else if(Byte0 == 0x80)
    {
        unsigned char iPitch = Byte1;
        mInstrument->release(iAttachedChannel[iPitch - iMinPitch]);
    }
    
    // CONTROL
    else if(Byte0 == 0xB0)
    {
        // damper
        if(Byte1 == 0x40)
            mInstrument->setDamper(Byte2 >= 64);
    }

    // PITCH WHEEL
    else if(Byte0 == 0xE0)
    {
        unsigned int iCents = 0;
        CLine cLine(0, -200, 16383, 200);

        if(cLine.can_calculate_y())
            iCents = (unsigned int)cLine.y(Byte1 + (Byte2 << 7));

        for(unsigned int i = 0; i < mInstrument->getNumberOfChannels(); i++)
            mInstrument->bend(i, iCents);
    }
}

void MCMIDIReceiver::message(const std::vector<unsigned char>& Message)
{
    if(Message.size() >= 3)
        message(Message[0], Message[1], Message[2]);
    else if(Message.size() == 2)
        message(Message[0], Message[1], 0);
    else if(Message.size() == 1)
        message(Message[0], 0, 0);
}

void MCMIDIReceiver::message32(uint32_t Message)
{
    message((Message & 0xFF000000) >> 24,
            (Message & 0x00FF0000) >> 16,
            (Message & 0x0000FF00) >> 8);
}

void MCMIDIReceiver::message32inv(uint32_t Message)
{
    message((Message & 0x0000FF),
            (Message & 0x00FF00) >> 8,
            (Message & 0xFF0000) >> 16);
}

void MCMIDIReceiver::setMode(unsigned char Mode)
{
    iMode = Mode;
}
