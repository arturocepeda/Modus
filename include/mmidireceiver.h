
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

#ifndef _MMIDIRECEIVER_H_
#define _MMIDIRECEIVER_H_

#include "minstrument.h"
#include "stdint.h"

/**
 *  @brief Receives MIDI messages, interpret them and tells the attached instrument what to do
 */
class MCMIDIReceiver
{
private:
    MCInstrument* mInstrument;
    bool bMIDIChannel[16];
    unsigned char* iAttachedChannel;
    int iRangeSize;

    MSNote mNote;
    unsigned char iMinPitch;
    unsigned char iMaxPitch;
    unsigned char iMode;

public:
    /**
     *  @brief Constructor
     *  @param Instrument Instrument which is going to be attached to the MIDI receiver
     */
    MCMIDIReceiver(MCInstrument* Instrument);
    /**
     *  @brief Constructor
     *  @param Instrument Instrument which is going to be attached to the MIDI receiver
     *  @param LowestPitch Lowest pitch to define the range in which the instrument will receive messages
     *  @param HighestPitch Highest pitch to define the range in which the instrument will receive messages
     */
    MCMIDIReceiver(MCInstrument* Instrument, unsigned char LowestPitch, unsigned char HighestPitch);
    ~MCMIDIReceiver();

    /**
     *  @brief Makes the MIDI receiver accept messages from the specified MIDI channel (1-16)
     *  @param MIDIChannel MIDI channel through which all the messages will be accepted
     */
    void listenToMIDIChannel(unsigned char MIDIChannel);
    /**
     *  @brief Makes the MIDI receiver accept messages from all MIDI channels
     */
    void listenToAllMIDIChannels();
    /**
     *  @brief Makes the MIDI receiver ignore messages from the specified MIDI channel (1-16)
     *  @param MIDIChannel MIDI channel through which all the messages will be ignored
     */
    void ignoreMIDIChannel(unsigned char MIDIChannel);
    /**
     *  @brief Makes the MIDI receiver ignore messages from all MIDI channels
     */
    void ignoreAllMIDIChannels();

    /**
     *  @brief Attaches a pitch to a channel, so the specified note will always be played through that channel
     *  @param Pitch Pitch to attach
     *  @param Channel Channel to which the specified pitch will be attached
     */
    void attachPitchToChannel(unsigned char Pitch, unsigned char Channel);
    /**
     *  @brief Attaches an interval of pitches to a channel, so the specified notes will always be played through 
     *         that channel
     *  @param FirstPitch First pitch of the interval (included)
     *  @param LastPitch Last pitch of the interval (included)
     *  @param Channel Channel to which the selected pitches will be attached
     */
    void attachPitchesToChannel(unsigned char FirstPitch, unsigned char LastPitch, unsigned char Channel);
    /**
     *  @brief Attaches all the pitches to a unique channel
     *  @param Channel Channel through which all the notes will be played
     */
    void attachAllPitchesToChannel(unsigned char Channel);
    /**
     *  @brief Attaches each pitch to a different channel (for example, for piano and harp). The instrument
     *         must have as many channels as the size of its range
     */
    void attachAllPitchesToDifferentChannels();

    /**
     *  @brief Receives a MIDI message (channel event)
     *  @param Byte0 Message's first byte (type and MIDI channel)
     *  @param Byte1 Message's second byte (parameter 1)
     *  @param Byte2 Message's third byte (parameter 2)
     */
    void message(unsigned char Byte0, unsigned char Byte1, unsigned char Byte2);
    /**
     *  @brief Receives a MIDI message (channel event) through a byte vector
     *  @param Message MIDI message
     */
    void message(const std::vector<unsigned char>& Message);
    /**
     *  @brief Receives a MIDI message (channel event) through a 32-bit value like this: Byte0-Byte1-Byte2-Byte3
     *  @param Message MIDI message
     */
    void message32(uint32_t Message);
    /**
     *  @brief Receives a MIDI message (channel event) through a 32-bit value like this: Byte3-Byte2-Byte1-Byte0
     *  @param Message MIDI message
     */
    void message32inv(uint32_t Message);

    /**
     *  @brief Defines the mode in which the notes are going to be played (default: 0)
     *  @param Mode New mode
     */
    void setMode(unsigned char Mode);
};

#endif
