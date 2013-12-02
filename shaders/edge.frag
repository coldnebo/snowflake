#version 120
// from http://www.ozone3d.net/tutorials/image_filtering_p2.php
#define KERNEL_SIZE 9

// LK: this shader was going to be modified to not only provide edge detection, but "grow" the texture
// through successive iterations but only along hexagonal "ley-lines" bias.  i.e.. if the texture coords
// were approximate to one of the axes of symmetry for a hexagon.
// however, I'm not able to get the FBO's working properly for this kind of magic, so I'm 
// shelving this approach for now in favor of more vanilla operations on the existing FBO.
// maybe when this code is published, someone else in the community can help?

// Laplacian convolution kernel (edge detection)
// 0   1   0
// 1  -4   1
// 0   1   0

// modded according to http://www.opengl.org/discussion_boards/ubbthreads.php?ubb=showflat&Number=273148
const float kernel[KERNEL_SIZE] = float[]
    ( 0.0/16.0,   1.0/16.0,  0.0/16.0,
		  1.0/16.0,  -4.0/16.0,  1.0/16.0,
		  0.0/16.0,   1.0/16.0,  0.0/16.0 );

uniform sampler2D srcColorMap;
uniform float width;
uniform float height;


void main(void)
{
  float step_w = 1.0/width;
  float step_h = 1.0/height;
  vec2 offset[KERNEL_SIZE] = vec2[] 
    ( vec2(-step_w, -step_h), vec2(0.0, -step_h), vec2(step_w, -step_h), 
			vec2(-step_w, 0.0),     vec2(0.0, 0.0),     vec2(step_w, 0.0), 
			vec2(-step_w, step_h),  vec2(0.0, step_h),  vec2(step_w, step_h) );

  int i = 0;
  vec4 sum = vec4(0.0);

  for( i=0; i<KERNEL_SIZE; i++ )
  {
    vec4 tmp = texture2D(srcColorMap, gl_TexCoord[0].st + offset[i]);
    sum += tmp * kernel[i];
  }

   gl_FragColor = sum;
   //gl_FragData[0] = sum;
}
