
////////////////////////////////////////////////////////////////////////
//
//  Modus
//  C++ Music Library
//  [Tools]
//
//  Arturo Cepeda Pérez
//  May 2013
//
//  --------------------------------------------------------------------
//
//  mscore2bin
//
//  Tool to generate binary files from score scripts
//
////////////////////////////////////////////////////////////////////////

#include "mconstants.h"
#include "mscore.h"

#ifdef _MSC_VER
#ifdef _DEBUG
#pragma comment(lib, ".\\..\\..\\..\\..\\lib\\Win32\\modusd.lib")
#else
#pragma comment(lib, ".\\..\\..\\..\\..\\lib\\Win32\\modus.lib")
#endif
#endif

#include <iostream>
#include <fstream>

using namespace std;

int main(int argc, char* argv[])
{
    // header
    cout << "\n  Modus " << MODUS_VERSION;
    cout << "\n  C++ Music Library";
    cout << "\n  MScore-to-Bin tool\n";

    if(argc < 3)
    {
        cout << "\n  Usage: mscore2bin <Script_filename> <Binary_filename>\n";
        return -1;
    }

    char* sScriptFile = argv[1];
    char* sBinaryFile = argv[2];

    // open script
    MCScore mScore;
    if(!mScore.loadScriptFromFile(sScriptFile))
    {
        cerr << "\n  The script file \"" << sScriptFile << "\" could not be readed\n";
        return -1;
    }

    // check if the metadata fields are used in the score
    MSScoreEntry* mScoreEntry;
    bool bMetadata = false;

    for(unsigned int i = 0; i < mScore.getNumberOfEntries(); i++)
    {
        mScoreEntry = mScore.getEntry(i);

        if(mScoreEntry->Note.MDA != 0 || mScoreEntry->Note.MDB != 0 || mScoreEntry->Note.MDC != 0)
        {
            bMetadata = true;
            break;
        }
    }

    // new binary file
    std::ofstream fBinaryFile(sBinaryFile, std::ios::binary | std::ios::out);

    if(!fBinaryFile.is_open())
    {
        cerr << "\n  Cannot create binary file \"" << sBinaryFile << "\"" << endl;
        return -1;
    }

    cout << "\n  Writing binary score file \"" << sBinaryFile << "\"...";

    // write header
    fBinaryFile.write("MBS", 3);                                        // Modus Binary Script
    fBinaryFile.write("", 12);                                          // 12 empty bytes (reserved)
    fBinaryFile.write(reinterpret_cast<char*>(&bMetadata), 1);          // whether the score is using metadata fields

    unsigned int iNumberOfEntries = mScore.getNumberOfEntries();
    fBinaryFile.write((char*)&iNumberOfEntries, sizeof(unsigned int));  // number of entries

    // write all the entries
    short iMeasure;
    unsigned char iBeat;
    unsigned char iTick;

    for(unsigned int i = 0; i < iNumberOfEntries; i++)
    {
        mScoreEntry = mScore.getEntry(i);

        iMeasure = (short)mScoreEntry->TimePosition.Measure;
        iBeat = (unsigned char)mScoreEntry->TimePosition.Beat;
        iTick = (unsigned char)mScoreEntry->TimePosition.Tick;

        fBinaryFile.write(reinterpret_cast<char*>(&iMeasure), sizeof(short));
        fBinaryFile.write(reinterpret_cast<char*>(&iBeat), 1);
        fBinaryFile.write(reinterpret_cast<char*>(&iTick), 1);
        fBinaryFile.write(reinterpret_cast<char*>(&mScoreEntry->Note.Mode), 1);
        fBinaryFile.write(reinterpret_cast<char*>(&mScoreEntry->Note.Channel), 1);
        fBinaryFile.write(reinterpret_cast<char*>(&mScoreEntry->Note.Pitch), 1);
        fBinaryFile.write(reinterpret_cast<char*>(&mScoreEntry->Note.Intensity), 1);
        fBinaryFile.write(reinterpret_cast<char*>(&mScoreEntry->Note.Duration), 1);

        if(bMetadata)
        {
            fBinaryFile.write(reinterpret_cast<char*>(&mScoreEntry->Note.MDA), 1);
            fBinaryFile.write(reinterpret_cast<char*>(&mScoreEntry->Note.MDB), 1);
            fBinaryFile.write(reinterpret_cast<char*>(&mScoreEntry->Note.MDC), 1);
        }
    }

    fBinaryFile.close();    

    cout << "\n\n  Binary score file \"" << sBinaryFile << "\" created successfully!\n\n";

    return 0;
}
