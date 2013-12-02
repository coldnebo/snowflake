#include "cinder/Cinder.h"
#include "cinder/gl/Fbo.h"
#include "cinder/Rand.h"
#include "cinder/gl/GlslProg.h"

using namespace ci;
using namespace std;

// LK: this class generates the snowflake heightmap based (loosely) on the physics described by
// Kenneth Libbrecht here: http://www.its.caltech.edu/~atomic/publist/rpp5_4_R03.pdf
// basically, the task of this class is to update a greyscale heightmap that represents the 
// accretion of ice particles mimicking the formation of a snowflake based on the parameters
// of supersaturation and temperature (which are settable by the user).
class SnowflakeGenerator {
public:
  void setup(gl::GlslProg* edgeShader);
  void update(uint32_t elapsedFrames);
  void renderSceneToFbo(uint32_t elapsedFrames);
  void reset(uint32_t currentFrame);

  enum GenerativeModes { REAL_LIFE, FIRST_GEN, SECOND_GEN, THIRD_GEN, FOURTH_GEN, FIFTH_GEN };
  void setMode(GenerativeModes mode) { currentMode = mode; };

  // used Cinder BasicFBO sample as a guide for FBO usage.
  gl::Fbo				mFbo;
	static const int FBO_WIDTH = 512, FBO_HEIGHT = 512;
  static const uint32_t MAX_FRAMES_TO_RUN = 60*5;
private:
  void render();
  void renderEdge();
  void render2();
  void render3();
  void render4();
  void render5();

  //gl::Fbo				mFboBackBuffer;

  GenerativeModes currentMode;
  uint32_t startedAtFrame;
  float currentTime;  // normalized from 0.0 - 1.0
  bool started_growing,stop_growing;
  Rand rand;
  gl::GlslProg* mEdgeShader;
  float param1, param2, param3;
};