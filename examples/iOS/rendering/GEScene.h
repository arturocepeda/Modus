
//////////////////////////////////////////////////////////////////
//
//  Arturo Cepeda PŽrez
//  iOS Game Engine
//
//  Scene Management
//
//  --- GEScene.h ---
//
//////////////////////////////////////////////////////////////////


#ifndef _GESCENE_H_
#define _GESCENE_H_

#include "GERendering.h"
#include "GEUtils.h"

class GEScene
{
protected:
   // rendering system
   GERendering* cRender;
   
   // frame counter
   unsigned int iCurrentFrame;

   // input management
   GELine* cPixelToPositionX;
   GELine* cPixelToPositionY;
   
   // scene management
   int iNextScene;
   void sceneChange(unsigned int iNewScene);

public:
   GEScene(GERendering* Render, void* GlobalData);
   ~GEScene();

   virtual void init() = 0;      // IMPORTANT: always begin with [iNextScene = -1]
   virtual void release() = 0;
   
	virtual void update() = 0;
   virtual void render() = 0;

   virtual void inputTouchBegin(int ID, CGPoint* Point) = 0;
   virtual void inputTouchMove(int ID, CGPoint* PreviousPoint, CGPoint* CurrentPoint) = 0;
   virtual void inputTouchEnd(int ID, CGPoint* Point) = 0;
   
   virtual void updateAccelerometerStatus(float X, float Y, float Z) = 0;
   
   int getNextScene();
};

#endif
