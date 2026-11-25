# ray-snake
snake game made using raylib-quickstart I also used README file of mentioned repository to create this introduction

# Setup
Download the source code for the game then use one of the options bellow

## Supported Platforms
Quickstart supports the main 3 desktop platforms:
* Windows
* Linux
* MacOS

# VSCode Users (all platforms)
*Note* You must have a compiler toolchain installed in addition to vscode.

1. Download the quickstart
2. Rename the folder to your game name
3. Open the folder in VSCode
4. Run the build task ( CTRL+SHIFT+B or F5 )
5. You are good to go

# Windows Users
There are two compiler toolchains available for windows, MinGW-W64 (a free compiler using GCC), and Microsoft Visual Studio

## Using MinGW-W64
* Double click the `build-MinGW-W64.bat` file
* CD into the folder in your terminal
  * if you are using the W64devkit and have not added it to your system path environment variable, you must use the W64devkit.exe terminal, not CMD.exe
  * If you want to use cmd.exe or any other terminal, please make sure that gcc/mingw-W64 is in your path environment variable.
* run `make`
* You are good to go

### Note on MinGW-64 versions
Make sure you have a modern version of MinGW-W64 (not mingw).
The best place to get it is from the W64devkit from
https://github.com/skeeto/w64devkit/releases

or the version installed with the raylib installer

#### If you have installed raylib from the installer
Make sure you have added the path

`C:\raylib\w64devkit\bin`

To your path environment variable so that the compiler that came with raylib can be found.

DO NOT INSTALL ANOTHER MinGW-W64 from another source such as msys2, you don't need it.

## Microsoft Visual Studio 2026
* Run `build-VisualStudio2026.bat`
* double click the `.slnx` file that is generated
* develop your game
* you are good to go

# Linux Users
* CD into the build folder
* run `./premake5 gmake`
* CD back to the root
* run `make`
* you are good to go

# MacOS Users
* CD into the build folder
* run `./premake5.osx gmake`
* CD back to the root
* run `make`
* you are good to go

# Output files
The built code will be in the bin dir

# Working directories and the resources folder
The example uses a utility function from `path_utils.h` that will find the resources dir and set it as the current working directory. This is very useful when starting out. If you wish to manage your own working directory you can simply remove the call to the function and the header.

# Building for other OpenGL targets
If you need to build for a different OpenGL version than the default (OpenGL 3.3) you can specify an OpenGL version in your premake command line. Just modify the bat file or add the following to your command line

## For OpenGL 1.1
`--graphics=opengl11`

## For OpenGL 2.1
`--graphics=opengl21`

## For OpenGL 4.3
`--graphics=opengl43`

## For OpenGLES 2.0
`--graphics=opengles2`

## For OpenGLES 3.0
`--graphics=opengles3`

## For Software Rendering
`--graphics=software`

*Note*
Sofware rendering does not work with glfw, use Win32 or SDL platforms
`--backend=win32`
