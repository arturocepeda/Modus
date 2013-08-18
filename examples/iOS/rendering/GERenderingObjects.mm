
//////////////////////////////////////////////////////////////////
//
//  Arturo Cepeda PŽrez
//  iOS Game Engine
//
//  Rendering Engine (OpenGL ES)
//
//  --- GERenderingObjects.mm ---
//
//////////////////////////////////////////////////////////////////


#include "GERenderingObjects.h"


//
//  GERenderingObject
//
void GERenderingObject::move(float DX, float DY, float DZ)
{
   vPosition.X += DX;
   vPosition.Y += DY;
   vPosition.Z += DZ;
}

void GERenderingObject::move(const GEVector& Move)
{
   vPosition.X += Move.X;
   vPosition.Y += Move.Y;
   vPosition.Z += Move.Z;
}

void GERenderingObject::scale(float SX, float SY, float SZ)
{
   vScale.X *= SX;
   vScale.Y *= SY;
   vScale.Z *= SZ;
}

void GERenderingObject::scale(const GEVector& Scale)
{
   vScale.X *= Scale.X;
   vScale.Y *= Scale.Y;
   vScale.Z *= Scale.Z;
}

void GERenderingObject::rotate(float RX, float RY, float RZ)
{
   vRotation.X += RX;
   vRotation.Y += RY;
   vRotation.Z += RZ;
}

void GERenderingObject::rotate(const GEVector& Rotate)
{
   vRotation.X += Rotate.X;
   vRotation.Y += Rotate.Y;
   vRotation.Z += Rotate.Z;
}

void GERenderingObject::getPosition(GEVector* Position)
{
   Position->X = vPosition.X;
   Position->Y = vPosition.Y;
   Position->Z = vPosition.Z;
}

void GERenderingObject::getRotation(GEVector* Rotation)
{
   Rotation->X = vRotation.X;
   Rotation->Y = vRotation.Y;
   Rotation->Z = vRotation.Z;
}

void GERenderingObject::getScale(GEVector* Scale)
{
   Scale->X = vScale.X;
   Scale->Y = vScale.Y;
   Scale->Z = vScale.Z;
}

void GERenderingObject::setPosition(float X, float Y, float Z)
{
   vPosition.X = X;
   vPosition.Y = Y;
   vPosition.Z = Z;
}

void GERenderingObject::setPosition(const GEVector& Position)
{
   vPosition.X = Position.X;
   vPosition.Y = Position.Y;
   vPosition.Z = Position.Z;
}

void GERenderingObject::setScale(float X, float Y, float Z)
{
   vScale.X = X;
   vScale.Y = Y;
   vScale.Z = Z;
}

void GERenderingObject::setScale(const GEVector& Scale)
{
   vScale.X = Scale.X;
   vScale.Y = Scale.Y;
   vScale.Z = Scale.Z;
}

void GERenderingObject::setRotation(float X, float Y, float Z)
{
   vRotation.X = X;
   vRotation.Y = Y;
   vRotation.Z = Z;
}

void GERenderingObject::setRotation(const GEVector& Rotation)
{
   vRotation.X = Rotation.X;
   vRotation.Y = Rotation.Y;
   vRotation.Z = Rotation.Z;
}



//
//  GERenderingObjectVisible
//
void GERenderingObjectVisible::show()
{
   bVisible = true;
}

void GERenderingObjectVisible::hide()
{
   bVisible = false;
}

void GERenderingObjectVisible::getModelMatrix(GLKMatrix4* ModelMatrix)
{
   *ModelMatrix = GLKMatrix4MakeTranslation(vPosition.X, vPosition.Y, vPosition.Z);
   *ModelMatrix = GLKMatrix4RotateX(*ModelMatrix, vRotation.X);
   *ModelMatrix = GLKMatrix4RotateY(*ModelMatrix, vRotation.Y);
   *ModelMatrix = GLKMatrix4RotateZ(*ModelMatrix, vRotation.Z);
   *ModelMatrix = GLKMatrix4Scale(*ModelMatrix, vScale.X, vScale.Y, vScale.Z);
}

void GERenderingObjectVisible::getColor(GEColor* Color)
{
   Color->R = cColor.R;
   Color->G = cColor.G;
   Color->B = cColor.B;
}

float GERenderingObjectVisible::getOpacity()
{
   return fOpacity;
}

bool GERenderingObjectVisible::getVisible()
{
   return bVisible;
}

GLuint GERenderingObjectVisible::getTexture()
{
   return iTexture;
}

void GERenderingObjectVisible::setColor(float R, float G, float B)
{
   cColor.set(R, G, B);
}

void GERenderingObjectVisible::setColor(const GEColor& Color)
{
   cColor = Color;
}

void GERenderingObjectVisible::setOpacity(float Opacity)
{
   fOpacity = Opacity;
}

void GERenderingObjectVisible::setVisible(bool Visible)
{
   bVisible = Visible;
}

void GERenderingObjectVisible::setTexture(GLuint Texture)
{
   iTexture = Texture;
}



//
//  GESprite
//
GESprite::GESprite()
{
   iNumVertices = 4;
   
   fOpacity = 1.0f;
   bVisible = true;

   memset(&vPosition, 0, sizeof(GEVector));
   memset(&vRotation, 0, sizeof(GEVector));
   
   vScale.X = 1.0f;
   vScale.Y = 1.0f;
   vScale.Z = 1.0f;
   
   fVertex = new float[iNumVertices * 3];
   fTextureCoordinate = new float[iNumVertices * 2];
   fNormals = NULL;
   
   setCenter(0.0f, 0.0f, 0.0f);
   setTextureCoordinates(1.0f, 1.0f, 
                         1.0f, 0.0f, 
                         0.0f, 1.0f, 
                         0.0f, 0.0f);
   
   // generate vertex array
   glGenVertexArraysOES(1, &iVertexArray);
   glBindVertexArrayOES(iVertexArray);
   
   // generate vertex buffers
   glGenBuffers(2, iVertexBuffers);
   
   // fill vertex array buffers
   glBindBuffer(GL_ARRAY_BUFFER, iVertexBuffers[GEVertexAttributes.Position]);
   glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * iNumVertices, fVertex, GL_STATIC_DRAW);
   glEnableVertexAttribArray(GEVertexAttributes.Position);
   glVertexAttribPointer(GEVertexAttributes.Position, 3, GL_FLOAT, GL_FALSE, 0, 0);
   
   glBindBuffer(GL_ARRAY_BUFFER, iVertexBuffers[GEVertexAttributes.TextureCoord0]);
   glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * iNumVertices, fTextureCoordinate, GL_STATIC_DRAW);
   glEnableVertexAttribArray(GEVertexAttributes.TextureCoord0);
   glVertexAttribPointer(GEVertexAttributes.TextureCoord0, 2, GL_FLOAT, GL_FALSE, 0, 0);
   
   glDisableVertexAttribArray(GEVertexAttributes.Normal);
   
   // unbind buffers and vertex array object
   glBindBuffer(GL_ARRAY_BUFFER, 0);
   glBindVertexArrayOES(0);
}

GESprite::~GESprite()
{
   // release vertex array and vertex buffers
   glDeleteBuffers(GEVertexAttributes.Count, iVertexBuffers);
   glDeleteVertexArraysOES(1, &iVertexArray);
   
   // release vertex data
   delete[] fVertex;
   delete[] fTextureCoordinate;
}

void GESprite::render()
{
   if(!bVisible)
      return;
   
   // bind vertex array object
   glBindVertexArrayOES(iVertexArray);

   // draw
   glDrawArrays(GL_TRIANGLE_STRIP, 0, iNumVertices);
}

void GESprite::setCenter(float X, float Y, float Z)
{
   fVertex[0] = -1 + X; fVertex[ 1] = -1 + Y; fVertex[ 2] = Z;   
   fVertex[3] =  1 + X; fVertex[ 4] = -1 + Y; fVertex[ 5] = Z;   
   fVertex[6] = -1 + X; fVertex[ 7] =  1 + Y; fVertex[ 8] = Z;   
   fVertex[9] =  1 + X; fVertex[10] =  1 + Y; fVertex[11] = Z;
   
   // update vertex buffer data
   glBindVertexArrayOES(iVertexArray);   
   glBindBuffer(GL_ARRAY_BUFFER, iVertexBuffers[GEVertexAttributes.Position]);
   glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * 3 * iNumVertices, fVertex);   
}

void GESprite::setTextureCoordinates(float Ax, float Ay, float Bx, float By,
                                     float Cx, float Cy, float Dx, float Dy)
{
   fTextureCoordinate[0] = Ax; fTextureCoordinate[1] = Ay;
   fTextureCoordinate[2] = Bx; fTextureCoordinate[3] = By;
   fTextureCoordinate[4] = Cx; fTextureCoordinate[5] = Cy;
   fTextureCoordinate[6] = Dx; fTextureCoordinate[7] = Dy;
   
   // update vertex buffer data
   glBindVertexArrayOES(iVertexArray);   
   glBindBuffer(GL_ARRAY_BUFFER, iVertexBuffers[GEVertexAttributes.TextureCoord0]);
   glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * 2 * iNumVertices, fTextureCoordinate);   
}


//
//  GELabel
//
GELabel::GELabel(NSString* Text, NSString* FontName, float FontSize, UITextAlignment TextAligment,
                 unsigned int Width, unsigned int Height)
{
   iNumVertices = 4;
   
   bVisible = true;
   
   memset(&vPosition, 0, sizeof(GEVector));
   memset(&vRotation, 0, sizeof(GEVector));
   
   vScale.X = 1.0f;
   vScale.Y = 1.0f;
   vScale.Z = 1.0f;
   
   cColor.set(1.0f, 1.0f, 1.0f);
   fOpacity = 1.0f;
   
   fVertex = new float[iNumVertices * 3];
   fTextureCoordinate = new float[iNumVertices * 2];
   fNormals = NULL;
   
   sText = Text;
   sFont = FontName;
   fFontSize = FontSize;
   iWidth = Width;
   iHeight = Height;   
   tAligment = TextAligment;
   
   createTexture();
   
   float fAspectRatio = (float)iHeight / iWidth;      
   
	fTextureCoordinate[0] = 0.0f;  fTextureCoordinate[1] = fMaxT;
   fTextureCoordinate[2] = fMaxS; fTextureCoordinate[3] = fMaxT;
   fTextureCoordinate[4] = 0.0f;  fTextureCoordinate[5] = 0.0f;
   fTextureCoordinate[6] = fMaxS; fTextureCoordinate[7] = 0.0f;
   
   switch(tAligment)
   {  
         // reference point: top left
      case UITextAlignmentLeft:         
         fVertex[0] = 0.0f; fVertex[1] = -fAspectRatio; fVertex[2] = 0.0f;
         fVertex[3] = 1.0f; fVertex[4] = -fAspectRatio; fVertex[5] = 0.0f;
         fVertex[6] = 0.0f; fVertex[7] = 0.0f; fVertex[8] = 0.0f;
         fVertex[9] = 1.0f; fVertex[10] = 0.0f; fVertex[11] = 0.0f;
         break;
         
         // reference point: top center
      case UITextAlignmentCenter:         
         fVertex[0] = -0.5f; fVertex[1] = -fAspectRatio; fVertex[2] = 0.0f;
         fVertex[3] = 0.5f; fVertex[4] = -fAspectRatio; fVertex[5] = 0.0f;
         fVertex[6] = -0.5f; fVertex[7] = 0.0f; fVertex[8] = 0.0f;
         fVertex[9] = 0.5f; fVertex[10] = 0.0f; fVertex[11] = 0.0f;        
         break;
         
         // reference point: top right
      case UITextAlignmentRight:         
         fVertex[0] = -1.0f; fVertex[1] = -fAspectRatio; fVertex[2] = 0.0f;
         fVertex[3] = 0.0f; fVertex[4] = -fAspectRatio; fVertex[5] = 0.0f;
         fVertex[6] = -1.0f; fVertex[7] = 0.0f; fVertex[8] = 0.0f;
         fVertex[9] = 0.0f; fVertex[10] = 0.0f; fVertex[11] = 0.0f;         
         break;
   }
   
   // generate vertex array
   glGenVertexArraysOES(1, &iVertexArray);
   glBindVertexArrayOES(iVertexArray);
   
   // generate vertex buffers
   glGenBuffers(2, iVertexBuffers);
   
   // fill vertex array buffers
   glBindBuffer(GL_ARRAY_BUFFER, iVertexBuffers[GEVertexAttributes.Position]);
   glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * iNumVertices, fVertex, GL_STATIC_DRAW);
   glEnableVertexAttribArray(GEVertexAttributes.Position);
   glVertexAttribPointer(GEVertexAttributes.Position, 3, GL_FLOAT, GL_FALSE, 0, 0);
   
   glBindBuffer(GL_ARRAY_BUFFER, iVertexBuffers[GEVertexAttributes.TextureCoord0]);
   glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * iNumVertices, fTextureCoordinate, GL_STATIC_DRAW);
   glEnableVertexAttribArray(GEVertexAttributes.TextureCoord0);
   glVertexAttribPointer(GEVertexAttributes.TextureCoord0, 2, GL_FLOAT, GL_FALSE, 0, 0);
   
   glDisableVertexAttribArray(GEVertexAttributes.Normal);
   
   // unbind buffers and vertex array object
   glBindBuffer(GL_ARRAY_BUFFER, 0);
   glBindVertexArrayOES(0);
}

GELabel::~GELabel()
{
   // release vertex array and vertex buffers
   glDeleteBuffers(GEVertexAttributes.Count, iVertexBuffers);
   glDeleteVertexArraysOES(1, &iVertexArray);
      
   // release texture
   releaseTexture();

   // release vertex data
   delete[] fVertex;
   delete[] fTextureCoordinate;
}

void GELabel::createTexture()
{
   // set width and height to 2-power values
   int i;
   
	if((iWidth != 1) && (iWidth & (iWidth - 1))) 
   {
		i = 1;
      
		while(i < iWidth)
         i *= 2;
      
		iWidth = i;
	}
   	
	if((iHeight != 1) && (iHeight & (iHeight - 1))) 
   {
		i = 1;
      
		while(i < iHeight)
         i *= 2;
      
		iHeight = i;
	}
   
   // create bitmap context
	void* pData = malloc(iHeight * iWidth);
   
   CGColorSpaceRef cgColorSpace = CGColorSpaceCreateDeviceGray();
	CGContextRef cgContext = CGBitmapContextCreate(pData, iWidth, iHeight, 8, iWidth, cgColorSpace, kCGImageAlphaNone);
	CGColorSpaceRelease(cgColorSpace);
	
   // context settings
	CGContextSetGrayFillColor(cgContext, 1.0, 1.0);
	CGContextTranslateCTM(cgContext, 0.0, iHeight);
	CGContextScaleCTM(cgContext, 1.0, -1.0);
   
   // generate text
   CGSize cgSize = CGSizeMake(iWidth, iHeight);
   UIFont* uiFont = [UIFont fontWithName:sFont size:fFontSize];
   
	UIGraphicsPushContext(cgContext);
   [sText drawInRect:CGRectMake(0, 0, cgSize.width, cgSize.height) withFont:uiFont lineBreakMode:UILineBreakModeWordWrap alignment:tAligment];
	UIGraphicsPopContext();
   
   fMaxS = cgSize.width / iWidth;
   fMaxT = cgSize.height / iHeight;
	
   // generate texture
	GLint iSaveName;   
	glGenTextures(1, &iTexture);   
   glGetIntegerv(GL_TEXTURE_BINDING_2D, &iSaveName);    
   glBindTexture(GL_TEXTURE_2D, iTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, iWidth, iHeight, 0, GL_ALPHA, GL_UNSIGNED_BYTE, pData);   
	glBindTexture(GL_TEXTURE_2D, iSaveName);
	
   // release context and data
	CGContextRelease(cgContext);
	free(pData);
}

void GELabel::releaseTexture()
{
   glDeleteTextures(1, &iTexture);
   iTexture = 0;
}

void GELabel::render()
{
   if(!bVisible)
      return;
   
   // bind vertex array object
   glBindVertexArrayOES(iVertexArray);

   // draw
   glDrawArrays(GL_TRIANGLE_STRIP, 0, iNumVertices);
}

void GELabel::setText(NSString* Text)
{
   sText = Text;
   
   releaseTexture();
   createTexture();
}
