
----------------------------------------------------------------------
    __  __
   /_/  \_\     Snowflake
    _\/\/_      a snowflake simulator
 /\_\_\/_/_/\
 \/ /_/\_\ \/   Final Project by Larry Kyrala for
   __/\/\__     CS 175: Introduction to Computer Graphics
   \_\  /_/     Professor: Hanspeter Pfister
                Harvard University
                Fall 2010
                
----------------------------------------------------------------------


======================================================================
FILES:

kyrala_l/
|-- README.txt                          // this readme
|-- report.html                         // final project report
|-- webpage.html                        // project web page
|-- snowflake.exe                       // pre-compiled win32 executable
|
|-- site                                // web page assets
|   |-- concept-sketches.png
|   |-- flakes.jpg
|   |-- icon-app.png
|   |-- icon-folder.png
|   |-- icon-html.png
|   |-- icon-text.png
|   |-- lamppost_0006.jpg
|   |-- lamppost_0006.obj
|   |-- snow-halo.jpg
|   |-- style.css
|   |-- tf1.png                         // screenshots
|   |-- tf2.png
|   |-- tf3.png
|   |-- tf4.png
|   |-- tf5.png
|   |-- tf6.png
|   `-- tf7.png
|
|-- snowflake                           // VC project folder  
|   |-- include
|   |   |-- CubeMap.h                   // from Cinder samples/cubeMapping
|   |   |-- Resources.h                 // LK
|   |   `-- SnowflakeGenerator.h        // LK
|   |-- src
|   |   |-- CubeMap.cpp                 // from Cinder samples/cubeMapping
|   |   |-- snowflake.cpp               // LK
|   |   `-- SnowflakeGenerator.cpp      // LK
|   |-- resources
|   |   |-- ColorRamp01.png             // color ramp used for thinfilm
|   |   |-- negx.jpg                    // skybox textures
|   |   |-- negy.jpg
|   |   |-- negz.jpg
|   |   |-- posx.jpg
|   |   |-- posy.jpg
|   |   |-- posz.jpg
|   |   |-- rl_snowflake_1.png          // real life snowflake textures
|   |   |-- rl_snowflake_2.png
|   |   |-- rl_snowflake_3.png
|   |   |-- rl_snowflake_4.png
|   |   |-- rl_snowflake_5.png
|   |   |-- texture-test2.png           // test texture, not used.
|   |   |-- texture-test.png            // test texture, not used.
|   |   `-- Torus2.obj                  // test object, not used.
|   |-- shaders
|   |   |-- basicFragmentShader.frag    // test shader, not used.
|   |   |-- basicVertexShader.vert
|   |   |-- edge.frag                   // laplacian convolution; disabled.
|   |   |-- edge.vert
|   |   |-- skybox.frag                 // skybox for cubemap.
|   |   |-- skybox.vert
|   |   |-- snowflake.frag              // refraction & thinfilm shader
|   |   `-- snowflake.vert
|   `-- vc10
|       |-- cinder_app_icon.ico
|       |-- Resources.rc
|       |-- snowflake.sln
|       |-- snowflake.suo
|       |-- snowflake.vcxproj
|       |-- snowflake.vcxproj.filters
|       `-- snowflake.vcxproj.user
`-- snowflake-shader                   // RenderMonkey project (shader prototypes)
    |-- ColorRamp01.png
    |-- Error.txt
    |-- snowflake.rfx                  // rendermonkey project file
    |-- texture.png
    |-- texture-test.xcf
    |-- texture.tga
    `-- wf0118b057.jpg

======================================================================
REQUIREMENTS:

Windows 7 (32-bit).
OpenGL capable card; minumum GLSL 1.20.  
Developed on Windows 7 64-bit / EVGA NVIDIA GTX 260 (896MB) running 
on an Intel Core i7 920 @ 2.67 GHz with 6GB RAM.

======================================================================
HOW TO INSTALL AND RUN:

Simply unzip this archive into a folder and open the kyrala_l folder
and double click "snowflake.exe" to run it.

(After the class and when this has been posted, I may post this project to my
github account.  See https://github.com/coldnebo for updates.)

======================================================================
HOW TO COMPILE:

You will need the Cinder libraries and headers in order to compile this
project.  

If you are a TF, please see the cinder-dist.zip that I've uploaded to
the dropbox.  Download this and extract it to the kyrala_l folder so that you
see the following directory structure:

kyrala_l/
  cinder_dist/
  snowflake/
  
If you are seeing this file after the class, then please go to http://libcinder.org/
and download Cinder for Visual C++ 2010.  (This project was compiled against
Cinder version 0.8.2).  You will either a) need to extract and rename the 
folder to "cinder_dist", or b) change references of cinder_dist to "cinder_0.8.2_vc2010"
in the project settings.

Once you have this setup, you should be able to open the sln solution file in 
Visual Studio 2010.  Select "Debug" or "Release" and build the solution.  Since
all the resources are compiled into the solution you should simply be able to 
run it or debug it in place.

(If this is after the class: I haven't tried compiling it on Mac, however 
Cinder supports this... you might need to download the other package for Cinder instead.)


======================================================================
USERS GUIDE:

keys:
  ESC                 - quit
  0   zero            - cycle through 5 "real life" snowflakes
  
  1-5                 - various procedural generators. (described in html)
  
  1                   - 'flower' generator.
  2                   - 'mosaic' generator.
  3                   - 'lineart' generator.
  4                   - 'outside-in (sushi-roll) lineart' generator
  5                   - 'random barycentric triple' generator
  
  R                   - reset generator
  
  A                   - 'auto' camera; smooth rotation around snowflake
  
  []  square brackets - adjust polarization
  <>  angle brackets  - adjust spectrum bias for polarization
  -=  minus/equals    - adjust thickness of interface/refraction factor

mouse:
  LEFT MOUSE DRAG   - rotate the camera
  RIGHT MOUSE DRAG  - rotate the snowflake
  MOUSE WHEEL       - zoom in/out on snowflake


======================================================================
LICENSES:

Except where otherwise noted, this program is released under BSD 
license:

Copyright (c) 2010, Larry Kyrala
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions 
are met:

    * Redistributions of source code must retain the above copyright 
notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above 
copyright notice, this list of conditions and the following 
disclaimer in the documentation and/or other materials provided with 
the distribution.
    * Neither the name of Harvard University nor the names of its 
contributors may be used to endorse or promote products derived from 
this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR 
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, 
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS 
OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED 
AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY 
WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
POSSIBILITY OF SUCH DAMAGE.

----------------------------------------------------------------------
Cinder 0.8.2  
http://libcinder.org

Cinder is a peer-reviewed, free, open source C++ library for creative 
coding. Cinder supports Mac OS X, Windows and iOS-based devices. It 
requires XCode 3.1 or later on the Mac, and Visual C++ 2008 or 2010 
on Windows.

Cinder is released under the Modified BSD License.
(http://www.oss-watch.ac.uk/resources/modbsd.xml)

----------------------------------------------------------------------
Portions of the snowflake GLSL shader were adapted from the resources 
and HLSL code contained in the NVIDIA SDK 9.52 Code Sample "Thinfilm":
http://developer.download.nvidia.com/SDK/9.5/Samples/samples.html#glsl_thinfilm

"This code is released free of charge for use in derivative works, 
whether academic, commercial, or personal. (Full License)"
http://developer.download.nvidia.com/licenses/general_license.txt

----------------------------------------------------------------------
SnowCrystals.com
http://www.its.caltech.edu/~atomic/snowcrystals/

Some textures in this program are derived from copyrighted pictures
on "SnowCrystals.com" by Kenneth Libbrecht of Caltech:

"Personal Uses:  If you want to use anything on this website for some 
small personal use -- something that does not involve publication -- 
then go ahead.  This includes book reports, presentations, class 
projects, personal Christmas cards, etc.  Please include a reference 
to to SnowCrystals.com whenever possible."
(http://www.its.caltech.edu/~atomic/snowcrystals/copyright/copyright.htm)

----------------------------------------------------------------------
Cubemap Texture "Vindelälven" courtesy of Emil Persson "Humus".
http://www.humus.name/index.php?page=Textures&ID=72

Author

This is the work of Emil Persson, aka Humus.
http://www.humus.name
humus@comhem.se

Legal stuff

This work is free and may be used by anyone for any purpose
and may be distributed freely to anyone using any distribution
media or distribution method as long as this file is included.
Distribution without this file is allowed if it's distributed
with free non-commercial software; however, fair credit of the
original author is expected.
Any commercial distribution of this software requires the written
approval of Emil Persson.


----------------------------------------------------------------------
ascii art snowflake #4 from a collection by Ilmari Karonen
http://vyznev.net/ascii/flakes01.asc

"I'll hereby permit you or anyone else to use, modify, sell and/or 
redistribute any of my ASCII pictures, provided that they are correctly 
attributed to me in a reasonable manner and that any modified versions 
are properly identified as such."


