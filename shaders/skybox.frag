// from HW3 
//#version 120

uniform samplerCube cubeMap;
uniform float fCameraDistance;

varying vec3 fvVaryingTexCoords;

// something I thought of to add contrast on the zoom...
vec4 focus(float dist, vec4 color) {
  const float bias = 0.3;
  return mix((1.0+bias)*color,(1.0-bias)*color,clamp(1.0,0.0,smoothstep(3.0,1.0,dist)));
}

void main( void )
{  
   gl_FragColor = focus(fCameraDistance,texture( cubeMap, fvVaryingTexCoords ));
}