
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

#define FINGERS         5
#define OPENAL_SOURCES  32

#define LOWEST_NOTE     21
#define KEY_WIDTH       0.275f
#define KEY_SCALE       0.58f

#define BOUNDS_TOP      0.5f
#define BOUNDS_BOTTOM  -0.7f
#define BOUNDS_LEFT    -(25 * KEY_WIDTH)
#define BOUNDS_RIGHT    (25 * KEY_WIDTH)

#define SLIDE_RATIO_A   0.0058f
#define SLIDE_RATIO_B   0.0018f
#define SLIDE_FRICTION  0.0007f

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
   GESprite* cSpriteKeyWhitePressed;
   GESprite* cSpriteKeyBlackPressed;
   GESprite* cSpritePedal[2];
   GESprite* cSpritePadlock[2];
   GESprite* cSpriteLoading;
   
   // labels
   GELabel* cTextModus;
   GELabel* cTextLoading;
   
   // textures
   struct
   {
      enum {KeyWhite, KeyBlack, KeyWhitePressed, KeyBlackPressed, PedalOn, PedalOff,
            PadlockOpen, PadlockClosed, Loading};      
   } 
   Textures;
   
   // sound system
   CAudio* cAudio;
   
   // device properties
   float fSlideRatio;
   
   // Modus objects
   MCInstrument* mPiano;
   MCOpenALSourceManager* mSourceManager;
   MCSoundGenOpenAL* mSoundGen;
   
   // sample-pack
   unsigned int iTotalSamples;
   unsigned int iSamplesLoaded;
   
   // piano status
   float fKeyboardOffset;
   float fSlideSpeed;
   float fFirstKeyWhitePosX;
   float fFirstKeyBlackPosX;
   bool bKeyboardSlide;
   
   unsigned char iFingerChannel[FINGERS];
   unsigned char iIntensity[88];
   bool bDamper;
   
   // music threads
   pthread_t pMusicTimerThread;
   pthread_t pLoadSamplesThread;
   
   void init_iPhone();
   void init_iPad();
   
   unsigned char findKey(float fTouchX, float fTouchY);
   
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
