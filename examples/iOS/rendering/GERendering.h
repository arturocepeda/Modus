
//////////////////////////////////////////////////////////////////
//
//  Arturo Cepeda PŽrez
//  iOS Game Engine
//
//  Rendering Engine (OpenGL ES)
//
//  --- GERendering.h ---
//
//////////////////////////////////////////////////////////////////


#ifndef _GERENDERING_H_
#define _GERENDERING_H_

#include "GERenderingObjects.h"

#define TEXTURES 256

class GERendering
{
private:
   EAGLContext* glContext;
   
   GLuint tTextures[TEXTURES];
   GETextureSize tTextureSize[TEXTURES];
   
   GEColor cBackgroundColor;

public:
	GERendering(EAGLContext* Context);
	~GERendering();

   // textures
   void loadTexture(unsigned int TextureIndex, NSString* Name);
   GLuint getTexture(unsigned int TextureIndex);
   GETextureSize& getTextureSize(unsigned int TextureIndex);
   
   // lighting
   void switchLighting(bool On);
   
   // background
   void setBackgroundColor(float R, float G, float B);
   
   // camera
   void useCamera(GECamera* Camera);

   // rendering
   void renderBegin();
   void renderMesh(GEMesh* Mesh);
   void renderSprite(GESprite* Sprite);
   void renderLabel(GELabel* Label);
   void renderEnd();
   
   // rendering mode
   void set2D(bool Portrait = true);
   void set3D();
};

#endif
