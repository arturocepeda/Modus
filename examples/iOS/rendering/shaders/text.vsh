
//////////////////////////////////////////////////////////////////
//
//  Arturo Cepeda Pérez
//  iOS Game Engine
//
//  Vertex shader (Text)
//
//  --- text.vsh ---
//
//////////////////////////////////////////////////////////////////


attribute vec4 aPosition;
attribute vec3 aNormal;
attribute vec2 aTextCoord0;

uniform mat4 uModelViewProjectionMatrix;

varying vec2 varTextCoord0;

void main()
{
   varTextCoord0 = aTextCoord0;
   
   gl_Position = uModelViewProjectionMatrix * aPosition;
}
