
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

#ifdef __linux__
#include <string.h>
#endif

#include <vector>

#define BUFFER_SIZE 256

struct SMIDIEvent
{
    int Track;
    int Time;

    unsigned char Type;
    unsigned char MIDIChannel;
    unsigned char Parameter1;
    unsigned char Parameter2;
};

class CMIDIFile
{
private:
    unsigned char* cPointer;

    char sBuffer[BUFFER_SIZE];
    int iFormatType;
    int iNumberOfTracks;
    int iTimeDivision;

    std::vector<SMIDIEvent> sEvents;

    void addEvent(SMIDIEvent& sEvent);
    void readString(char* sString, int iSize);
    void readByte(unsigned char* iVar);
    void readByte(unsigned char* iVar4bitsLeft, unsigned char* iVar4bitsRight);
    void read2Bytes(int* iVar);
    void read4Bytes(int* iVar);
    int readVariableLength(int* iVar);

public:
    CMIDIFile();
    ~CMIDIFile();

    bool openFile(const char* Filename);

    int getTimeDivision();

    unsigned int getNumberOfEvents();
    SMIDIEvent* getEvent(unsigned int Index);
};
