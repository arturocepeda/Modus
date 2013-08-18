
//////////////////////////////////////////////////////////////////
//
//  Arturo Cepeda PŽrez
//  iOS Game Engine
//
//  Rendering Engine (OpenGL ES)
//
//  --- GERenderingObjects.h ---
//
//////////////////////////////////////////////////////////////////


#ifndef _GERENDERINGOBJECTS_H_
#define _GERENDERINGOBJECTS_H_

#include "GEDevice.h"

#include <GLKit/GLKit.h>

#define ZNEAR     0.1f
#define ZFAR      1000.0f

#define PI        3.14159f
#define DEG2RAD   57.2958f    // (180/PI)


//
//  GEColor
//
struct GEColor
{
   float R;
   float G;
   float B;
   
   GEColor()
   {
      set(1.0f, 1.0f, 1.0f);
   }
   
   GEColor(float cR, float cG, float cB)
   {
      set(cR, cG, cB);
   }
   
   void set(float cR, float cG, float cB)
   {
      R = cR;
      G = cG;
      B = cB;
   }
};


//
//  GEVector
//
struct GEVector
{
   float X;
   float Y;
   float Z;
   
   GEVector()
   {
      set(0.0f, 0.0f, 0.0f);
   }
   
   GEVector(float vX, float vY, float vZ)
   {
      set(vX, vY, vZ);
   }
   
   void set(float vX, float vY, float vZ)
   {
      X = vX;
      Y = vY;
      Z = vZ;
   }
   
   const GEVector operator +(const GEVector& vVector)
   {
      return GEVector(X + vVector.X, Y + vVector.Y, Z + vVector.Z);
   }
   
   const GEVector operator +=(const GEVector& vVector)
   {
      return GEVector(X + vVector.X, Y + vVector.Y, Z + vVector.Z);
   }
   
   const GEVector operator -(const GEVector& vVector)
   {
      return GEVector(X - vVector.X, Y - vVector.Y, Z - vVector.Z);
   }

   const GEVector operator -=(const GEVector& vVector)
   {
      return GEVector(X - vVector.X, Y - vVector.Y, Z - vVector.Z);
   }
   
   void normalize()
   {
      float fLength = sqrt(X * X + Y * Y + Z * Z);
      
      X /= fLength;
      Y /= fLength;
      Z /= fLength;
   }
   
   GEVector crossProduct(const GEVector& vVector)
   {
      GEVector vCrossProduct;
      
      vCrossProduct.X = Y * vVector.Z - Z * vVector.Y;
      vCrossProduct.Y = Z * vVector.X - X * vVector.Z;
      vCrossProduct.Z = X * vVector.Y - Y * vVector.X;
      
      return vCrossProduct;
   }
};


//
//  GELight
//
struct GELight
{
   GEVector Position;
   GEColor Color;
   float Intensity;
};


//
//  GETextureSize
//
struct GETextureSize
{
   unsigned int Width;
   unsigned int Height;
};


//
//  Shader uniforms
//
struct
{
   enum
   {
      ModelViewProjection, 
      ModelView,
      Normal,
      ObjectColor,
      Texture0,
      
      Count
   };
} 
GEUniforms;


//
//  Vertex attributes
//
struct
{
   enum
   {
      Position,
      TextureCoord0,
      Normal,
      
      Count
   };
}
GEVertexAttributes;


//
//  Shader programs
//
struct 
{
   enum
   {
      HUD,
      Text,
      
      Count
   };
}
GEPrograms;


//
//  GERenderingObject
//
class GERenderingObject
{
protected:
   GEVector vPosition;
   GEVector vRotation;
   GEVector vScale;
   
public:
   void move(float DX, float DY, float DZ = 0.0f);
   void move(const GEVector& Move);
   void scale(float SX, float SY, float SZ = 1.0f);
   void scale(const GEVector& Scale);
   void rotate(float RX, float RY, float RZ);
   void rotate(const GEVector& Rotate);

   void getPosition(GEVector* Position);
   void getRotation(GEVector* Rotation);
   void getScale(GEVector* Scale);  

   void setPosition(float X, float Y, float Z = 0.0f);
   void setPosition(const GEVector& Position);
   void setScale(float X, float Y, float Z = 1.0f);
   void setScale(const GEVector& Scale);
   void setRotation(float X, float Y, float Z);
   void setRotation(const GEVector& Rotation);
};



//
//  GERenderingObjectVisible
//
class GERenderingObjectVisible : public GERenderingObject
{
protected:
   unsigned int iNumVertices;
   
   float fOpacity;
   bool bVisible;
   GEColor cColor;
   GLuint iTexture;
   
   // vertex attributes pointers
   float* fVertex;
   float* fNormals;
   float* fTextureCoordinate;
   
   // vertex array and vertex buffers
   GLuint iVertexArray;
   GLuint iVertexBuffers[GEVertexAttributes.Count];
   
public:
   void show();
   void hide();
   
   virtual void render() = 0;
   
   void getModelMatrix(GLKMatrix4* ModelMatrix);
   void getColor(GEColor* Color);
   float getOpacity();
   bool getVisible();
   GLuint getTexture();
      
   void setColor(float R, float G, float B);
   void setColor(const GEColor& Color);
   void setOpacity(float Opacity);
   void setVisible(bool Visible);
   void setTexture(GLuint Texture);
};



//
//  GESprite
//
class GESprite : public GERenderingObjectVisible
{
public:
   GESprite();
   ~GESprite();

   void loadFromFile(const char* Filename);
   void unload();

   void render();
   
   void setCenter(float X, float Y, float Z);
   void setTextureCoordinates(float Ax, float Ay, float Bx, float By,
                              float Cx, float Cy, float Dx, float Dy);
};


//
//  GELabel
//
class GELabel : public GERenderingObjectVisible
{
private:
   NSString* sText;
   NSString* sFont;
   UITextAlignment tAligment;
   float fFontSize;
   
   unsigned int iWidth;
   unsigned int iHeight;
   
   float fMaxS;
   float fMaxT;
   
   void createTexture();
   void releaseTexture();
   
public:
   GELabel(NSString* Text, NSString* FontName, float FontSize, UITextAlignment TextAligment,
           unsigned int Width, unsigned int Height);
   ~GELabel();
   
   void render();
   
   void setText(NSString* Text);
};


#endif
