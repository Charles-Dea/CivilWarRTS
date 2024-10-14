## OS Support
The game will run on both Windows and Linux. There are also instructions for compiling the game from source on Linux (including cross-compiling a Windows binary). This is not supported on Windows.

Additionally, Linux users will have to ensure they have he following libraries:

OpenGL

GLEW

GLFW

Enet

OpenAL

Alut

These should be in the repos of most distros.
## Running the Game
Running the game is as simple as running the appropriate file; bin/CivilWarRTS on Linux, and bin\CivilWarRTS.exe on Windows. If running from the command line, you must cd into bin.
## Compiling
The game can only be compiled on Linux using g++ and mingw++.
To compile a Linux binary run:

`g++ -O3 -o bin/CivilWarRTS -lGL -lGLEW -lglflw -lenet -lopenal -lalut src/*.cpp`

To cross-compile a Windows binary run:

`x86_64-w64-mingw32-g++ -static -static-libgcc -static-libstdc++ -o CivilWarRTS.exe src/*.cpp winlibs/*`

## Contacting Me
I haven't updated the game in while, in fact I started doing a full re-write of it, but if you have a question or issue you can email me at charliedea@proton.me, and I'll try to respond as soon as possible.
