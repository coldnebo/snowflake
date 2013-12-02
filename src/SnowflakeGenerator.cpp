#ifndef	SNOWFLAKE_GENERATOR_H
#define SNOWFLAKE_GENERATOR_H
#include "SnowflakeGenerator.h"
#include "cinder/Camera.h"
#include "Resources.h"
#include "cinder/CinderResources.h"
#include <gl/GLU.h>
#include "cinder/gl/gl.h"

void SnowflakeGenerator::setup(gl::GlslProg* edgeShader) {
  gl::Fbo::Format format;
  format.setColorInternalFormat(GL_RGBA);
	mFbo = gl::Fbo( FBO_WIDTH, FBO_HEIGHT, format );
  //mFboBackBuffer = gl::Fbo( FBO_WIDTH, FBO_HEIGHT, format );
  mEdgeShader = edgeShader;

  started_growing = false;
  stop_growing = true;
}

void SnowflakeGenerator::update(uint32_t elapsedFrames) {
  // stop growing after a certain point.
  if (elapsedFrames > startedAtFrame + MAX_FRAMES_TO_RUN)
    stop_growing = true;

  glPushMatrix();
    // render into our FBO
	  renderSceneToFbo(elapsedFrames);
  glPopMatrix();
}

void SnowflakeGenerator::reset(uint32_t currentFrame) {
  startedAtFrame = currentFrame;
  started_growing = true;
  stop_growing = false;
  rand.seed(currentFrame);
}

void SnowflakeGenerator::renderSceneToFbo(uint32_t elapsedFrames) {
  float currentFrameTime = (float)(elapsedFrames - startedAtFrame);
  currentTime = (float)currentFrameTime/(float)MAX_FRAMES_TO_RUN;


  // this will restore the old framebuffer binding when we leave this function
	// on non-OpenGL ES platforms, you can just call mFbo.unbindFramebuffer() at the end of the function
	// but this will restore the "screen" FBO on OpenGL ES, and does the right thing on both platforms
	//gl::SaveFramebufferBinding bindingSaver;
	
  // copy the current contents to the backbuffer so we can bind it to the shader for effects.
 // glBindFramebufferEXT( GL_READ_FRAMEBUFFER_EXT, mFbo.getId() );
	//glBindFramebufferEXT( GL_DRAW_FRAMEBUFFER_EXT, mFboBackBuffer.getId() );		
 // glBlitFramebufferEXT( mFbo.getBounds().getX1(), mFbo.getBounds().getY1(), mFbo.getBounds().getX2(), mFbo.getBounds().getY2(), 
 //                       mFboBackBuffer.getBounds().getX1(), mFboBackBuffer.getBounds().getY1(), mFboBackBuffer.getBounds().getX2(), mFboBackBuffer.getBounds().getY2(), 
 //                       GL_COLOR_BUFFER_BIT, GL_LINEAR );

  // blows up with invalid enum... hmmm?  was trying to guard against binding the texture for the renderEdge shader before it was complete.
  //GLenum status = (GLenum) glCheckFramebufferStatusEXT(mFboBackBuffer.getId());
  //if (status != GL_FRAMEBUFFER_COMPLETE_EXT) return;

  mFbo.bindFramebuffer();
  

  if ( !stop_growing ) {
 
    CameraOrtho cam;
    cam.setOrtho(-1.0,1.0,-1.0,1.0,-1.0,1.0);
    cam.setEyePoint(Vec3f(0.0,0.0,-1.0));
    cam.setWorldUp(Vec3f(0.0,0.0,1.0));
    cam.setCenterOfInterestPoint(Vec3f::zero());
    gl::setMatrices(cam);

    // setup the viewport to match the dimensions of the FBO
	  gl::setViewport( mFbo.getBounds() );

    // the buffer is conceptually only 255 levels "high"... that's ok, because focusing on snowflakes that are mostly 2D.
    float stepSize = 2.0/255.0; // one color level.
    float grey = stepSize;
    Color drawColor = ColorA(grey,grey,grey,grey);
    
    glDisable( GL_DEPTH_TEST );
    gl::enableAlphaBlending();
    glBlendFunc( GL_SRC_ALPHA, GL_ONE );
    gl::color(drawColor);
 
    // initial clear at the start of growing...
    if (started_growing) {
      gl::clear(Color::black(),true);
      //if (currentMode == SECOND_GEN) {
      //  gl::color(Color::white());
      //  gl::drawSolidCircle(Vec2f::zero(),0.1,20); // draw a seed for the edge shader
      //  gl::color(drawColor);
      //}
      // init params once at the start
      param1 = rand.nextFloat();
      param2 = rand.nextFloat();
      param3 = rand.nextFloat();
      started_growing = false;
    }

    switch (currentMode) {
    case FIRST_GEN:   render();       break;
    case SECOND_GEN:  render2();      break;
    case THIRD_GEN:   render3();      break;
    case FOURTH_GEN:  render4();      break;
    case FIFTH_GEN:   render5();      break;
    default: break;
    }

  }
  // for some reason, if I don't do this every frame, (even without the pairing) it will 
  // end the run on a weird light square instead of being completely transparent on the edges.
  // I've checked elsewhere for state not being undone, but who knows.  opengl "hell".
  // so just unset junk here to make it work.
  glEnable( GL_DEPTH_TEST );
  gl::disableAlphaBlending();
  glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

  mFbo.unbindFramebuffer();
}


// this attempts to use a Laplacian Convolution (edge detection)
// filter to find the edge of the snowflake (interface).  The shader then 
// "grows" the flake along it's edges, attenuating anything not on a hex lattice.
void SnowflakeGenerator::renderEdge() {
  int LOC = 4;

  //gl::drawSolidCircle(rand.nextVec2f()*0.5,0.1,20); 

  // I get a 1282 error here even though I'm using a seperate backbuffer 
  // (it's a bad idea to try to simultaneously read and write to the same buffer).
  // but not sure I'm doing this right.  Deadline looms large, will probably
  // shelve this part in favor of other simpler generators.  :P

  mEdgeShader->bind();
  /*mFboBackBuffer.bindTexture(LOC);
  mEdgeShader->uniform("srcColorMap", LOC);
  mEdgeShader->uniform("width", mFboBackBuffer.getWidth());
  mEdgeShader->uniform("height", mFboBackBuffer.getHeight());*/

  
  //mEdgeShader->uniform("dstColorMap", LOC+1);

  float EDGE = 2.0;
  float EDGE2 = EDGE/2.0;

  //gl::drawSolidRect( mFbo.getBounds() );


  glBegin( GL_QUADS );
      glTexCoord2d(0.0+EDGE2,0.0+EDGE2); glVertex2d(-1.0+EDGE,-1.0+EDGE);
      glTexCoord2d(1.0-EDGE2,0.0+EDGE2); glVertex2d(1.0-EDGE,-1.0+EDGE);
      glTexCoord2d(1.0-EDGE2,1.0-EDGE2); glVertex2d(1.0-EDGE,1.0-EDGE);
      glTexCoord2d(0.0+EDGE2,1.0-EDGE2); glVertex2d(-1.0+EDGE,1.0-EDGE);
  glEnd();

  mEdgeShader->unbind();
  //mFboBackBuffer.unbindTexture();
}


// this is called once the blend buffer has been correctly setup.
// should only be called by the above, never publically.
// first generation...
// "wavy flowers"
void SnowflakeGenerator::render() {

  float radius = currentTime * 0.7;

  Vec2f delta = Vec2f::one()*cos(radius*param1*17.0)*0.1;
  Vec2f growth = Vec2f(0,radius);
  Vec2f p1 = Vec2f(-radius/2, growth.y);
  Vec2f p2 = p1 + delta;
  Rectf rect = Rectf(p1,p2);

  float SYMMETRY = math<float>::ceil(math<float>::clamp(param3*50.0,20.0,50.0));
  for(float i=0; i<SYMMETRY; i++) {
    glPushMatrix();
      gl::rotate(sin(radius*param2*7.0)*30.0);
      gl::drawSolidRect(rect);
    glPopMatrix();
    gl::rotate(360.0/SYMMETRY);
  }
}


// second generation...
// "radial pop" -- modulated intervals.
void SnowflakeGenerator::render2() {

  float radius = currentTime * 0.7;

  Vec2f delta = Vec2f::one()*0.1 * param1;
  Vec2f p1 = Vec2f::one()*0.01;
  Vec2f p2 = p1 + delta + Vec2f(0,param3*0.2); 
  Rectf rect = Rectf(p1,p2);

  float SYMMETRY = math<float>::ceil(math<float>::clamp(param3*50.0,30.0,50.0));
  const float MODAMT = delta.x;
  float modDist = radius/MODAMT;
  float intPart;
  math<float>::modf(modDist, &intPart);

  for(float i=0; i<SYMMETRY; i++) {
    
    glPushMatrix();
      gl::rotate(sin(radius*param2*7.0)*5.0);
      gl::translate(Vec2f(0,intPart*MODAMT));
      gl::rotate(45.0);
      gl::drawSolidRect(rect);
    glPopMatrix();
    gl::rotate(360.0/SYMMETRY);
  }

}


// 'line-art'
void SnowflakeGenerator::render3() {
  float radius = currentTime * 0.9;

  float SYMMETRY = math<float>::ceil(math<float>::clamp(param3*50.0,6.0,18.0));

  Vec2f p1 = Vec2f(0,radius*param1);
  Vec2f p2 = Vec2f(0,radius*param2);
  p2.rotate(360.0/SYMMETRY);
  Vec2f h1 = p1/2.0;
  Vec2f h2 = p2/2.0;

  for(float i=0; i<SYMMETRY; i++) {
    glPushMatrix();
      gl::rotate(sin(radius*param3*7.0)*30.0);
      glBegin(GL_TRIANGLES);
		    gl::vertex(p1);
        gl::vertex(h2);
        gl::vertex(p1*0.85);
		    gl::vertex(p2);
        gl::vertex(h1);
        gl::vertex(p2*0.85);
	    glEnd();
    glPopMatrix();
    gl::rotate(360.0/SYMMETRY);
  }
}

// 'outside-in (sushi roll)'
void SnowflakeGenerator::render4() {
  float radius = currentTime * 0.9;
  const float MODAMT = param1;
  float modDist = radius/MODAMT;
  float intPart;
  math<float>::modf(modDist, &intPart);
  float length = 1 - intPart*MODAMT;

  float SYMMETRY = math<float>::ceil(math<float>::clamp(param3*50.0,6.0,18.0));

  Vec2f p1 = Vec2f(0,length*param1);
  Vec2f p2 = Vec2f(0,length*param2);
  p2.rotate(360.0/SYMMETRY);
  Vec2f h1 = p1/2.0;
  Vec2f h2 = p2/2.0;

  for(float i=0; i<SYMMETRY; i++) {
    glPushMatrix();
      gl::rotate(sin(radius*param3*7.0)*30.0);
      glBegin(GL_TRIANGLES);
		    gl::vertex(p1);
        gl::vertex(h2);
        gl::vertex(p1*0.85);
		    gl::vertex(p2);
        gl::vertex(h1);
        gl::vertex(p2*0.85);
	    glEnd();
    glPopMatrix();
    gl::rotate(360.0/SYMMETRY);
  }

}


// like 3 except that internal dots are linear combinations of p1,p2.
// i.e. using param'd barycentric coordinates.
void SnowflakeGenerator::render5() {
  float radius = currentTime * .8;
  float SYMMETRY = math<float>::ceil(math<float>::clamp(param3*50.0,6.0,18.0));

  Vec2f p1 = Vec2f(0,radius*param1);
  Vec2f p2 = Vec2f(0,radius*param2);
  p2.rotate(360.0/SYMMETRY);

  for(float i=0; i<SYMMETRY; i++) {
    glPushMatrix();
      gl::rotate(sin(radius*param3*7.0)*30.0);
      glBegin(GL_LINES);
		    gl::vertex(p1);
        gl::vertex(p1*param2 + p2*param1);
        gl::vertex(p1*param1 + p2*param3);
        gl::vertex(p1*param3 + p2*param1);
		    gl::vertex(p2);
	    glEnd();
    glPopMatrix();
    gl::rotate(360.0/SYMMETRY);
  }
}


#endif