uniform vec4 lightPositionWS;
uniform vec4 viewPositionWS;
uniform mat4 fmModel;
uniform float filmScale;

uniform sampler2D snowflakeMap;

varying vec2 snowflakeTexCoords;
varying vec3 Normal;
varying vec3 ViewDirection;
varying vec3 LightDirection;
varying float fFilmDepth;

void main( void )
{
  //float filmScale = 0.5;
  float heightScale = 0.01;  // controls how thick the heightmap looks, this seems like a good value based on the gradient smoothness in test images.
  
  //fFilmDepth = filmScale;
  vec4 vertexPositionMS = gl_Vertex;  // model space
  vec3 normalMS         = gl_Normal;  // model space

  // displace the position according to the heightfield...
  snowflakeTexCoords = gl_MultiTexCoord0.st;
  vec4 p = texture2D(snowflakeMap, snowflakeTexCoords);
  // unit mesh
  float unit = 1.0/512.0;
  vec4 ps = texture2D(snowflakeMap, snowflakeTexCoords + vec2(unit,0.0));
  vec4 pt = texture2D(snowflakeMap, snowflakeTexCoords + vec2(0.0,unit));

  vertexPositionMS = vec4(normalMS * p.r * heightScale , 0.0) + vertexPositionMS;
  vec4 t = (vec4(normalMS * ps.r * heightScale , 0.0) + vec4(unit,0.0,0.0,0.0)) - vertexPositionMS;
  vec4 b = (vec4(normalMS * pt.r * heightScale , 0.0) + vec4(0.0,0.0,unit,0.0)) - vertexPositionMS;

  fFilmDepth = (p.r) * filmScale;
  normalMS = (normalize(cross(t,b))).xyz;
   
  // disregard the displacements for debugging.
  //fFilmDepth = filmScale;
  //vec4 vertexPositionMS = gl_Vertex;  // model space
  //vec3 normalMS         = gl_Normal;  // model space
   
  vec4 vertexPositionWS = fmModel * vertexPositionMS;
  vec4 normalWS         = fmModel * vec4(normalMS,0.0);

  // for thin film and phong specular, we need eye space vectors...
  ViewDirection  = -(viewPositionWS   -  vertexPositionWS).xyz;
  LightDirection = -(lightPositionWS  -  vertexPositionWS).xyz;
  Normal         = (normalWS).xyz;

  gl_Position = gl_ModelViewProjectionMatrix * vertexPositionMS;  // in screen space
}