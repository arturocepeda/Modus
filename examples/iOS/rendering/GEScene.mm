
//////////////////////////////////////////////////////////////////
//
//  Arturo Cepeda PŽrez
//  iOS Game Engine
//
//  Scene Management
//
//  --- GEScene.mm ---
//
//////////////////////////////////////////////////////////////////


#include "GEScene.h"

GEScene::GEScene(GERendering* Render, void* GlobalData)
{
   cRender = Render;

   cPixelToPositionX = new GELine(0.0f, -1.0f, GEDevice::getTouchPadSizeX(), 1.0f);   
   cPixelToPositionY = new GELine(0.0f, GEDevice::getAspectRatio(), 
                                  GEDevice::getTouchPadSizeY(), -GEDevice::getAspectRatio());
}

GEScene::~GEScene()
{
   delete cPixelToPositionX;
   delete cPixelToPositionY;
}

void GEScene::sceneChange(unsigned int iNewScene)
{
   iNextScene = (int)iNewScene;
}

int GEScene::getNextScene()
{
   return iNextScene;
}
