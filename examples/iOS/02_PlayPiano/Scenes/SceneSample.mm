
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
bool bSamplesLoaded = false;
bool bThreadEnd = false;


//
//  GESceneSampleThreads
//
void* GESceneSampleThreads::LoadSamplesThread(void* lp)
{
   GESceneSample* cScene = (GESceneSample*)lp;
   MCSoundGenAudio* mSoundGen = cScene->getSoundGen();

   mSoundGen->loadSamplePack(GEDevice::getResourcePath(@"PianoAAC.msp"),
                             GESceneSampleCallbacks::SampleLoaded, lp);

   mTimer->start();
   bSamplesLoaded = true;
   
   return NULL;
}

void* GESceneSampleThreads::MusicTimerThread(void* lp)
{
   while(!bThreadEnd)
      mTimer->update();
   
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
   fKeyboardOffset = 0.0f;
   fSlideSpeed = 0.0f;
   bKeyboardSlide = true;

   // textures
   cRender->loadTexture(Textures.KeyWhite, @"pianokey_white.png");
   cRender->loadTexture(Textures.KeyBlack, @"pianokey_black.png");
   cRender->loadTexture(Textures.KeyWhitePressed, @"pianokey_white_root.png");
   cRender->loadTexture(Textures.KeyBlackPressed, @"pianokey_black_root.png");
   cRender->loadTexture(Textures.PedalOn, @"pianopedal_on.png");
   cRender->loadTexture(Textures.PedalOff, @"pianopedal_off.png");
   cRender->loadTexture(Textures.PadlockOpen, @"padlock_open.png");
   cRender->loadTexture(Textures.PadlockClosed, @"padlock_closed.png");
   cRender->loadTexture(Textures.Loading, @"note.png");
   
   // sprites
   cSpriteUnused = new GESprite();
   
   cSpriteKeyWhite = new GESprite();   
   cSpriteKeyWhite->setTexture(cRender->getTexture(Textures.KeyWhite));
   
   cSpriteKeyBlack = new GESprite();
   cSpriteKeyBlack->setTexture(cRender->getTexture(Textures.KeyBlack));
   cSpriteKeyBlack->setScale(1.0f, 0.75f);
   
   cSpriteKeyWhitePressed = new GESprite();
   cSpriteKeyWhitePressed->setTexture(cRender->getTexture(Textures.KeyWhitePressed));
   
   cSpriteKeyBlackPressed = new GESprite();
   cSpriteKeyBlackPressed->setTexture(cRender->getTexture(Textures.KeyBlackPressed));
   cSpriteKeyBlackPressed->setScale(1.0f, 0.75f);
   
   cSpritePedal[0] = new GESprite();
   cSpritePedal[0]->setTexture(cRender->getTexture(Textures.PedalOff));
   cSpritePedal[0]->setScale(0.6f, 0.6f);
   
   cSpritePedal[1] = new GESprite();
   cSpritePedal[1]->setTexture(cRender->getTexture(Textures.PedalOn));
   cSpritePedal[1]->setScale(0.6f, 0.6f);
   
   cSpritePadlock[0] = new GESprite();
   cSpritePadlock[0]->setTexture(cRender->getTexture(Textures.PadlockClosed));
   cSpritePadlock[0]->setScale(0.15f, 0.15f);
   
   cSpritePadlock[1] = new GESprite();
   cSpritePadlock[1]->setTexture(cRender->getTexture(Textures.PadlockOpen));
   cSpritePadlock[1]->setScale(0.15f, 0.15f);
   
   cSpriteLoading = new GESprite();
   cSpriteLoading->setTexture(cRender->getTexture(Textures.Loading));
   
   const float fHalfPi = 1.5708f;
   
   cSpriteKeyWhite->setRotation(0.0f, 0.0f, fHalfPi);
   cSpriteKeyBlack->setRotation(0.0f, 0.0f, fHalfPi);
   cSpriteKeyWhitePressed->setRotation(0.0f, 0.0f, fHalfPi);
   cSpriteKeyBlackPressed->setRotation(0.0f, 0.0f, fHalfPi);
   cSpritePedal[0]->setRotation(0.0f, 0.0f, fHalfPi);
   cSpritePedal[1]->setRotation(0.0f, 0.0f, fHalfPi);
   cSpritePadlock[0]->setRotation(0.0f, 0.0f, fHalfPi);
   cSpritePadlock[1]->setRotation(0.0f, 0.0f, fHalfPi);
   cSpriteLoading->setRotation(0.0f, 0.0f, fHalfPi);

   // labels
   cTextModus = new GELabel(@"Modus\nC++ Music Library\nSample Application", @"Optima-ExtraBlack", 
                            28.0f, UITextAlignmentLeft, 1024, 128);
   cTextModus->setColor(0.2f, 0.2f, 0.2f);
   
   cTextLoading = new GELabel(@"", @"Optima-ExtraBlack",
                              28.0f, UITextAlignmentLeft, 1024, 64);
   cTextLoading->setColor(0.2f, 0.2f, 0.2f);
      
   if(GEDevice::iPhone())
      init_iPhone();
   else
      init_iPad();
   
   // initialize audio system
   cAudio = new CAudio();
   
   // Modus objects
   MSRange mPianoRange(21, 108);
   mPiano = new MCInstrument(1, mPianoRange, mPianoRange.getSize());
   
   mSoundGen = new MCSoundGenOpenAL(1, mPianoRange.getSize(), false);
   mPiano->setSoundGen(mSoundGen);
   
   mTimer = new MCTimer();
   mTimer->setCallbackTick(GESceneSampleCallbacks::TimerTick, this);
   
   // set instrument callbacks
   mPiano->setCallbackPlay(GESceneSampleCallbacks::PlayNote, this);
   mPiano->setCallbackRelease(GESceneSampleCallbacks::ReleaseNote, this);
   mPiano->setCallbackDamper(GESceneSampleCallbacks::Damper, this);
   
   // create music threads
   pthread_create(&pLoadSamplesThread, NULL, GESceneSampleThreads::LoadSamplesThread, this);
   pthread_create(&pMusicTimerThread, NULL, GESceneSampleThreads::MusicTimerThread, NULL);
}

void GESceneSample::init_iPhone()
{
   fSlideRatio = SLIDE_RATIO_A;
   
   cSpriteLoading->setScale(0.125f, 0.125f);
   
   cTextModus->setPosition(-1.35f, 0.9f);
   cTextModus->setScale(3.0f, 3.0f);
   cTextLoading->setPosition(-1.35f, 0.4f);
   cTextLoading->setScale(3.0f, 3.0f);
}

void GESceneSample::init_iPad()
{
   fSlideRatio = SLIDE_RATIO_B;
   
   cSpriteLoading->setScale(0.1f, 0.1f);
   
   cTextModus->setPosition(-1.2f, 0.9f);
   cTextModus->setScale(2.0f, 2.0f);
   cTextLoading->setPosition(-1.2f, 0.4f);
   cTextLoading->setScale(2.0f, 2.0f);
}

void GESceneSample::release()
{
   // wait until the music timer thread finishes
   bThreadEnd = true;
   pthread_join(pMusicTimerThread, NULL);
   
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
   delete cSpriteKeyWhitePressed;
   delete cSpriteKeyBlackPressed;
   delete cSpriteLoading;
   
   for(int i = 0; i < 2; i++)
   {
      delete cSpritePedal[i];
      delete cSpritePadlock[i];
   }
   
   // release labels
   delete cTextModus;   
   delete cTextLoading;
}

void GESceneSample::update()
{
   // keyboard update
   fKeyboardOffset += fSlideSpeed;
   
   fKeyboardOffset = fmin(BOUNDS_RIGHT, fKeyboardOffset);
   fKeyboardOffset = fmax(BOUNDS_LEFT, fKeyboardOffset);
   
   if(fSlideSpeed > 0.0f)
      fSlideSpeed -= SLIDE_FRICTION;
   else if(fSlideSpeed < 0.0f)
      fSlideSpeed += SLIDE_FRICTION;
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
}

void GESceneSample::render()
{
   // labels
   cRender->useProgram(GEPrograms.Text);
   cRender->renderLabel(cTextModus);
  
   if(!bSamplesLoaded)
   {
      int iPercentage = 1 + (int)(((float)iSamplesLoaded / iTotalSamples) * 100);
      NSString* sLoadingText = [[NSString alloc] initWithFormat:@"Loading audio samples: %d%%", iPercentage];
      
      cTextLoading->setText(sLoadingText);      
      cRender->renderLabel(cTextLoading);
      
      float fPosY = 0.0f;
      float fPosX = -1.25f;      
      
      cRender->useProgram(GEPrograms.HUD);
      
      for(unsigned int i = 1; i <= iPercentage / 5; i++)
      {
         cSpriteLoading->setPosition(fPosX, fPosY);
         cRender->renderSprite(cSpriteLoading);
         fPosX += 0.125f;
      }
      
      cRender->renderEnd();
      return;
   }   
   
   cRender->useProgram(GEPrograms.HUD);
   
   // piano keyboard   
   float fPosY = -0.1f;
   float fKeyPosX;
   
   int iNote;
   int iOctave;
   int iOffset;
   int i;
   
   // white keys
   fFirstKeyWhitePosX = fKeyboardOffset - KEY_WIDTH * 26 + (KEY_WIDTH / 2);  // number of white keys: 52, 52/2=26
   
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
      
      fKeyPosX = fFirstKeyWhitePosX + (KEY_WIDTH * iOffset);
      cSpriteKeyWhite->setPosition(fKeyPosX, fPosY);
      cRender->renderSprite(cSpriteKeyWhite);
      
      if(iIntensity[i] > 0)
      {
         cSpriteKeyWhitePressed->setPosition(fKeyPosX, fPosY);
         cSpriteKeyWhitePressed->setOpacity(iIntensity[i] / 127.0f);
         cRender->renderSprite(cSpriteKeyWhitePressed);
      }
   }   
   
   // black keys
   fPosY += 0.22f;
   fFirstKeyBlackPosX = fKeyboardOffset - KEY_WIDTH * 26;
   
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
      
      fKeyPosX = fFirstKeyBlackPosX + (KEY_WIDTH * iOffset);
      cSpriteKeyBlack->setPosition(fKeyPosX, fPosY);
      cRender->renderSprite(cSpriteKeyBlack);
      
      if(iIntensity[i] > 0)
      {
         cSpriteKeyBlackPressed->setPosition(fKeyPosX, fPosY);
         cSpriteKeyBlackPressed->setOpacity(iIntensity[i] / 127.0f);
         cRender->renderSprite(cSpriteKeyBlackPressed);
      }
   }
   
   // piano pedal
   cSpritePedal[bDamper]->setPosition(0.0f, fPosY - 0.87f);
   cRender->renderSprite(cSpritePedal[bDamper]);
   
   // padlock
   cSpritePadlock[bKeyboardSlide]->setPosition(1.1f, 0.7f);
   cRender->renderSprite(cSpritePadlock[bKeyboardSlide]);
}

unsigned char GESceneSample::findKey(float fTouchX, float fTouchY)
{
   float fKeyNumber = (fTouchX - fFirstKeyWhitePosX) / (KEY_WIDTH * KEY_SCALE);
   unsigned char iKeyNumber = (unsigned char)fKeyNumber;
   
   // when the finger is under the black keys, it must be a white key
   if(fTouchY < ((BOUNDS_TOP + BOUNDS_BOTTOM) / 2) && !MCNotes::isNatural(iKeyNumber + LOWEST_NOTE))
   {
      // check only decimal value
      fKeyNumber -= (float)iKeyNumber;
      
      if(fKeyNumber > 0.5f)
         iKeyNumber++;
      else
         iKeyNumber--;
   }
   
   return iKeyNumber;
}

void GESceneSample::inputTouchBegin(int ID, CGPoint* Point)
{
   if(!bSamplesLoaded)
      return;
   
   // stop sliding
   fSlideSpeed = 0.0f;

   // check position
   float fTouchX = -cPixelToPositionY->y(Point->x);
   float fTouchY = -cPixelToPositionX->y(Point->y);
   
   // padlock
   if(fTouchY < 0.9f && fTouchY > 0.5f && fTouchX < 1.3f && fTouchX > 0.9f)
   {
      bKeyboardSlide = !bKeyboardSlide;
      return;
   }

   // pedal
   if(fTouchY < BOUNDS_BOTTOM)
   {
      mPiano->setDamper(!bDamper);
      return;
   }
   
   // key
   else if(fTouchY < BOUNDS_TOP)
   {
      MSNote mNote;      
      mNote.Channel = findKey(fTouchX, fTouchY);
      mNote.Pitch = mNote.Channel + LOWEST_NOTE;
      mNote.Intensity = 127;
      mNote.Mode = 0;
      mNote.Duration = 0;
            
      iFingerChannel[ID] = mNote.Channel;      
      mPiano->playNote(mNote);
   }
}

void GESceneSample::inputTouchMove(int ID, CGPoint* PreviousPoint, CGPoint* CurrentPoint)
{
   if(bKeyboardSlide)
   {
      // only the first finger can slide the keyboard
      if(ID == 0)
         fSlideSpeed = (CurrentPoint->x - PreviousPoint->x) * fSlideRatio;
   }
   else
   {
      // check position
      float fTouchX = -cPixelToPositionY->y(CurrentPoint->x);
      float fTouchY = -cPixelToPositionX->y(CurrentPoint->y);
       
      if(fTouchY > BOUNDS_TOP || fTouchY < BOUNDS_BOTTOM)
         return;
      
      unsigned char iCurrentChannel = findKey(fTouchX, fTouchY);
      
      if(iFingerChannel[ID] != iCurrentChannel)
      {
         // release previous note
         mPiano->release(iFingerChannel[ID]);
         
         // play new note
         MSNote mNote;      
         mNote.Channel = iCurrentChannel;
         mNote.Pitch = mNote.Channel + LOWEST_NOTE;
         mNote.Intensity = 127;
         mNote.Mode = 0;
         mNote.Duration = 0;
         
         iFingerChannel[ID] = mNote.Channel;      
         mPiano->playNote(mNote);
      }
   }
}

void GESceneSample::inputTouchEnd(int ID, CGPoint* Point)
{
   mPiano->release(iFingerChannel[ID]);
}

void GESceneSample::updateAccelerometerStatus(float X, float Y, float Z)
{    
}

