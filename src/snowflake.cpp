/*
    Snowflake
    a snowflake simulator

    Final Project by Larry Kyrala for
    CS 175: Introduction to Computer Graphics
    Professor: Hanspeter Pfister
    Fall 2010
*/

#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/Camera.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/ImageIo.h"
#include "cinder/ObjLoader.h"
#include "cinder/gl/Vbo.h"
#include "cinder/gl/Texture.h"
#include "cinder/CinderMath.h"
#include "CubeMap.h"
#include "Resources.h"
#include "SnowflakeGenerator.h"
#include "cinder/Perlin.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class DeferredFloat {
public:
  float value, delta, min, max;
  DeferredFloat::DeferredFloat()
    : min(0.0),
      max(1.0),
      delta(0.01),
      value(0.5),
      is_incrementing(false),
      is_decrementing(false) {}

  void increment(bool is_key_down) {
    is_incrementing = is_key_down;
  }
  void decrement(bool is_key_down) {
    is_decrementing = is_key_down;
  }
  void update() {
    if (is_incrementing)
      value = math<float>::clamp(value+delta,min,max);
    else if (is_decrementing)
      value = math<float>::clamp(value-delta,min,max);
  }

private:
  bool is_incrementing;
  bool is_decrementing;
};


#define NUM_PARTICLES 1000


class snowflakeApp : public AppBasic {
public:

  CameraPersp		mCamera;
  gl::VboMesh		mVBOTorus, mVBOMesh;
  gl::GlslProg	mShader, mSkyboxShader, mEdgeShader;
  CubeMap*      mSkyboxMap;
  gl::Texture		mThinFilmMap;
  gl::Texture*  mRLSnowFlakes[5]; 
  Matrix44f     fmSFModel; 
 
  DeferredFloat fPolarization, fPolarizationSpectralBias, filmScale;

  SnowflakeGenerator snowgen;
  
  Vec3f         snow[NUM_PARTICLES];
  Rand          rand;

  int currentRLFlake;
  float         cameraDistance;
  Vec4f fvLightPosition, fvEyePosition;
  Vec4f viewPosition;

  // "globals"
  Vec2f         cameraRotation, snowflakeRotation, dragStart, snowflakeDragStart, dragOffset, snowflakeDragOffset;
  bool          is_rotating, is_snowflake_rotating, is_auto_camera;

  SnowflakeGenerator::GenerativeModes genMode;
  // constants
  
  const int CUBE_MAP_LOC, THINFILM_MAP_LOC, SNOWFLAKE_MAP_LOC;
  const float fSpecularPower;
  Vec4f fvSpecularColor;
  Perlin sPerlin;
  // init "consts"...
  snowflakeApp::snowflakeApp() 
    : CUBE_MAP_LOC(0),
      THINFILM_MAP_LOC(1),
      SNOWFLAKE_MAP_LOC(2),
      sPerlin(2),
      fSpecularPower(10.0f) {}

  void setup()
  {
    cameraRotation = snowflakeRotation = dragStart = snowflakeDragStart = dragOffset = snowflakeDragOffset = Vec2f::zero();
    is_auto_camera = is_rotating = is_snowflake_rotating = false;
    fPolarizationSpectralBias.value = 0.3;
    fvSpecularColor = Vec4f(0.7,0.7,0.7,1.0);
    currentRLFlake = 0;
    genMode = SnowflakeGenerator::GenerativeModes::REAL_LIFE;

    for (int i=0;i<NUM_PARTICLES;i++)
      snow[i] = rand.nextVec3f();

    cameraDistance = 3.0f;
    // handcrafted position to match the sunset in the texcube...
    fvLightPosition = Vec4f(-1.94836,0.179892,2.2741,1.0);
    // push it out a bit to avoid possible weirdness in shader.
    fvLightPosition *= 100.0;

    setWindowSize(1280/1.0,720/1.0);
    setFrameRate(30.0);
    
    glEnable( GL_DEPTH_TEST );
    glEnable( GL_TEXTURE_2D );

    mCamera.setPerspective(45.0, getWindowAspectRatio(), 1.0, 1000.0);
    mCamera.setEyePoint( Vec3f(0.0,0.0,cameraDistance) );
    mCamera.setCenterOfInterestPoint( Vec3f(0.0, 0.0, 0.0) );
    mCamera.setWorldUp( Vec3f(0.0,1.0,0.0) );

    setupTorus();
    setupMesh();
    setupShaders();

    setupSkybox();   
    snowgen.setup(&mEdgeShader);
    snowgen.reset(getElapsedFrames());
  }

  void setupShaders()
  {
    try {
      //mShader = gl::GlslProg( loadResource( BASIC_SHADER_VERT ), loadResource( BASIC_SHADER_FRAG ) );
      mShader       = gl::GlslProg( loadResource( SNOWFLAKE_SHADER_VERT ), loadResource( SNOWFLAKE_SHADER_FRAG ) );
      mSkyboxShader = gl::GlslProg( loadResource( SKYBOX_SHADER_VERT ),    loadResource( SKYBOX_SHADER_FRAG ) );
      mEdgeShader   = gl::GlslProg( loadResource( EDGE_SHADER_VERT ),      loadResource( EDGE_SHADER_FRAG ) );
    }
    catch( gl::GlslProgCompileExc &exc ) {
      std::cout << "Shader compile error: " << std::endl;
      std::cout << exc.what();
    }
    catch( ... ) {
      std::cout << "Unable to load shader" << std::endl;
    }

    try {
		  mThinFilmMap = gl::Texture( loadImage( loadResource( THINFILM_COLOR_RAMP ) ) );
      mThinFilmMap.setWrapS(GL_REPEAT);

      mRLSnowFlakes[0] = new gl::Texture( loadImage( loadResource( RL_SNOWFLAKE_1 ) ) );
      mRLSnowFlakes[1] = new gl::Texture( loadImage( loadResource( RL_SNOWFLAKE_2 ) ) );
      mRLSnowFlakes[2] = new gl::Texture( loadImage( loadResource( RL_SNOWFLAKE_3 ) ) );
      mRLSnowFlakes[3] = new gl::Texture( loadImage( loadResource( RL_SNOWFLAKE_4 ) ) );
      mRLSnowFlakes[4] = new gl::Texture( loadImage( loadResource( RL_SNOWFLAKE_5 ) ) );
      
	  }

	  catch( ... ) {
		  std::cout << "unable to load the texture file!" << std::endl;
	  }

  }

  void setupTorus()
  {
    TriMesh mMesh;
    ObjLoader loader( loadResource( TORUS_OBJ )->createStream() );
    loader.load( &mMesh );
    mVBOTorus = gl::VboMesh( mMesh );
  }


  // lazy, I adapted this from 
  // ...\cinder_0.8.2_vc2010\cinder_0.8.2_vc2010\samples\VBOSample\src\VBOSampleApp.cpp, line 33
  // but it's similar to what I did in HW4 to populate a mesh. (except the cinder framework makes it much
  // easier to allocate and setup the buffers).
  // Note: I'm not putting heights in because we're doing that with the shader (vertex displacement, not a geometry shader which requires newer GLSL).
  void setupMesh()
  {
    const int VERTICES_X = 512, VERTICES_Z = 512;
    // setup the parameters of the Vbo
	  int totalVertices = VERTICES_X * VERTICES_Z;
	  int totalQuads = ( VERTICES_X - 1 ) * ( VERTICES_Z - 1 );
	  gl::VboMesh::Layout layout;
	  layout.setStaticIndices();
    layout.setStaticPositions();
	  layout.setStaticTexCoords2d();
    layout.setStaticNormals();
	  mVBOMesh = gl::VboMesh( totalVertices, totalQuads * 4, layout, GL_QUADS );
	
	  // buffer our static data - the texcoords and the indices
	  vector<uint32_t> indices;
	  vector<Vec2f> texCoords;
    vector<Vec3f> positions;
    vector<Vec3f> normals;
	  for( int x = 0; x < VERTICES_X; ++x ) {
		  for( int z = 0; z < VERTICES_Z; ++z ) {
			  // create a quad for each vertex, except for along the bottom and right edges
			  if( ( x + 1 < VERTICES_X ) && ( z + 1 < VERTICES_Z ) ) {
				  indices.push_back( (x+0) * VERTICES_Z + (z+0) );
				  indices.push_back( (x+1) * VERTICES_Z + (z+0) );
				  indices.push_back( (x+1) * VERTICES_Z + (z+1) );
				  indices.push_back( (x+0) * VERTICES_Z + (z+1) );
			  }
			  // the texture coordinates are mapped to [0,1.0)
			  texCoords.push_back( Vec2f( x / (float)VERTICES_X, z / (float)VERTICES_Z ) );
        // the positions are mapped to a unit mesh...
        positions.push_back( Vec3f(x/(float)VERTICES_X, 0.0f, z/(float)VERTICES_Z) );
        normals.push_back( Vec3f(0.0,1.0f,0.0f) );
		  }
	  }
	
	  mVBOMesh.bufferIndices( indices );
	  mVBOMesh.bufferTexCoords2d( 0, texCoords );
    mVBOMesh.bufferPositions( positions );
    mVBOMesh.bufferNormals( normals );
  }

  void setupSkybox()
  {
    mSkyboxMap = new CubeMap( GLsizei(512), GLsizei(512),
      Surface8u( loadImage( loadResource( POSX ) ) ),
      Surface8u( loadImage( loadResource( POSY ) ) ),
      Surface8u( loadImage( loadResource( POSZ ) ) ),
      Surface8u( loadImage( loadResource( NEGX ) ) ),
      Surface8u( loadImage( loadResource( NEGY ) ) ),
      Surface8u( loadImage( loadResource( NEGZ ) ) ));
  }

  // input handlers
  
  void mouseDown( MouseEvent event ) { 
    if ( event.isLeft() ) {
      is_rotating = true;           dragStart          = getMousePos() - dragOffset; is_auto_camera = false;
    } else if ( event.isRight() ) {
      is_snowflake_rotating = true; snowflakeDragStart = getMousePos() - snowflakeDragOffset; is_auto_camera = false;
    }
  }
  void mouseUp( MouseEvent event ) { 
    if ( event.isLeft() ) {
      is_rotating = false;           dragOffset          = (getMousePos() - dragStart); 
    } else if ( event.isRight() ) {
      is_snowflake_rotating = false; snowflakeDragOffset = (getMousePos() - snowflakeDragStart); 
    }
  }
  void mouseWheel( MouseEvent event ) { cameraDistance += event.getWheelIncrement() * -0.2f; }
  
  void keyUp( KeyEvent event ) {
    switch( event.getChar() ) {
    case KeyEvent::KEY_ESCAPE:       quit();  break;
    case KeyEvent::KEY_r:            snowgen.reset(getElapsedFrames());  break;
    case KeyEvent::KEY_a:            is_auto_camera = !is_auto_camera;  break;
    case KeyEvent::KEY_0:            currentRLFlake++; if (currentRLFlake>4) currentRLFlake = 0;  genMode = SnowflakeGenerator::GenerativeModes::REAL_LIFE;  break;
    case KeyEvent::KEY_1:            genMode = SnowflakeGenerator::GenerativeModes::FIRST_GEN;   snowgen.reset(getElapsedFrames()); break;
    case KeyEvent::KEY_2:            genMode = SnowflakeGenerator::GenerativeModes::SECOND_GEN;  snowgen.reset(getElapsedFrames()); break;
    case KeyEvent::KEY_3:            genMode = SnowflakeGenerator::GenerativeModes::THIRD_GEN;   snowgen.reset(getElapsedFrames()); break;
    case KeyEvent::KEY_4:            genMode = SnowflakeGenerator::GenerativeModes::FOURTH_GEN;  snowgen.reset(getElapsedFrames()); break;
    case KeyEvent::KEY_5:            genMode = SnowflakeGenerator::GenerativeModes::FIFTH_GEN;   snowgen.reset(getElapsedFrames()); break;
    // continously varying keys:
    case KeyEvent::KEY_LEFTBRACKET:  fPolarization.decrement(false); break;
    case KeyEvent::KEY_RIGHTBRACKET: fPolarization.increment(false); break;
    case KeyEvent::KEY_COMMA:        fPolarizationSpectralBias.decrement(false); break;
    case KeyEvent::KEY_PERIOD:       fPolarizationSpectralBias.increment(false); break;
    case KeyEvent::KEY_MINUS:        filmScale.decrement(false); break;
    case KeyEvent::KEY_EQUALS:       filmScale.increment(false); break;
    }
  }

  void keyDown( KeyEvent event ) {
    switch( event.getChar() ) {
    // continuously varying keys:
    case KeyEvent::KEY_LEFTBRACKET:  fPolarization.decrement(true); break;
    case KeyEvent::KEY_RIGHTBRACKET: fPolarization.increment(true); break;
    case KeyEvent::KEY_COMMA:        fPolarizationSpectralBias.decrement(true); break;
    case KeyEvent::KEY_PERIOD:       fPolarizationSpectralBias.increment(true); break;
    case KeyEvent::KEY_MINUS:        filmScale.decrement(true); break;
    case KeyEvent::KEY_EQUALS:       filmScale.increment(true); break;
    }
  }


  void resize(int w, int h) { 
	  // let OpenGL know we have a new camera
	  gl::setMatrices( mCamera );
  }

  void update() 
  {
    snowgen.update(getElapsedFrames());
    snowgen.setMode(genMode);
    gl::setViewport( getWindowBounds() );
    mCamera.setPerspective( 45.0, getWindowAspectRatio(), 0.1, 1000.0 ); // needed after resize ends for some reason...  bug?
    rotateCameraFrame(cameraRotation);
    rotateSnowflakeFrame(snowflakeRotation);
    gl::setMatrices( mCamera );
    fPolarization.update();
    fPolarizationSpectralBias.update();
    filmScale.update();
    updateSnow();
    checkGLErrors();
  }

  
  void updateSnow() {
    Vec3f DOWN_FORCE = Vec3f(0,-0.001,0);

    for(int i=0;i<NUM_PARTICLES;i++) {
      snow[i] += DOWN_FORCE;

      Vec3f noise = sPerlin.dfBm( snow[i] * 0.01f + Vec3f( i/(float)NUM_PARTICLES, 0, i/(float)NUM_PARTICLES ) );
	    Vec3f perlin = noise.normalized() * 0.0005f;
      snow[i] += perlin;

      if (snow[i].y < -1.0)
        snow[i].y = 1.0;
      if (snow[i].x < -1.0) snow[i].x =  1.0;
      if (snow[i].x > 1.0)  snow[i].x = -1.0;
      if (snow[i].z < -1.0) snow[i].z =  1.0;
      if (snow[i].z > 1.0)  snow[i].z = -1.0;
      
    }
  }


  // LK: this is an orbiting camera, which uses screen-space to rotate.
  // think of a sheet of paper with a ball on top of it.  sliding the paper up down/left right rotates the ball.
  // similarly, mouse drags in screen space will rotate the camera in an orbit around the object.
  void rotateCameraFrame(Vec2f& rotation)
  {
    const float SCALE_FACTOR = 5.0;
    if (is_rotating) {
      Vec2f offset = getMousePos() - dragStart;
      rotation.x = (float)offset.x/(float)getWindowWidth();
      rotation.y = -((float)offset.y/(float)getWindowHeight());  // invert y-axis
    } else if (is_auto_camera) {
      float el = (float)getElapsedFrames()/2000.0;
      Vec2f offset = Vec2f(-sin(el)*(float)getWindowWidth()*1.5,sin(el)*30);
      dragOffset = offset;
      cameraDistance = cos(el) + 1.5;
      rotation.x = (float)offset.x/(float)getWindowWidth();
      rotation.y = -((float)offset.y/(float)getWindowHeight());  // invert y-axis
    }

    Vec3f p = Vec3f(0.0f,0.0f,cameraDistance);
    p.rotateX(rotation.y*SCALE_FACTOR);  
    p.rotateY(rotation.x*SCALE_FACTOR);  
    //console() << "camera position is " << p << endl;
    mCamera.setEyePoint(p);
    mCamera.setCenterOfInterestPoint(Vec3f::zero());
  }

  // similar idea, but in this case, we're rotating the flake...
  void rotateSnowflakeFrame(Vec2f& rotation)
  {
    const float SCALE_FACTOR = 3.0;
    if (is_snowflake_rotating) {
      Vec2f offset = getMousePos() - snowflakeDragStart;
      rotation.x = (float)offset.x/(float)getWindowWidth();
      rotation.y = ((float)offset.y/(float)getWindowHeight());
    } else if (is_auto_camera) {
      float el = (float)getElapsedFrames()/2000.0;
      Vec2f offset = Vec2f(-sin(el*60)*30.0,cos(el*60)*30.0);
      snowflakeDragOffset = offset;
      cameraDistance = cos(el) + 1.5;
      rotation.x = (float)offset.x/(float)getWindowWidth();
      rotation.y = -((float)offset.y/(float)getWindowHeight());  // invert y-axis
    }     

    fmSFModel.setToIdentity();
    fmSFModel.rotate( Vec3f( 0.0, rotation.x * SCALE_FACTOR, 0.0 ) );
    fmSFModel.rotate( Vec3f( rotation.y * SCALE_FACTOR, 0.0, 0.0 ) );
  }




  void draw()
  {
    gl::clear();
    drawSkybox();
    drawSnow();
    //drawTorus();
    drawMesh();
  }

  //void drawTorus() 
  //{
  //  glCullFace(GL_BACK);
  //  glPushMatrix();
  //    // scale and position the model in the world...
  //    Matrix44f fmModel; fmModel.setToIdentity();
  //    fmModel.scale( Vec3f( 0.0125, 0.0125, 0.0125 ) );  // scale to unit torus.
  //    fmModel.rotate(  Vec3f( M_PI/2, 0.0, 0.0 ) );
 
  //    // bind textures and shader 
  //    mThinFilmMap.bind(THINFILM_MAP_LOC);
  //    mSnowflakeTexture.bind(SNOWFLAKE_MAP_LOC);
  //    mSkyboxMap->bindMulti(CUBE_MAP_LOC); 
  //    mShader.bind();
  //    
  //    // set uniforms...
  //    mShader.uniform( "fringeMap", THINFILM_MAP_LOC );
  //    mShader.uniform( "snowflakeMap", SNOWFLAKE_MAP_LOC );
  //    mShader.uniform( "enviroment", CUBE_MAP_LOC );
  //    //    specular settings...
  //    mShader.uniform( "fvSpecular", fvSpecularColor );
  //    mShader.uniform( "fSpecularPower", fSpecularPower );
 
  //    //    and inputs in world space...
  //    mShader.uniform( "lightPositionWS", fvLightPosition );
  //    viewPosition = Vec4f(mCamera.getEyePoint(),1.0);
  //    mShader.uniform( "viewPositionWS", viewPosition );
  //    mShader.uniform( "fmModel", fmModel );

  //    // sync the gl state by multiplying the model transformations.
  //    gl::multModelView(fmModel);
  //    //gl::drawSphere(Vec3f(0.0,0.0,0.0),1.0,50);  // easier to debug than the torus... don't scale!
  //    gl::draw( mVBOTorus );

  //    mShader.unbind();
  //    mSkyboxMap->unbind();
  //    mSnowflakeTexture.unbind(SNOWFLAKE_MAP_LOC);
  //    mThinFilmMap.unbind(THINFILM_MAP_LOC);
  //    
  //  glPopMatrix();
  //  checkGLErrors();
  //}

  void drawMesh() 
  {
    glEnable(GL_BLEND);
    glDisable(GL_CULL_FACE);
    glPushMatrix();
      // scale and position the model in the world...
      Matrix44f fmModel; fmModel.setToIdentity();
      fmModel *= fmSFModel;  // rotate the model according to the mouse.
      fmModel.translate( Vec3f(-0.5f,0.5f, 0.0f) );  // center it
      fmModel.rotate(  Vec3f( M_PI/2, 0.0, 0.0 ) );
 
      // bind textures and shader 
      mThinFilmMap.bind(THINFILM_MAP_LOC);
      // debug texture to test shader...

      int activeFlake = currentRLFlake;
      if (genMode == SnowflakeGenerator::GenerativeModes::REAL_LIFE) {
        // use a real texture;
        mRLSnowFlakes[activeFlake]->bind(SNOWFLAKE_MAP_LOC); 
      } else {
        // bind snowgen fbo 
        snowgen.mFbo.bindTexture(SNOWFLAKE_MAP_LOC);
      }
      mSkyboxMap->bindMulti(CUBE_MAP_LOC); 
      mShader.bind();
      
      // set uniforms...
      mShader.uniform( "fringeMap", THINFILM_MAP_LOC );
      mShader.uniform( "snowflakeMap", SNOWFLAKE_MAP_LOC );
      mShader.uniform( "enviroment", CUBE_MAP_LOC );
      //    specular settings...
      mShader.uniform( "fvSpecular", fvSpecularColor );
      mShader.uniform( "fSpecularPower", fSpecularPower );
 
      //    and inputs in world space...
      mShader.uniform( "lightPositionWS", fvLightPosition );
      viewPosition = Vec4f(mCamera.getEyePoint(),1.0);
      mShader.uniform( "viewPositionWS", viewPosition );
      mShader.uniform( "fmModel", fmModel );
      mShader.uniform( "fCameraDistance", cameraDistance );
      mShader.uniform( "fPolarization", fPolarization.value );
      mShader.uniform( "fPolarizationSpectralBias", fPolarizationSpectralBias.value );
      mShader.uniform( "filmScale", filmScale.value );

      //drawSnow();

      // sync the gl state by multiplying the model transformations.
      gl::multModelView(fmModel);
      //gl::drawSphere(Vec3f(0.0,0.0,0.0),1.0,50);  // easier to debug than the torus... don't scale!
      gl::draw( mVBOMesh );

      

      mShader.unbind();
      mSkyboxMap->unbind();

      if (genMode == SnowflakeGenerator::GenerativeModes::REAL_LIFE) {
        // use a real texture;
        mRLSnowFlakes[activeFlake]->unbind(SNOWFLAKE_MAP_LOC); 
      } else {
        // bind snowgen fbo 
        snowgen.mFbo.unbindTexture();
      }
      mThinFilmMap.unbind(THINFILM_MAP_LOC);
      
    glPopMatrix();
    checkGLErrors();
  }

  void drawSnow() {
    gl::color(Color::white()*0.8);
    gl::disable(GL_TEXTURE_2D);
    gl::enable(GL_POINT_SMOOTH);
    float att[3] = { 0.0, 1.0, 1.0 };
    glPointSize(20.0);
    glPointParameterf(GL_POINT_SIZE_MIN, 1.0f);
    glPointParameterf(GL_POINT_SIZE_MAX, 20.0f);
    glPointParameterfv(GL_POINT_DISTANCE_ATTENUATION, att);
    glBegin(GL_POINTS);

    for(int i=0;i<NUM_PARTICLES;i++) {
      Vec3f p1 = snow[i]*10.0f;
      Vec3f p2 = p1+rand.nextVec3f()*0.05;
      Vec3f p3 = p1+rand.nextVec3f()*0.05;
      gl::vertex(p1);
      //gl::vertex(p2);
      //gl::vertex(p3);
    }
    glEnd();
  }
  
  void drawSkybox() 
  {
    glEnable( GL_MULTISAMPLE_ARB );
    mSkyboxMap->bindMulti(CUBE_MAP_LOC);

    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glPushMatrix();
      mSkyboxShader.bind();
      mSkyboxShader.uniform( "cubeMap", CUBE_MAP_LOC );
      mSkyboxShader.uniform( "fCameraDistance", cameraDistance );
      gl::scale( Vec3f( 20, 20, 20 ) );
      gl::drawCube( Vec3f::zero(), Vec3f::one() );
      mSkyboxShader.unbind();
    glPopMatrix();
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    mSkyboxMap->unbind();
  }


  // Checks if there's an error inside OpenGL and if yes,
  // print the error the throw an exception.
  void checkGLErrors() {
    const GLenum errCode = glGetError();      // check for errors
    if (errCode != GL_NO_ERROR) {
      std::cerr << "Error: " << glGetError() << std::endl;
      throw runtime_error((const char*)"GLERROR: "+glGetError());
    }
  }

};

CINDER_APP_BASIC( snowflakeApp, RendererGl )
