
////////////////////////////////////////////////////////////////////////
//
//  Modus
//  C++ Music Library
//  [Tools]
//
//  Arturo Cepeda
//  July 2012
//
//  --------------------------------------------------------------------
//
//  CMIDIFile
//
//  Class to read MIDI files
//
////////////////////////////////////////////////////////////////////////

#include "midifile.h"
#include <fstream>

CMIDIFile::CMIDIFile()
{
    memset(sBuffer, 0, BUFFER_SIZE);
    iNumberOfTracks = 0;
}

CMIDIFile::~CMIDIFile()
{
}

bool CMIDIFile::openFile(const char* Filename)
{
    std::ifstream fMIDIFile(Filename, std::ios::in | std::ios::binary);

    // can't open the file
    if(!fMIDIFile.is_open())
        return false;

    // read MIDI file:
    SMIDIEvent sEvent;

    unsigned char* cData;
    unsigned char cEventType;

    int iFileSize;
    int iChunkSize;
    int iBytesReaded;
    int iBytesVariableLength;
    int iDeltaTime;
    int iMetaLength;

    // obtain file size
    fMIDIFile.seekg(0, std::ios::end);
    iFileSize = (int)fMIDIFile.tellg();
    fMIDIFile.seekg(0, std::ios::beg);

    // load data into memory
    cData = new unsigned char[iFileSize];
    fMIDIFile.read((char*)cData, iFileSize);
    fMIDIFile.close();

    // initialize data pointer
    cPointer = cData;

    //
    //  Header Chunk
    //
    readString(sBuffer, 4);         // "MThd"
    sBuffer[4] = '\0';

    if(strcmp(sBuffer, "MThd") != 0)
    {
        delete[] cData;
        return false;
    }

    read4Bytes(&iChunkSize);        // Chunk size
    read2Bytes(&iFormatType);       // Format type
    read2Bytes(&iNumberOfTracks);   // Number of tracks
    read2Bytes(&iTimeDivision);     // Time division

    //
    //  Track Chunk
    //
    for(int i = 0; i < iNumberOfTracks; i++)
    {
        readString(sBuffer, 4);                                     // "MTrk"
        sBuffer[4] = '\0';

        if(strcmp(sBuffer, "MTrk") != 0)
        {
            delete[] cData;
            return false;
        }

        read4Bytes(&iChunkSize);                                    // Chunk size

        iBytesReaded = 0;
        sEvent.Track = i;
        sEvent.Time = 0;

        while(iBytesReaded < iChunkSize)                            // MIDI events
        {
            // read delta time and calculate current time position
            iBytesVariableLength = readVariableLength(&iDeltaTime);
            iBytesReaded += iBytesVariableLength;
            sEvent.Time += iDeltaTime;

            // check event type
            cEventType = (*cPointer & 0xF0) >> 4;

            // defined event type: read type and MIDI channel 
            // (if undefined the previous type stays)
            if(cEventType >= 0x8)
            {
                readByte(&sEvent.Type, &sEvent.MIDIChannel);
                iBytesReaded++;
            }

            switch(sEvent.Type)
            {
            //
            //  Channel Events
            //
            case 0x8:   // note off
            case 0x9:   // note on
            case 0xA:   // note aftertouch
            case 0xB:   // controller
            case 0xE:   // pitch bend
                readByte(&sEvent.Parameter1);
                readByte(&sEvent.Parameter2);
                iBytesReaded += 2;
                break;

            case 0xC:   // program change
            case 0xD:   // channel aftertouch
                readByte(&sEvent.Parameter1);
                sEvent.Parameter2 = 0;
                iBytesReaded++;
                break;

            //
            //  Meta-Events / System Exclusive Events
            //
            case 0xF:

                sEvent.Parameter1 = 0;
                sEvent.Parameter2 = 0;

                // Meta-Events
                if(sEvent.MIDIChannel == 0xF)
                {
                    // type
                    readByte(&sEvent.Parameter1);
                    iBytesReaded++;
                }

                // read data length
                iBytesVariableLength = readVariableLength(&iMetaLength);
                iBytesReaded += iBytesVariableLength;

                // read data (unused)
                readString(sBuffer, iMetaLength);
                iBytesReaded += iMetaLength;

                // time signature meta-event: store time signature numerator as 2nd parameter
                if(sEvent.Parameter1 == 0x58)
                    sEvent.Parameter2 = sBuffer[0];

                break;
            }

            // add event to the list            
            addEvent(sEvent);
        }        
    }

    delete[] cData;
    return true;
}

int CMIDIFile::getTimeDivision()
{
    return iTimeDivision;
}

unsigned int CMIDIFile::getNumberOfEvents()
{
    return sEvents.size();
}

SMIDIEvent* CMIDIFile::getEvent(unsigned int Index)
{
    if(Index < sEvents.size())
        return &sEvents[Index];
    else
        return NULL;
}


//
//  Private methods
//
void CMIDIFile::addEvent(SMIDIEvent& sEvent)
{
    // if the list is empty, just add it
    if(sEvents.size() == 0)
    {
        sEvents.push_back(sEvent);
        return;
    }

    // 1) at the beginning
    if(sEvent.Time < sEvents[0].Time)
    {
        sEvents.insert(sEvents.begin(), sEvent);
    }

    // 2) at the end
    else if(sEvent.Time >= sEvents[sEvents.size() - 1].Time)
    {
        sEvents.push_back(sEvent);
    }

    // 3) in the middle
    else
    {
        for(unsigned int i = 1; i < sEvents.size() - 1; i++)
        {
            if(sEvent.Time < sEvents[i].Time)
            {
                sEvents.insert(sEvents.begin() + i, sEvent);
                return;
            }
        }
    }
}

void CMIDIFile::readString(char* sString, int iSize)
{
    memcpy(sString, cPointer, iSize);
    cPointer += iSize;
}

void CMIDIFile::readByte(unsigned char* iVar)
{
    *iVar = *cPointer;
    cPointer++;
}

void CMIDIFile::readByte(unsigned char* iVar4bitsLeft, unsigned char* iVar4bitsRight)
{
    *iVar4bitsLeft = (*cPointer & 0xF0) >> 4;
    *iVar4bitsRight = *cPointer & 0x0F;
    cPointer++;
}

void CMIDIFile::read2Bytes(int* iVar)
{
    *iVar = *cPointer << 8;
    cPointer++;

    *iVar += *cPointer;
    cPointer++;
}

void CMIDIFile::read4Bytes(int* iVar)
{
    *iVar = *cPointer << 24;
    cPointer++;

    *iVar += *cPointer << 16;
    cPointer++;

    *iVar += *cPointer << 8;
    cPointer++;

    *iVar += *cPointer;
    cPointer++;
}

int CMIDIFile::readVariableLength(int* iVar)
{   
    int iBytes = 0;
    unsigned char cByte;
    
    *iVar = 0;

    do
    {
        // read the next byte
        cByte = *cPointer;
        cPointer++;
        iBytes++;

        // displace current result 7 bits to the left and add the new 7-bit value to it
        *iVar <<= 7;
        *iVar += cByte & 0x7F;

    } while((cByte & 0x80) != 0);   // the first bit is 0

    // return number of bytes
    return iBytes;
}
