
////////////////////////////////////////////////////////////////////////
//
//  Modus
//  C++ Music Library
//  [Tools]
//
//  Arturo Cepeda
//  July 2012 - December 2012
//
//  --------------------------------------------------------------------
//
//  msamplepack
//
//  Tool to generate sample-packs for Modus
//
////////////////////////////////////////////////////////////////////////

#include "mconstants.h"

#ifdef __linux__
#include <string.h>
#endif

#include <iostream>
#include <fstream>

#define AUDIO_FORMATS  4

using namespace std;

int main(int argc, char* argv[])
{
    const char* sAudioFormats[] = {"wav", "ogg", "aac", "ima4"};

    char sMSamplePackVersion[16];
    char sSamplePack[64];
    char sPath[256];
    char sFormat[8];
    char sExtension[8];

    unsigned int iTotalNumberOfSamples = 0;
    unsigned int iNumSampleSets;
    unsigned int iAux;
    unsigned int i;
    unsigned int j;

    unsigned int* iMode;
    unsigned int* iMinIntensity;
    unsigned int* iMaxIntensity;
    unsigned int* iRangeLow;
    unsigned int* iRangeHigh;
    unsigned int* iNumberOfSamples;

    // version
    sprintf(sMSamplePackVersion, "MSP%s", MODUS_VERSION);

    // header
    cout << "\n  Modus " << MODUS_VERSION;
    cout << "\n  C++ Music Library";
    cout << "\n  SamplePack tool\n";

    // general settings
    cout << "\n  Name for the sample-pack: ";
    cin >> sSamplePack;
    strcat(sSamplePack, ".msp");

    cout << "  Samples path (with '/' or '\\' at the end): ";
    cin >> sPath;

    memset(sExtension, 0, 8);
    cout << "  Extension (without '.' at the beginning): ";
    cin >> sExtension;

    memset(sFormat, 0, 8);
    cout << "  Audio format (";

    for(i = 0; i < AUDIO_FORMATS; i++)
    {
        cout << sAudioFormats[i];

        if(i < (AUDIO_FORMATS - 1))
            cout << ", ";
    }

    cout << "): ";
    cin >> sFormat;

    cout << "  Number of sample sets: ";
    cin >> iNumSampleSets;

    while(iNumSampleSets == 0)
    {
        cout << "  Number of sample sets (> 0): ";
        cin >> iNumSampleSets;
    }

    // memory allocation
    iMode = new unsigned int[iNumSampleSets];
    iMinIntensity = new unsigned int[iNumSampleSets];
    iMaxIntensity = new unsigned int[iNumSampleSets];
    iRangeLow = new unsigned int[iNumSampleSets];
    iRangeHigh = new unsigned int[iNumSampleSets];
    iNumberOfSamples = new unsigned int[iNumSampleSets];

    // settings for each sample set
    for(i = 0; i < iNumSampleSets; i++)
    {
        cout << "\n\n   SAMPLE SET #" << i;
        cout << "\n  ===============" << endl;

        cout << "\n  Mode: ";
        cin >> iMode[i];

        cout << "  Minimum intensity: ";
        cin >> iMinIntensity[i];

        cout << "  Maximum intensity: ";
        cin >> iMaxIntensity[i];

        if(iMinIntensity[i] > iMaxIntensity[i])
        {
            iAux = iMinIntensity[i];
            iMinIntensity[i] = iMaxIntensity[i];
            iMaxIntensity[i] = iAux;
        }

        cout << "  Range - Lowest pitch: ";
        cin >> iRangeLow[i];

        cout << "  Range - Highest pitch: ";
        cin >> iRangeHigh[i];

        if(iRangeLow[i] > iRangeHigh[i])
        {
            iAux = iRangeLow[i];
            iRangeLow[i] = iRangeHigh[i];
            iRangeHigh[i] = iAux;
        }

        iNumberOfSamples[i] = iRangeHigh[i] - iRangeLow[i] + 1;
    }

    // generate sample pack
    cout << "\n\n  Generating sample pack...";

    ofstream *fPackFile = new ofstream(sSamplePack, ios::out | ios::binary);

    if(!fPackFile->is_open())
    {
        // memory release
        delete fPackFile;
        delete[] iMode;
        delete[] iMinIntensity;
        delete[] iMaxIntensity;
        delete[] iRangeLow;
        delete[] iRangeHigh;
        delete[] iNumberOfSamples;

        // error message
        cerr << "\n  Error: could not create sample pack file\n\n";
        return -1;
    }

    // file header
    fPackFile->write(sMSamplePackVersion, 16);                          // Version
    fPackFile->write(sFormat, 8);                                       // Format
    fPackFile->write((char *)&iNumSampleSets, sizeof(unsigned int));    // Number of sample sets

    char sReserved[4];
    memset(sReserved, 0, 4);
    fPackFile->write(sReserved, 4);                                     // Reserved (4 bytes)

    // samples
    char sSampleFilename[512];
    ifstream* fSampleFile;
    unsigned int iSampleFileSize;
    char* sSampleData;

    for(i = 0; i < iNumSampleSets; i++)
    {
        fPackFile->write((char*)&i, sizeof(unsigned int));                      // SampleSet: ID
        fPackFile->write((char*)&iMode[i], sizeof(unsigned int));               // SampleSet: Mode
        fPackFile->write((char*)&iMinIntensity[i], sizeof(unsigned int));       // SampleSet: Minimum intensity
        fPackFile->write((char*)&iMaxIntensity[i], sizeof(unsigned int));       // SampleSet: Maximum intensity
        fPackFile->write((char*)&iRangeLow[i], sizeof(unsigned int));           // SampleSet: Lowest pitch
        fPackFile->write((char*)&iRangeHigh[i], sizeof(unsigned int));          // SampleSet: Highest pitch
        fPackFile->write((char*)&iNumberOfSamples[i], sizeof(unsigned int));    // SampleSet: Number of samples
        fPackFile->write(sReserved, 4);                                         // SampleSet: Reserved (4 bytes)

        iTotalNumberOfSamples += iNumberOfSamples[i];

        for(j = 0; j < iNumberOfSamples[i]; j++)
        {
            sprintf(sSampleFilename, "%s%02d_%03d.%s", sPath, i, j + iRangeLow[i], sExtension);
            fSampleFile = new ifstream(sSampleFilename, ios::in | ios::binary | ios::ate);

            if(fSampleFile->is_open())
            {
                iSampleFileSize = (unsigned int)fSampleFile->tellg();
                sSampleData = new char[iSampleFileSize];

                fSampleFile->seekg(0, ios::beg);
                fSampleFile->read(sSampleData, iSampleFileSize);
                fSampleFile->close();
            }
            else
            {
                iSampleFileSize = 0;                
            }

            fPackFile->write((char *)&iSampleFileSize, sizeof(unsigned int));   // SampleSet - Sample: size

            if(iSampleFileSize > 0)
            {
                fPackFile->write(sSampleData, iSampleFileSize);                 // SampleSet - Sample: data
                delete[] sSampleData;
            }

            delete fSampleFile;
        }
    }

    fPackFile->write((char*)&iTotalNumberOfSamples, sizeof(unsigned int));
    fPackFile->close();

    cout << "\n  Sample pack created successfully!\n\n";

    // memory release
    delete fPackFile;
    delete[] iMode;
    delete[] iMinIntensity;
    delete[] iMaxIntensity;
    delete[] iRangeLow;
    delete[] iRangeHigh;
    delete[] iNumberOfSamples;

    return 0;
}
