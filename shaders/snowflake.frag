uniform vec4  fvSpecular;
uniform float fSpecularPower;
uniform float fCameraDistance;
uniform float fPolarization;
uniform float fPolarizationSpectralBias;

uniform sampler2D   fringeMap;
uniform samplerCube environment;
uniform sampler2D   snowflakeMap;

varying vec2 fringeCoords;
varying vec2 snowflakeTexCoords;

varying vec3 Normal;
varying vec3 ViewDirection;
varying vec3 LightDirection;

varying float fFilmDepth;


// LK: returns the refracted vector given normalized I, N vectors, and eta...  
// from snell's law: http://en.wikipedia.org/wiki/Snell's_law
vec3 refract_function(vec3 i, vec3 n, float eta) {
  float fNDotNL = dot(n,-i);
  float cosTheta1 = fNDotNL;
  float cosTheta2 = sqrt(1.0 - pow(eta,2.0) * (1.0 - pow(cosTheta1,2.0)));
  
  vec3 vreflect = i + (2.0*cosTheta1)*n;
  vec3 vrefract;

  if (fNDotNL > 0.0)
    vrefract = eta*i + (eta*cosTheta1 - cosTheta2)*n;
  else
    vrefract = eta*i + (eta*cosTheta1 + cosTheta2)*n;

  return vrefract;
}

// adapted from nvidia thinfilm HLSL shader example
// http://developer.download.nvidia.com/SDK/9.5/Samples/samples.html#glsl_thinfilm
// also see http://en.wikipedia.org/wiki/Thin-film_interference
float calc_view_depth(float fNDotV, float Thickness) {
  return Thickness/fNDotV;
}

// something I thought of to add contrast on the zoom...
vec4 focus(float dist, vec4 color) {
  const float bias = 0.3;
  return mix((1.0+bias)*color,(1.0-bias)*color,clamp(1.0,0.0,smoothstep(3.0,1.0,dist)));
}

void main( void )
{
  vec3  fvNormal         = normalize( Normal );
  vec3  fvViewDirection  = normalize( ViewDirection );
  vec3  fvLightDirection = normalize( LightDirection );

  // simulate a thinfilm effect using the fringeMap and specified thickness.
  // since we are using two-sided surfaces, let this part be abs...
  float fNDotV           = abs(dot( fvNormal, -fvViewDirection ));
  float u                = calc_view_depth( fNDotV, fFilmDepth );
  vec4 fringeColor	     = texture2D(fringeMap,vec2(u + fPolarizationSpectralBias,0.5));
  fringeColor            = mix(1.0,fringeColor,fPolarization);
   // the thinfilm effect modulates the phong specular component...
  float fNDotL           = dot( fvNormal, fvLightDirection );
  vec3  fvReflection     = normalize( ( ( 2.0 * fvNormal ) * fNDotL ) - fvLightDirection );
  float fRDotV           = max( 0.0, dot( fvReflection, fvViewDirection ) );
  vec4 fvTotalSpecular   = fringeColor * fvSpecular * ( pow( fRDotV, fSpecularPower ) );
  // hardcode eta for the snowflake shader at the value for ice...
  float etaRatio = 0.96 - 0.20*smoothstep(0.01,1.0,fFilmDepth); //0.95; //0.76;   // range between 0.76, ice and 0.96, close to air.
  // refraction stuff...
  vec3 fvRefraction   = refract_function(fvViewDirection, fvNormal, etaRatio);
  vec4 refractColor   = textureCube(environment, fvRefraction);
  vec4 noRefractColor = focus(fCameraDistance, textureCube(environment, fvViewDirection));

  vec4 snowflakeColor = texture2D(snowflakeMap, snowflakeTexCoords);
   //vec4 finalColor = mix(refractColor,phongColor,refractPhongMix);
   //vec4 finalColor = mix(refractColor,fvTotalSpecular,1.0);
  vec4 finalColor = noRefractColor;
  const float blendGap = 20.0/255.0;
  if (snowflakeColor.r == 0)
    discard;
  if (snowflakeColor.r > 0 && snowflakeColor.r < blendGap) {
    // do a mix to cut down on aliasing...
    finalColor = mix(noRefractColor,refractColor+fvTotalSpecular,smoothstep(0.0,blendGap,snowflakeColor.r));
  } 
  if (snowflakeColor.r > blendGap) {
    finalColor = refractColor+fvTotalSpecular;
  }



  gl_FragColor = finalColor;
}
