
//////////////////////////////////////////////////////////////////
//
//  Arturo Cepeda PŽrez
//  iOS Game Engine
//
//  Rendering Engine (OpenGL ES)
//
//  --- GERenderingShaders.mm ---
//
//////////////////////////////////////////////////////////////////


#include "GERenderingShaders.h"

//
//  GEShader
//
GEShader::~GEShader()
{
   glDeleteShader(iID);
}

void GEShader::load(NSString* sFilename, NSString* sExt)
{
   // read source file   
   NSString* sShaderPath = [[NSBundle mainBundle] pathForResource:sFilename ofType:sExt];
   const GLchar* sShaderSource = [[NSString stringWithContentsOfFile:sShaderPath encoding:NSUTF8StringEncoding error:nil] UTF8String];
   glShaderSource(iID, 1, &sShaderSource, NULL);
   
   // compile shader
   glCompileShader(iID);
   glGetShaderiv(iID, GL_COMPILE_STATUS, &iStatus);
   
   if(!check())
   {
      // get info log length
      GLint iLogLength;
      glGetShaderiv(iID, GL_INFO_LOG_LENGTH, &iLogLength);
      
      // get info log contents
      GLchar* sLog = new GLchar[iLogLength];            
      glGetShaderInfoLog(iID, iLogLength, NULL, sLog);
      
      // show info log
      NSLog(@"Shader '%@.%@': compiling error\n%s", sFilename, sExt, sLog);
      
      delete[] sLog;      
      exit(1);
   }
}

GLuint GEShader::getID()
{
   return iID;
}

bool GEShader::check()
{
   return (iStatus != 0);
}


//
//  GEVertexShader
//
GEVertexShader::GEVertexShader(NSString* Filename)
{
   // get shader ID
	iID = glCreateShader(GL_VERTEX_SHADER);
   iStatus = 0;
   
   // load shader
   load(Filename, @"vsh");
}


//
//  GEFragmentShader
//
GEFragmentShader::GEFragmentShader(NSString* Filename)
{
   // get shader ID
   iID = glCreateShader(GL_FRAGMENT_SHADER);
   iStatus = 0;
   
   // load shader
   load(Filename, @"fsh");
}
