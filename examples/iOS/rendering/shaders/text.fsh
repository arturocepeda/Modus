
//////////////////////////////////////////////////////////////////
//
//  Arturo Cepeda Pérez
//  iOS Game Engine
//
//  Fragment shader (Text)
//
//  --- text.fsh ---
//
//////////////////////////////////////////////////////////////////


precision lowp float;

varying vec2 varTextCoord0;

uniform vec4 uObjectColor;
uniform sampler2D uTexture0;

void main()
{
   vec4 vColor = texture2D(uTexture0, varTextCoord0);
   
   float fAlpha = uObjectColor.w;
   vColor.w *= fAlpha;
   
   if(vColor.w > 0.0)
      vColor = vec4(uObjectColor.xyz, vColor.w);
   
   gl_FragColor = vColor;
}
