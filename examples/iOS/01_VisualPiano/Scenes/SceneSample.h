
////////////////////////////////////////////////////////////////////////
//
//  Modus
//  C++ Music Library
//  Sample Application
//
//  Arturo Cepeda
//
////////////////////////////////////////////////////////////////////////

#ifndef _SCENESAMPLE_H_
#define _SCENESAMPLE_H_

#include "GEScene.h"
#include "audio.h"

#include "modus.h"
#include "mxsoundgenopenal.h"

#define LOWEST_NOTE     21
#define SCORES          2

class GESceneSampleThreads
{
public:
   static void* LoadSamplesThread(void* lp);
   static void* MusicTimerThread(void* lp);
};

class GESceneSampleCallbacks
{
public:
   static void SampleLoaded(unsigned int TotalSamples, unsigned int Loaded, void* Data);
   static void TimerTick(const MSTimePosition& TimePosition, void* Data);
   static void PlayNote(unsigned int Instrument, const MSNote& Note, void* Data);
   static void ReleaseNote(unsigned int Instrument, const MSNote& Note, void* Data);
   static void Damper(unsigned int Instrument, bool DamperState, void* Data);
};

class GESceneSample : public GEScene
{
private:
   // sprites
   GESprite* cSpriteUnused;
   GESprite* cSpriteKeyWhite;
   GESprite* cSpriteKeyBlack;
   GESprite* cSpriteKeyPress;  
   GESprite* cSpriteKeyPressBack;
   GESprite* cSpritePedalOn;
   GESprite* cSpritePedalOff;
   GESprite* cSpritePedal;
   GESprite* cSpriteLoading;
   
   // labels
   GELabel* cTextModus;
   GELabel* cTextLoading;
   GELabel* cTextPlaying;
   
   // textures
   struct
   {
      enum {KeyWhite, KeyBlack, KeyPress, KeyPressBack, PedalOn, PedalOff, Loading};      
   } 
   Textures;
   
   // sound system
   CAudio* cAudio;
   
   // Modus objects
   MCInstrument* mPiano;
   MCScore mScore[SCORES];
   MCSoundGenOpenAL* mSoundGen;
   
   // sample-pack
   unsigned int iTotalSamples;
   unsigned int iSamplesLoaded;
   
   // piano status
   unsigned char iIntensity[88];
   bool bDamper;
   int iCurrentScore;
   
   // music threads
   pthread_t pMusicTimerThread;
   pthread_t pLoadSamples;
   
   void init_iPhone();
   void init_iPad();
   
public:
   GESceneSample(GERendering* Render, void* GlobalData);
   
   void init();
   void update();
   void render();
   void release();
   
   void updateSamplesLoaded(unsigned int TotalSamples, unsigned int Loaded);
   void updatePiano(const MSTimePosition& TimePosition);
   
   MCSoundGenOpenAL* getSoundGen();
   
   void setIntensity(unsigned char Pitch, unsigned char Intensity);
   void setDamper(bool On);
   
   void inputTouchBegin(int ID, CGPoint* Point);
   void inputTouchMove(int ID, CGPoint* PreviousPoint, CGPoint* CurrentPoint);
   void inputTouchEnd(int ID, CGPoint* Point);
   
   void updateAccelerometerStatus(float X, float Y, float Z);   
};

#endif
