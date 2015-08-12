
////////////////////////////////////////////////////////////////////////
//
//  Modus
//  C++ Music Library
//  [Sound Generator]
//
//  Copyright (c) 2012-2015 Arturo Cepeda
//
//  --------------------------------------------------------------------
//
//  Audio engine: 
//
//  irrKlang
//  Copyright (c) Nikolaus Gebhardt / Ambiera 2001-2009  
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

#include "mxsoundgenirrklang.h"
#include <fstream>
#include <cmath>
#include "mutils.h"

using namespace Modus;

//
//  Audio source manager
//
MCirrKlangSourceManager::MCirrKlangSourceManager(irrklang::ISoundEngine* AudioSystem, unsigned int NumSources)
   : MCAudioSourceManager(NumSources)
   , ikEngine(AudioSystem)
{
}

void MCirrKlangSourceManager::allocateSources()
{
}

void MCirrKlangSourceManager::releaseSources()
{
   for(unsigned int i = 0; i < iNumSources; i++)
   {
      irrklang::ISound* ikSound = reinterpret_cast<irrklang::ISound*>(sSources[i].Source);

      if(ikSound)
         ikSound->drop();
   }
}

void MCirrKlangSourceManager::playSource(unsigned int SourceIndex, void* Sound, bool Sound3D)
{
   irrklang::ISoundSource* ikBuffer = reinterpret_cast<irrklang::ISoundSource*>(Sound);
   irrklang::ISound* ikSound = 0;

   if(Sound3D)
   {
      irrklang::vec3df ikPosition3D(sSources[SourceIndex].Position[0],
         sSources[SourceIndex].Position[1], sSources[SourceIndex].Position[2]);
      sSources[SourceIndex].Source = ikEngine->play3D(ikBuffer, ikPosition3D, false, true);
      ikSound = reinterpret_cast<irrklang::ISound*>(sSources[SourceIndex].Source);
      ikSound->setPosition(ikPosition3D);
   }
   else
   {
      sSources[SourceIndex].Source = ikEngine->play2D(ikBuffer, false, true);
      ikSound = reinterpret_cast<irrklang::ISound*>(sSources[SourceIndex].Source);
      ikSound->setPan(sSources[SourceIndex].Position[0]);
   }

   ikSound->setVolume(sSources[SourceIndex].Volume);
   ikSound->setIsPaused(false);
}

void MCirrKlangSourceManager::stopSource(unsigned int SourceIndex)
{
   irrklang::ISound* ikSound = reinterpret_cast<irrklang::ISound*>(sSources[SourceIndex].Source);
   ikSound->stop();
}

bool MCirrKlangSourceManager::isSourcePlaying(unsigned int SourceIndex)
{
   irrklang::ISound* ikSound = reinterpret_cast<irrklang::ISound*>(sSources[SourceIndex].Source);

   return ikSound && !ikSound->isFinished();
}

void MCirrKlangSourceManager::setSourceVolume(unsigned int SourceIndex, float Volume)
{
   sSources[SourceIndex].Volume = Volume;

   irrklang::ISound* ikSound = reinterpret_cast<irrklang::ISound*>(sSources[SourceIndex].Source);

   if(ikSound && !ikSound->isFinished())
      ikSound->setVolume(Volume);
}

void MCirrKlangSourceManager::setSourcePitch(unsigned int SourceIndex, int Cents)
{
   irrklang::ISound* ikSound = reinterpret_cast<irrklang::ISound*>(sSources[SourceIndex].Source);

   if(ikSound && !ikSound->isFinished())
      ikSound->setPlaybackSpeed((float)pow(2, Cents / 1200.0f));
}

void MCirrKlangSourceManager::setSourcePan(unsigned int SourceIndex, float Pan)
{
   sSources[SourceIndex].Position[0] = Pan;

   irrklang::ISound* ikSound = reinterpret_cast<irrklang::ISound*>(sSources[SourceIndex].Source);

   if(ikSound && !ikSound->isFinished())
      ikSound->setPan(sSources[SourceIndex].Position[0]);
}

void MCirrKlangSourceManager::setSourcePosition(unsigned int SourceIndex, float X, float Y, float Z)
{
   sSources[SourceIndex].Position[0] = X;
   sSources[SourceIndex].Position[1] = Y;
   sSources[SourceIndex].Position[2] = Z;

   irrklang::ISound* ikSound = reinterpret_cast<irrklang::ISound*>(sSources[SourceIndex].Source);

   if(ikSound && !ikSound->isFinished())
   {
      irrklang::vec3df ikPosition3D(sSources[SourceIndex].Position[0],
         sSources[SourceIndex].Position[1], sSources[SourceIndex].Position[2]);
      ikSound->setPosition(ikPosition3D);
   }
}

void MCirrKlangSourceManager::setSourceDirection(unsigned int SourceIndex, float X, float Y, float Z)
{
}


//
//  Sound generator
//
unsigned int MCSoundGenirrKlang::iNumberOfInstances = 0;

MCSoundGenirrKlang::MCSoundGenirrKlang(unsigned int ID, unsigned int NumberOfChannels, bool Sound3D, irrklang::ISoundEngine* Engine)
   : MCSoundGenAudioMultipleChannel(ID, NumberOfChannels, Sound3D)
   , iEngine(Engine)
{
   if(!cManager)
   {
      cManager = new MCirrKlangSourceManager(Engine, IRRKLANG_SOURCES);
      cManager->allocateSources();
   }

   iNumberOfInstances++;

   iSound = NULL;
   iSampleSet = new int[iNumberOfChannels];

   for(unsigned int i = 0; i < iNumberOfChannels; i++)
      iSampleSet[i] = -1;
}

MCSoundGenirrKlang::~MCSoundGenirrKlang()
{
   unloadSamples();

   iNumberOfInstances--;

   if(iNumberOfInstances == 0)
   {
      cManager->releaseSources();
      delete cManager;
      cManager = NULL;
   }
}

void MCSoundGenirrKlang::loadSamples()
{
    if(sSampleSet.size() == 0)
        return;

    char sFilename[512];
    unsigned int i, j;

    iSound = new irrklang::ISoundSource**[sSampleSet.size()];

    for(i = 0; i < sSampleSet.size(); i++)
    {
        iSound[i] = new irrklang::ISoundSource*[iNumberOfSamples[i]];

        for(j = 0; j < iNumberOfSamples[i]; j++)
        {
            sprintf(sFilename, "%s%02d_%03d.%s", sSampleSet[i].Path, 
                                                 sSampleSet[i].ID, 
                                                 j + sSampleSet[i].Range.LowestNote,
                                                 sSampleSet[i].Format);

            iSound[i][j] = iEngine->addSoundSourceFromFile(sFilename);
        }
    }
}

void MCSoundGenirrKlang::loadSamplePack(std::istream& Stream, 
                                        void (*callback)(unsigned int TotalSamples, unsigned int Loaded, void* Data),
                                        void* Data)
{
    unsigned int iTotalSamples = 0;
    unsigned int iLoaded = 0;

    // read total number of samples
    if(callback)
    {
        Stream.seekg(-(std::ios::off_type)sizeof(unsigned int), std::ios::end);
        Stream.read((char*)&iTotalSamples, sizeof(unsigned int));
        Stream.seekg(0, std::ios::beg);
    }

    char sVersion[16];
    char sFormat[8];
    char sReserved[4];

    unsigned int iInput;
    unsigned int iNumSampleSets;

    Stream.read(sVersion, 16);                                     // Version
    Stream.read(sFormat, 8);                                       // Format
    Stream.read((char*)&iNumSampleSets, sizeof(unsigned int));     // Number of sample sets
    Stream.read(sReserved, 4);                                     // (Reserved)

    unsigned int i;
    unsigned int j;

    // release previously loaded samples
    unloadSamples();

    // clear sampleset list
    if(sSampleSet.size() > 0)
    {
        sSampleSet.clear();
        iNumberOfSamples.clear();
    }

    // load samplesets and samples
    MSSampleSet mSampleSet;
    unsigned int iNumSamples;
    char sSoundName[64];

    iSound = new irrklang::ISoundSource**[iNumSampleSets];

    for(i = 0; i < iNumSampleSets; i++)
    {
        // sample set
        strcpy(mSampleSet.Path, "");
        strcpy(mSampleSet.Format, sFormat);

        Stream.read((char*)&iInput, sizeof(unsigned int));        // SampleSet: ID
        mSampleSet.ID = (unsigned char)iInput;

        Stream.read((char*)&iInput, sizeof(unsigned int));        // SampleSet: Mode
        mSampleSet.Mode = (unsigned char)iInput;

        Stream.read((char*)&iInput, sizeof(unsigned int));        // SampleSet: Minimum intensity
        mSampleSet.MinimumIntensity = (unsigned char)iInput;

        Stream.read((char*)&iInput, sizeof(unsigned int));        // SampleSet: Maximum intensity
        mSampleSet.MaximumIntensity = (unsigned char)iInput;

        Stream.read((char*)&iInput, sizeof(unsigned int));        // SampleSet: Lowest pitch
        mSampleSet.Range.LowestNote = (unsigned char)iInput;

        Stream.read((char*)&iInput, sizeof(unsigned int));        // SampleSet: Highest pitch
        mSampleSet.Range.HighestNote = (unsigned char)iInput;

        sSampleSet.push_back(mSampleSet);

        // samples inside the sample set
        Stream.read((char*)&iNumSamples, sizeof(unsigned int));   // SampleSet: Number of samples
        iNumberOfSamples.push_back((unsigned char)iNumSamples);

        Stream.read(sReserved, 4);                                // SampleSet: (Reserved)

        iSound[i] = new irrklang::ISoundSource*[iNumSamples];

        unsigned int iSampleSize;
        char* sSampleData;

        for(j = 0; j < iNumSamples; j++)
        {
            Stream.read((char*)&iSampleSize, sizeof(unsigned int));
            iSound[i][j] = NULL;

            if(iSampleSize == 0)
                continue;

            sSampleData = new char[iSampleSize];
            Stream.read(sSampleData, iSampleSize);

            sprintf(sSoundName, "%02d%02d", i, j);
            iSound[i][j] = iEngine->addSoundSourceFromMemory(sSampleData, iSampleSize, sSoundName);

            // callback
            if(callback)
                callback(iTotalSamples, ++iLoaded, Data);

            delete[] sSampleData;
        }
    }
}

void MCSoundGenirrKlang::unloadSamples()
{
    if(!iSound)
        return;

    unsigned int i;
    unsigned int j;

    for(i = 0; i < sSampleSet.size(); i++)
    {
        for(j = 0; j < iNumberOfSamples[i]; j++)
            iEngine->removeSoundSource(iSound[i][j]);

        delete[] iSound[i];
    }

    sSampleSet.clear();
    iNumberOfSamples.clear();

    delete[] iSound;
    iSound = NULL;
}

void MCSoundGenirrKlang::playAudioSample(unsigned int SourceIndex, int SampleSet, int SampleIndex)
{
   cManager->playSource(SourceIndex, iSound[SampleSet][SampleIndex], b3DSound);
}
