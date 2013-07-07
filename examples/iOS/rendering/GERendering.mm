
//////////////////////////////////////////////////////////////////
//
//  Arturo Cepeda PŽrez
//  iOS Game Engine
//
//  Rendering Engine (OpenGL ES)
//
//  --- GERendering.mm ---
//
//////////////////////////////////////////////////////////////////


#include "GERendering.h"


//
//  GERendering
//
GERendering::GERendering(EAGLContext* Context)
{   
   glContext = Context;
   cBackgroundColor.set(1.0f, 1.0f, 1.0f);
   
	// enable OpenGL texturing
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_2D);
   glEnable(GL_BLEND);
   glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
   glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
   
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   
   // generate textures
   glGenTextures(TEXTURES, tTextures);
   memset(tTextureSize, 0, sizeof(GETextureSize) * TEXTURES);
   
   // 3D mode
   set3D();
}

GERendering::~GERendering()
{
   // release textures
   glDeleteTextures(TEXTURES, tTextures);
}

void GERendering::loadTexture(unsigned int TextureIndex, NSString* Name)
{
   if(TextureIndex >= TEXTURES)
      return;
   
	CGImageRef cgImage = [UIImage imageNamed:Name].CGImage;
	CGContextRef cgContext;
	GLubyte* pData = nil;	
	size_t iWidth, iHeight;
	
	if(!cgImage)
      return;
   
   glBindTexture(GL_TEXTURE_2D, tTextures[TextureIndex]);
   
	iWidth = CGImageGetWidth(cgImage);
	iHeight = CGImageGetHeight(cgImage);
   
   tTextureSize[TextureIndex].Width = iWidth;
   tTextureSize[TextureIndex].Height = iHeight;
	
	pData = (GLubyte*)calloc(iWidth * iHeight * 4, sizeof(GLubyte));
   
	// Uses the bitmatp creation function provided by the Core Graphics framework. 
	cgContext = CGBitmapContextCreate(pData, iWidth, iHeight, 8, iWidth * 4, 
                                     CGImageGetColorSpace(cgImage), kCGImageAlphaPremultipliedLast);
   
	// After you create the context, you can draw the image to the context.
	CGContextDrawImage(cgContext, CGRectMake(0.0f, 0.0f, (CGFloat)iWidth, (CGFloat)iHeight), 
                      cgImage);
   
	// You don't need the context at this point, so you need to release it to avoid memory leaks.
	CGContextRelease(cgContext);
   
	// setup texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
   
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, iWidth, iHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, pData);
	free(pData);
   
   glBindTexture(GL_TEXTURE_2D, 0);
}

GLuint GERendering::getTexture(unsigned int TextureIndex)
{
   return tTextures[TextureIndex];
}

GETextureSize& GERendering::getTextureSize(unsigned int TextureIndex)
{
   return tTextureSize[TextureIndex];
}


//
//  lighting
//
void GERendering::switchLighting(bool On)
{
   if(On)
      glEnable(GL_LIGHTING);
   else
      glDisable(GL_LIGHTING);
}


//
//  Background 
//
void GERendering::setBackgroundColor(float R, float G, float B)
{
   cBackgroundColor.set(R, G, B);
}


//
//  Camera
//
void GERendering::useCamera(GECamera* Camera)
{
   glMatrixMode(GL_PROJECTION);   
   glLoadIdentity();
   
   float fLeft, fRight;
   float fTop, fBottom;
   
   //
   // Distance to the projection plane = 1 / tan(FOVangle / 2)
   // Taking FOVangle = PI / 4, then distance = 1 / tan(PI / 8)
   //
   float fDistance = 1 / tan(PI * 0.125);
   
   fRight = ZNEAR / fDistance;
   fLeft = -fRight;
   
   fBottom = GEDevice::getAspectRatio() * fLeft;
   fTop = GEDevice::getAspectRatio() * fRight;
   
   glFrustumf(fLeft, fRight, fBottom, fTop, ZNEAR, ZFAR);
   
   GEVector vRotation;
   Camera->getRotation(&vRotation);
   
   GEVector vEye;
   Camera->getPosition(&vEye);
   
   glRotatef(vRotation.X, 1.0f, 0.0f, 0.0f);
   glRotatef(vRotation.Y, 0.0f, 1.0f, 0.0f);
   glRotatef(vRotation.Z, 0.0f, 0.0f, 1.0f);
   glTranslatef(vEye.X, vEye.Y, vEye.Z);
}


//
//  Rendering
//
void GERendering::renderBegin()
{
   glClearColor(cBackgroundColor.R, cBackgroundColor.G, cBackgroundColor.B, 1.0f);
	glClearDepthf(1.0f);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GERendering::renderMesh(GEMesh* Mesh)
{
   Mesh->render();
}

void GERendering::renderSprite(GESprite* Sprite)
{
   Sprite->render();
}

void GERendering::renderLabel(GELabel* Label)
{
   Label->render();
}

void GERendering::renderEnd()
{
	glBindTexture(GL_TEXTURE_2D, 0);
   [glContext presentRenderbuffer:GL_RENDERBUFFER_OES];
}

void GERendering::set2D(bool Portrait)
{
   glDisable(GL_DEPTH_TEST);
   
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   
   if(Portrait)
      glOrthof(-1.0f, 1.0f, -GEDevice::getAspectRatio(), GEDevice::getAspectRatio(), -1.0f, 1.0f);
   else
      glOrthof(-GEDevice::getAspectRatio(), GEDevice::getAspectRatio(), -1.0f, 1.0f, -1.0f, 1.0f);
}

void GERendering::set3D()
{
	glEnable(GL_DEPTH_TEST);
}
