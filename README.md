# Chip8 Emulator 

In order to replicate the development environment, follow the instructions below:

A. OS: Ubuntu 16.04 LTS x64 (VirtualBox VM)
B. Eclipse CDT 
C. SDL1.2 ("Simple DirectMedia Layer is a cross-platform development library designed to provide low level access to audio, keyboard, mouse, joystick, and graphics hardware via OpenGL and Direct3D", find here https://www.libsdl.org/).
  i. The SDL team no longer supports version 1.2 but have now moved on to version 2.0. However, all of the libraries for version 1.2 are still available in the Linux repos but not for Windows. This project was developed with version 1.2.

Here are the steps to install SDL1.2 in Ubuntu:

  1)  Before installing anything, update your system 
      a. sudo apt-get update
      b. sudo apt-get upgrade
      c. Verify you have gcc installed ($gcc -v), if it's not there, install it.
  2)	Open the Ubuntu Software installer application
  3)	Search and install Synaptic Package Manager 
  4)	Search and mark for installation the following packages:
      a.	libsdl1.2debian
      b.	libsdl1.2-dev
      c.	libsdl-image-1.2
      d.	libsdl-image-1.2-dev
      e.	libsmpeg0
      f.	libsmgeg-dev
      g.	libsdl-mixer1.2
      h.	libsdl-mixer1.2-dev
      i.	libsdl-ttf2.0
      j.	libsdl-ttf2.0-dev
  5)	Install the SDL packages
  6)	Setup Eclipse CDT
      a.	Download and install the latest version from Eclipse.org. Do not get it from Ubuntu Software because they only have the 3.8 version from 2012. 
  7)	Download the Chip 8 project from GitHub and unzip it into your development directory
  8)	Open Eclipse and import the project
  9)	Eclipse needs to be configured as follows, under Project -> Properties: 
      a. C/C++ Build -> Tool chain: Linux GCC, Current Builder: Gnu Make Builder
      b. C/C++ Build -> Settings -> GCC C++ Linker -> Libraries, add the following libraries (type them in):
          1.	SDL_ttf
          2.	SDL_mixer
          3.	SDL_image
          4.	SDL
  10) Build project and run as a C/C++ application
