
##OVERVIEW:

Please see the main project website at http://coldnebo.github.io/snowflake

##REQUIREMENTS:

* Windows 7 (32-bit).
* OpenGL capable card; minumum GLSL 1.20.  
* Developed on Windows 7 64-bit / EVGA NVIDIA GTX 260 (896MB) running 
on an Intel Core i7 920 @ 2.67 GHz with 6GB RAM.

##HOW TO INSTALL AND RUN:

Simply download [snowflake.exe](http://coldnebo.github.io/snowflake/snowflake.exe) and run it.

You can also build the executable yourself from source by following the instructions below.

##HOW TO COMPILE:

You will need the Cinder libraries and headers in order to compile this
project.  

Create the following directory structure:

    kyrala_l/
      cinder_dist/
      
Then:

    $ cd kyrala_l
    $ git clone https://github.com/coldnebo/snowflake

Alternatively you can download and extract the [snowflake-project.zip](http://coldnebo.github.io/snowflake/snowflake-project.zip) if you don't want to use git.  Note: this zip was built at the time of the project submission and won't contain later modifications, so you should consider git.

Now you should have:

    kyrala_l/
      cinder_dist/
      snowflake/

Then, please go to http://libcinder.org/
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


##USERS GUIDE:

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






