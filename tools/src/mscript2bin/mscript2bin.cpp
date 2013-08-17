
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
//  mscript2bin
//
//  Tool to generate binary files from scripts
//
////////////////////////////////////////////////////////////////////////

#include "mscript2bin.h"
#include <iostream>

#ifdef _MSC_VER
#ifdef _DEBUG
#pragma comment(lib, ".\\..\\..\\..\\..\\lib\\Win32\\modusd.lib")
#else
#pragma comment(lib, ".\\..\\..\\..\\..\\lib\\Win32\\modus.lib")
#endif
#endif

using namespace std;

int main(int argc, char* argv[])
{
    // header
    cout << "\n  Modus " << MODUS_VERSION;
    cout << "\n  C++ Music Library";
    cout << "\n  MScript-to-Bin tool\n";

    if(argc < 4)
    {
        cout << "\n  Usage: mscript2bin <Script_type> <Script_filename> <Binary_filename> [-def:<Definition_filename>]\n";
        cout << "\n  Script type: -score, -harmony, -scales, -song\n";
        return -1;
    }

    char* sScriptType = argv[1];
    ScriptType iScriptType;

    if(strcmp(sScriptType, "-score") == 0)
        iScriptType = ScriptType::Score;
    else if(strcmp(sScriptType, "-harmony") == 0)
        iScriptType = ScriptType::Harmony;
    else if(strcmp(sScriptType, "-scales") == 0)
        iScriptType = ScriptType::Scales;
    else if(strcmp(sScriptType, "-song") == 0)
        iScriptType = ScriptType::Song;
    else
    {
        cout << "\n  Invalid parameter: " << sScriptType << "\n";
        cout << "\n  Usage: mscript2bin <Script_type> <Script_filename> <Binary_filename> [-def:<Definition_filename>]\n";
        cout << "\n  Script type: -score, -harmony, -scales, -song\n";
        return -1;
    }

    char* sScriptFile = argv[2];
    char* sBinaryFile = argv[3];
    char* sDefinitionFile = argc >= 5 ? argv[4] : NULL;

    if(sDefinitionFile)
        sDefinitionFile += 5;

    bool generated = false;

    switch(iScriptType)
    {
    case ScriptType::Score:
        generated = generateScore(sScriptFile, sBinaryFile);
        break;
    case ScriptType::Harmony:
        generated = generateHarmonyPattern(sScriptFile, sBinaryFile, sDefinitionFile);
        break;
    case ScriptType::Scales:
        generated = generateScalePattern(sScriptFile, sBinaryFile, sDefinitionFile);
        break;
    case ScriptType::Song:
        generated = generateSongStructure(sScriptFile, sBinaryFile);
        break;
    }

    if(generated)
        cout << "\n\n  Binary script file \"" << sBinaryFile << "\" created successfully!\n\n";

    return 0;
}

bool generateScore(char* sScriptFile, char* sBinaryFile)
{
    // open script
    MCScore mScore;

    if(!mScore.loadScriptFromFile(sScriptFile))
    {
        cerr << "\n  The script file \"" << sScriptFile << "\" could not be readed\n";
        return false;
    }

    // check if the metadata fields are used in the score
    MSScoreEntry* mEntry;
    bool bMetadata = false;

    for(unsigned int i = 0; i < mScore.getNumberOfEntries(); i++)
    {
        mEntry = mScore[i];

        if(mEntry->Note.MDA != 0 || mEntry->Note.MDB != 0 || mEntry->Note.MDC != 0)
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
        return false;
    }

    cout << "\n  Writing binary score file \"" << sBinaryFile << "\"...";

    // write header
    writeHeader(fBinaryFile, ScriptType::Score);

    fBinaryFile.write((char*)&bMetadata, 1);                            // whether the score is using metadata fields

    unsigned int iNumberOfEntries = mScore.getNumberOfEntries();
    fBinaryFile.write((char*)&iNumberOfEntries, sizeof(unsigned int));  // number of entries

    // write all the entries
    for(unsigned int i = 0; i < iNumberOfEntries; i++)
    {
        mEntry = mScore[i];

        writeTimePosition(fBinaryFile, mEntry->TimePosition);

        fBinaryFile.write((char*)&mEntry->Note.Mode, 1);
        fBinaryFile.write((char*)&mEntry->Note.Channel, 1);
        fBinaryFile.write((char*)&mEntry->Note.Pitch, 1);
        fBinaryFile.write((char*)&mEntry->Note.Intensity, 1);
        fBinaryFile.write((char*)&mEntry->Note.Duration, 1);

        if(bMetadata)
        {
            fBinaryFile.write((char*)&mEntry->Note.MDA, 1);
            fBinaryFile.write((char*)&mEntry->Note.MDB, 1);
            fBinaryFile.write((char*)&mEntry->Note.MDC, 1);
        }
    }

    fBinaryFile.close();

    return true;
}

bool generateHarmonyPattern(char* sScriptFile, char* sBinaryFile, char* sDefinitionFile)
{
    // open script
    MCHarmonyPattern mHarmonyPattern;

    bool bScriptLoad = sDefinitionFile ? mHarmonyPattern.loadScriptFromFile(sDefinitionFile, sScriptFile)
                                       : mHarmonyPattern.loadScriptFromFile(sScriptFile);
    if(!bScriptLoad)
    {
        cerr << "\n  The script file \"" << sScriptFile << "\" could not be readed\n";
        return false;
    }

    // new binary file
    std::ofstream fBinaryFile(sBinaryFile, std::ios::binary | std::ios::out);

    if(!fBinaryFile.is_open())
    {
        cerr << "\n  Cannot create binary file \"" << sBinaryFile << "\"" << endl;
        return false;
    }

    cout << "\n  Writing binary score file \"" << sBinaryFile << "\"...";

    // write header
    MSHarmonyPatternEntry* mEntry;    
    writeHeader(fBinaryFile, ScriptType::Harmony);

    unsigned int iNumberOfEntries = mHarmonyPattern.getNumberOfEntries();
    fBinaryFile.write((char*)&iNumberOfEntries, sizeof(unsigned int));  // number of entries

    // write all the entries
    for(unsigned int i = 0; i < iNumberOfEntries; i++)
    {
        mEntry = mHarmonyPattern[i];

        writeTimePosition(fBinaryFile, mEntry->TimePosition);

        fBinaryFile.write((char*)&mEntry->RootNote, 1);
        writeNoteList(fBinaryFile, mEntry->Chord);
        fBinaryFile.write((char*)&mEntry->BassNote, 1);
        writeNoteList(fBinaryFile, mEntry->Tensions);
    }

    fBinaryFile.close();

    return true;
}

bool generateScalePattern(char* sScriptFile, char* sBinaryFile, char* sDefinitionFile)
{
    // open script
    MCScalePattern mScalePattern;

    bool bScriptLoad = sDefinitionFile ? mScalePattern.loadScriptFromFile(sDefinitionFile, sScriptFile)
                                       : mScalePattern.loadScriptFromFile(sScriptFile);
    if(!bScriptLoad)
    {
        cerr << "\n  The script file \"" << sScriptFile << "\" could not be readed\n";
        return false;
    }

    // new binary file
    std::ofstream fBinaryFile(sBinaryFile, std::ios::binary | std::ios::out);

    if(!fBinaryFile.is_open())
    {
        cerr << "\n  Cannot create binary file \"" << sBinaryFile << "\"" << endl;
        return false;
    }

    cout << "\n  Writing binary score file \"" << sBinaryFile << "\"...";

    // write header
    MSScalePatternEntry* mEntry;
    writeHeader(fBinaryFile, ScriptType::Scales);

    unsigned int iNumberOfEntries = mScalePattern.getNumberOfEntries();
    fBinaryFile.write((char*)&iNumberOfEntries, sizeof(unsigned int));

    // write all the entries
    for(unsigned int i = 0; i < iNumberOfEntries; i++)
    {
        mEntry = mScalePattern[i];

        writeTimePosition(fBinaryFile, mEntry->TimePosition);

        fBinaryFile.write((char*)&mEntry->RootNote, 1);
        writeNoteList(fBinaryFile, mEntry->Scale);
    }

    fBinaryFile.close();

    return true;
}

bool generateSongStructure(char* sScriptFile, char* sBinaryFile)
{
    // open script
    MCSongStructure mSongStructure;

    if(!mSongStructure.loadScriptFromFile(sScriptFile))
    {
        cerr << "\n  The script file \"" << sScriptFile << "\" could not be readed\n";
        return false;
    }

    // new binary file
    std::ofstream fBinaryFile(sBinaryFile, std::ios::binary | std::ios::out);

    if(!fBinaryFile.is_open())
    {
        cerr << "\n  Cannot create binary file \"" << sBinaryFile << "\"" << endl;
        return false;
    }

    cout << "\n  Writing binary score file \"" << sBinaryFile << "\"...";

    // write header
    MSSongSection* mEntry;
    writeHeader(fBinaryFile, ScriptType::Song);

    unsigned int iNumberOfEntries = mSongStructure.getNumberOfEntries();
    fBinaryFile.write((char*)&iNumberOfEntries, sizeof(unsigned int));

    // write all the entries
    for(unsigned int i = 0; i < iNumberOfEntries; i++)
    {
        mEntry = mSongStructure[i];

        fBinaryFile.write((char*)&mEntry->BeatsPerMeasure, sizeof(unsigned int));
        fBinaryFile.write((char*)&mEntry->FirstMeasure, sizeof(int));
        fBinaryFile.write((char*)&mEntry->LastMeasure, sizeof(int));
        fBinaryFile.write((char*)&mEntry->Tempo, sizeof(float));
        fBinaryFile.write((char*)&mEntry->TargetTempo, sizeof(float));
    }

    fBinaryFile.close();

    return true;
}

void writeHeader(std::ofstream& fStream, ScriptType iScriptType)
{
    char cScriptType = (char)iScriptType;

    fStream.write("MBS0", 4);           // Modus Binary Script, version
    fStream.write(&cScriptType, 1);     // Script type
    fStream.write("", 11);              // 11 empty bytes (reserved)
}

void writeTimePosition(std::ofstream& fStream, const MSTimePosition& mTimePosition)
{
    short iMeasure = (short)mTimePosition.Measure;
    unsigned char iBeat = (unsigned char)mTimePosition.Beat;
    unsigned char iTick = (unsigned char)mTimePosition.Tick;

    fStream.write((char*)&iMeasure, sizeof(short));
    fStream.write((char*)&iBeat, 1);
    fStream.write((char*)&iTick, 1);
}

void writeNoteList(std::ofstream& fStream, const std::vector<MTNote>& mNoteList)
{
    char iNumberOfNotes = (char)mNoteList.size();
    fStream.write(&iNumberOfNotes, 1);

    for(unsigned int i = 0; i < mNoteList.size(); i++)
        fStream.write((char*)&mNoteList[i], 1);
}
