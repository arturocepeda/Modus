
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
#include "GERenderingShaders.h"

#define TEXTURES 256

class GERendering
{
private:
   EAGLContext* glContext;
   GEColor cBackgroundColor;
   GEColor cColor;
   
   // matrices
   GLKMatrix4 matProjection;
   GLKMatrix4 matView;
   GLKMatrix4 matModel;
   
   GLKMatrix3 matNormal;
   GLKMatrix4 matModelView;
   GLKMatrix4 matModelViewProjection;

   // textures
   GLuint tTextures[TEXTURES];
   GETextureSize tTextureSize[TEXTURES];
   
   // shaders
   GEProgram sPrograms[GEPrograms.Count];
   GLuint iUniforms[GEPrograms.Count][GEUniforms.Count];
   unsigned int iActiveProgram;
   
   void loadShaders();   
   void attachShaders(unsigned int iProgramIndex, GEVertexShader& cVertexShader, GEFragmentShader& cFragmentShader);
   void linkProgram(unsigned int iProgramIndex);
   bool checkProgram(unsigned int iProgramIndex);
   void getUniformsLocation(unsigned int iProgramIndex);

public:
	GERendering(EAGLContext* Context);
	~GERendering();

   // textures
   void loadTexture(unsigned int TextureIndex, NSString* Name);
   void loadTextureCompressed(unsigned int TextureIndex, NSString* Name,
                              unsigned int Size, unsigned int BPP, bool Alpha = false);
   GLuint getTexture(unsigned int TextureIndex);
   GETextureSize& getTextureSize(unsigned int TextureIndex);
   
   // background
   void setBackgroundColor(float R, float G, float B);
   
   // rendering
   void renderBegin();
   void renderSprite(GESprite* Sprite);
   void renderLabel(GELabel* Label);
   void renderEnd();
   
   // rendering mode
   void set2D(bool Portrait = true);
   void useProgram(unsigned int iProgramIndex);
};

#endif
