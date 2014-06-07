
////////////////////////////////////////////////////////////////////////
//
//  Modus
//  C++ Music Library
//  Sample Application
//
//  Arturo Cepeda
//
////////////////////////////////////////////////////////////////////////


#include "SceneSample.h"
#include "GEUtils.h"


//
//  global data
//
MCTimer* mTimer;
pthread_mutex_t pMutex;
bool bSamplesLoaded = false;
bool bThreadEnd = false;


//
//  GESceneSampleThreads
//
void* GESceneSampleThreads::LoadSamplesThread(void* lp)
{
   GESceneSample* cScene = (GESceneSample*)lp;
   MCSoundGenAudio* mSoundGen = cScene->getSoundGen();
   
   mSoundGen->loadSamplePack(GEDevice::getResourcePath(@"PianoAAC.msp"), GESceneSampleCallbacks::SampleLoaded, lp);
   
   pthread_mutex_init(&pMutex, NULL);
   mTimer->start();
   bSamplesLoaded = true;
   
   return NULL;
}

void* GESceneSampleThreads::MusicTimerThread(void* lp)
{
   while(!bThreadEnd)
   {
      pthread_mutex_lock(&pMutex);
      mTimer->update();
      pthread_mutex_unlock(&pMutex);
   }
   
   return NULL;
}


//
//  GESceneSampleCallbacks
//
void GESceneSampleCallbacks::SampleLoaded(unsigned int TotalSamples, unsigned int Loaded, void* Data)
{
   GESceneSample* cScene = (GESceneSample*)Data;
   cScene->updateSamplesLoaded(TotalSamples, Loaded);
}

void GESceneSampleCallbacks::TimerTick(const MSTimePosition& TimePosition, void* Data)
{
   GESceneSample* cScene = (GESceneSample*)Data;
   cScene->updatePiano(TimePosition);
}

void GESceneSampleCallbacks::PlayNote(unsigned int Instrument, const MSNote& Note, void* Data)
{
   GESceneSample* cScene = (GESceneSample*)Data;   
   cScene->setIntensity(Note.Pitch, Note.Intensity);
}

void GESceneSampleCallbacks::ReleaseNote(unsigned int Instrument, const MSNote& Note, void* Data)
{
   GESceneSample* cScene = (GESceneSample*)Data;
   cScene->setIntensity(Note.Pitch, 0);
}

void GESceneSampleCallbacks::Damper(unsigned int Instrument, bool DamperState, void* Data)
{
   GESceneSample* cScene = (GESceneSample*)Data;
   cScene->setDamper(DamperState);
}


//
//  GESceneSample
//
GESceneSample::GESceneSample(GERendering* Render, void* GlobalData) : GEScene(Render, GlobalData)
{
   // initialize piano status
   memset(iIntensity, 0, 88);
   bDamper = false;
}

void GESceneSample::init()
{   
   iNextScene = -1;
   cRender->setBackgroundColor(0.8f, 0.8f, 1.0f);
   cRender->set2D(false);
   
   iTotalSamples = 0;
   iSamplesLoaded = 0;

   // textures
   cRender->loadTexture(Textures.KeyWhite, @"pianokey_white.png");
   cRender->loadTexture(Textures.KeyBlack, @"pianokey_black.png");
   cRender->loadTexture(Textures.KeyPress, @"pianokey_press.png");
   cRender->loadTexture(Textures.KeyPressBack, @"pianokey_press_back.png");
   cRender->loadTexture(Textures.PedalOn, @"pianopedal_on.png");
   cRender->loadTexture(Textures.PedalOff, @"pianopedal_off.png");
   cRender->loadTexture(Textures.Loading, @"note.png");
   
   // sprites
   cSpriteUnused = new GESprite();
   cSpriteKeyWhite = new GESprite();
   cSpriteKeyWhite->setTexture(cRender->getTexture(Textures.KeyWhite));
   cSpriteKeyBlack = new GESprite();
   cSpriteKeyBlack->setTexture(cRender->getTexture(Textures.KeyBlack));
   cSpriteKeyPress = new GESprite();
   cSpriteKeyPress->setTexture(cRender->getTexture(Textures.KeyPress));
   cSpriteKeyPressBack = new GESprite();
   cSpriteKeyPressBack->setTexture(cRender->getTexture(Textures.KeyPressBack));
   cSpritePedalOn = new GESprite();
   cSpritePedalOn->setTexture(cRender->getTexture(Textures.PedalOn));
   cSpritePedalOff = new GESprite();
   cSpritePedalOff->setTexture(cRender->getTexture(Textures.PedalOff));
   cSpritePedal = cSpritePedalOff;
   cSpriteLoading = new GESprite();
   cSpriteLoading->setTexture(cRender->getTexture(Textures.Loading));
   
   const float fHalfPi = 1.5708f;
   
   cSpriteKeyWhite->setRotation(0.0f, 0.0f, fHalfPi);
   cSpriteKeyBlack->setRotation(0.0f, 0.0f, fHalfPi);
   cSpriteKeyPress->setRotation(0.0f, 0.0f, fHalfPi);
   cSpriteKeyPressBack->setRotation(0.0f, 0.0f, fHalfPi);
   cSpritePedalOn->setRotation(0.0f, 0.0f, fHalfPi);
   cSpritePedalOff->setRotation(0.0f, 0.0f, fHalfPi);
   cSpriteLoading->setRotation(0.0f, 0.0f, fHalfPi);

   // labels
   cTextModus = new GELabel(@"Modus\nC++ Music Library\nSample Application", @"Optima-ExtraBlack", 
                            28.0f, UITextAlignmentLeft, 1024, 128);
   cTextModus->setColor(0.2f, 0.2f, 0.2f);
   
   cTextLoading = new GELabel(@"", @"Optima-ExtraBlack",
                              28.0f, UITextAlignmentLeft, 1024, 64);
   cTextLoading->setColor(0.2f, 0.2f, 0.2f);
   
   cTextPlaying = new GELabel(@"Playing! Touch the screen to change the piece...", @"Optima-ExtraBlack",
                              28.0f, UITextAlignmentLeft, 1024, 64);
   cTextPlaying->setColor(0.2f, 0.2f, 0.2f);
   
   if(GEDevice::iPhone())
      init_iPhone();
   else
      init_iPad();
   
   // initialize audio system
   cAudio = new CAudio();
   
   // Modus objects
   MSRange mPianoRange(21, 108);
   mPiano = new MCInstrument(1, mPianoRange, mPianoRange.getSize());
   
   mSoundGen = new MCSoundGenOpenAL(1,  mPianoRange.getSize(), false);
   mPiano->setSoundGen(mSoundGen);
   
   mScore[0].loadScriptFromFile(GEDevice::getResourcePath(@"score.piano.chopin.txt"));
   mScore[1].loadScriptFromFile(GEDevice::getResourcePath(@"score.piano.chopin2.txt"));
   mScore[1].displace(1);
   
   iCurrentScore = 0;
   mPiano->setScore(&mScore[iCurrentScore]);
   
   mTimer = new MCTimer();
   mTimer->setCallbackTick(GESceneSampleCallbacks::TimerTick, this);
   
   // set instrument callbacks
   mPiano->setCallbackPlay(GESceneSampleCallbacks::PlayNote, this);
   mPiano->setCallbackRelease(GESceneSampleCallbacks::ReleaseNote, this);
   mPiano->setCallbackDamper(GESceneSampleCallbacks::Damper, this);
   
   // create music threads
   pthread_create(&pLoadSamples, NULL, GESceneSampleThreads::LoadSamplesThread, this);
   pthread_create(&pMusicTimerThread, NULL, GESceneSampleThreads::MusicTimerThread, NULL);
}

void GESceneSample::init_iPhone()
{
   cSpriteKeyWhite->setScale(0.2f, 0.2f);
   cSpriteKeyBlack->setScale(0.2f, 0.2f);
   cSpriteKeyPress->setScale(0.2f, 0.2f);
   cSpriteKeyPressBack->setScale(0.2f, 0.2f);
   cSpritePedalOn->setScale(0.2f, 0.2f);
   cSpritePedalOff->setScale(0.2f, 0.2f);
   cSpriteLoading->setScale(0.125f, 0.125f);
   
   cTextModus->setPosition(-1.35f, 0.9f);
   cTextModus->setScale(3.0f, 3.0f);
   cTextLoading->setPosition(-1.35f, 0.4f);
   cTextLoading->setScale(3.0f, 3.0f);
   cTextPlaying->setPosition(-1.35f, 0.4f);
   cTextPlaying->setScale(3.0f, 3.0f);
}

void GESceneSample::init_iPad()
{
   cSpriteKeyWhite->setScale(0.2f, 0.2f);
   cSpriteKeyBlack->setScale(0.2f, 0.15f);
   cSpriteKeyPress->setScale(0.2f, 0.2f);
   cSpriteKeyPressBack->setScale(0.2f, 0.15f);
   cSpritePedalOn->setScale(0.2f, 0.2f);
   cSpritePedalOff->setScale(0.2f, 0.2f);
   cSpriteLoading->setScale(0.1f, 0.1f);
   
   cTextModus->setPosition(-1.2f, 0.9f);
   cTextModus->setScale(2.0f, 2.0f);
   cTextLoading->setPosition(-1.2f, 0.4f);
   cTextLoading->setScale(2.0f, 2.0f);
   cTextPlaying->setPosition(-1.2f, 0.4f);
   cTextPlaying->setScale(2.0f, 2.0f);
}

void GESceneSample::release()
{
   // wait until the music timer thread finishes
   bThreadEnd = true;
   pthread_join(pMusicTimerThread, NULL);
   pthread_mutex_destroy(&pMutex);   
   
   // release Modus objects
   delete mSoundGen;
   delete mPiano;
   delete mTimer;
   
   // release audio system
   delete cAudio;
   
   // release sprites
   delete cSpriteUnused;
   delete cSpriteKeyWhite;
   delete cSpriteKeyBlack;
   delete cSpriteKeyPress;
   delete cSpriteKeyPressBack;
   delete cSpritePedalOn;
   delete cSpritePedalOff;
   delete cSpriteLoading;
   
   // release labels
   delete cTextModus;   
   delete cTextLoading;
   delete cTextPlaying;
}

void GESceneSample::update()
{
}

void GESceneSample::updateSamplesLoaded(unsigned int TotalSamples, unsigned int Loaded)
{
   iTotalSamples = TotalSamples;
   iSamplesLoaded = Loaded;
}

void GESceneSample::updatePiano(const MSTimePosition& TimePosition)
{
   mPiano->update(TimePosition);
}

MCSoundGenOpenAL* GESceneSample::getSoundGen()
{
   return mSoundGen;
}

void GESceneSample::setIntensity(unsigned char Pitch, unsigned char Intensity)
{
   iIntensity[Pitch - LOWEST_NOTE] = Intensity;
}

void GESceneSample::setDamper(bool On)
{
   bDamper = On;
   cSpritePedal = bDamper? cSpritePedalOn: cSpritePedalOff;
}

void GESceneSample::render()
{
   // labels   
   cRender->useProgram(GEPrograms.Text);
   cRender->renderLabel(cTextModus);
  
   if(bSamplesLoaded)
   {
      cRender->renderLabel(cTextPlaying);
   }
   else 
   {
      int iPercentage = 1 + (int)(((float)iSamplesLoaded / iTotalSamples) * 100);
      NSString* sLoadingText = [[NSString alloc] initWithFormat:@"Loading audio samples: %d%%", iPercentage];
      
      cTextLoading->setText(sLoadingText);
      cRender->renderLabel(cTextLoading);
      
      float fPosY = 0.0f;
      float fPosX = (GEDevice::iPhone())? -1.25f: -1.1f;
      cRender->useProgram(GEPrograms.HUD);
      
      for(unsigned int i = 1; i <= iPercentage / 5; i++)
      {
         cSpriteLoading->setPosition(fPosX, fPosY);
         cRender->renderSprite(cSpriteLoading);
         fPosX += (GEDevice::iPhone())? 0.125f: 0.11f;
      }
      
      cRender->renderEnd();
      return;
   }
   
   cRender->useProgram(GEPrograms.HUD);
   
   // piano keyboard
   float fPosY = -0.2f;
   float fKeyPosX;
   float fWhiteWidth = 0.05f;
   float fFirstKeyPosX = -fWhiteWidth * 26 + (fWhiteWidth / 2);  // number of white keys: 52, 52/2=26

   int iNote;
   int iOctave;
   int iOffset;
   int i;
   
   // white keys
   for(i = 0; i < 88; i++)
   {
      if(!MCNotes::isNatural(i + LOWEST_NOTE))
         continue;
      
      iNote = (i + LOWEST_NOTE) % 12;
      iOctave = (i + LOWEST_NOTE) / 12 - 2;  // pitch: [21, 108] --> octave: [0, 7]
      iOffset = 2 + (iOctave * 7);           // initial offset 2: the first C is the 3rd key
      
      switch(iNote)
      {
         case D:
            iOffset += 1;
            break;
         case E:
            iOffset += 2;
            break;
         case F:
            iOffset += 3;
            break;
         case G:
            iOffset += 4;
            break;
         case A:
            iOffset += 5;
            break;
         case B:
            iOffset += 6;
            break;
      }
      
      fKeyPosX = fFirstKeyPosX + (fWhiteWidth * iOffset);
      cSpriteKeyWhite->setPosition(fKeyPosX, fPosY);
      cRender->renderSprite(cSpriteKeyWhite);
      
      if(iIntensity[i] > 0)
      {
         cSpriteKeyPress->setPosition(fKeyPosX, fPosY);
         cSpriteKeyPress->setOpacity(iIntensity[i] / 127.0f);
         cRender->renderSprite(cSpriteKeyPress);
      }
   }   
   
   // black keys
   fPosY += 0.045f;
   fFirstKeyPosX = -fWhiteWidth * 26;
   
   for(i = 0; i < 88; i++)
   {
      if(MCNotes::isNatural(i + LOWEST_NOTE))
         continue;
      
      iNote = (i + LOWEST_NOTE) % 12;
      iOctave = (i + LOWEST_NOTE) / 12 - 2;  // pitch: [21, 108] --> octave: [0, 7]
      iOffset = 2 + (iOctave * 7);           // initial offset 2: the first C is the 3rd key
      
      switch(iNote)
      {
         case Cs:
            iOffset += 1;
            break;
         case Ds:
            iOffset += 2;
            break;
         case Fs:
            iOffset += 4;
            break;
         case Gs:
            iOffset += 5;
            break;
         case As:
            iOffset += 6;
            break;
      }
      
      fKeyPosX = fFirstKeyPosX + (fWhiteWidth * iOffset);
      cSpriteKeyBlack->setPosition(fKeyPosX, fPosY);
      cRender->renderSprite(cSpriteKeyBlack);
      
      if(iIntensity[i] > 0)
      {
         cSpriteKeyPressBack->setPosition(fKeyPosX, fPosY + 0.05f);
         cRender->renderSprite(cSpriteKeyPressBack);
         
         cSpriteKeyPress->setPosition(fKeyPosX, fPosY + 0.05f);
         cSpriteKeyPress->setOpacity(iIntensity[i] / 127.0f);
         cRender->renderSprite(cSpriteKeyPress);
      }
   }
   
   // piano pedal
   cSpritePedal->setPosition(0.0f, fPosY - 0.19f);
   cRender->renderSprite(cSpritePedal);
}

void GESceneSample::inputTouchBegin(int ID, CGPoint* Point)
{
   if(!bSamplesLoaded)
      return;
   
   iCurrentScore++;
   
   if(iCurrentScore == SCORES)
      iCurrentScore = 0;
   
   mPiano->setDamper(false);
   mPiano->releaseAll();
   mPiano->setScore(&mScore[iCurrentScore]);
   
   pthread_mutex_lock(&pMutex);
   mTimer->reset();
   mTimer->start();
   pthread_mutex_unlock(&pMutex);
}

void GESceneSample::inputTouchMove(int ID, CGPoint* PreviousPoint, CGPoint* CurrentPoint)
{
}

void GESceneSample::inputTouchEnd(int ID, CGPoint* Point)
{
}

void GESceneSample::updateAccelerometerStatus(float X, float Y, float Z)
{
}
