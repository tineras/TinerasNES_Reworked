################################################################################

File:        readme.txt

Author:      Aaron Wiginton

Project:     TinerasNES NES Emulator (C++)

Created:     September 3, 2014, 8:40 PM

Modified:    September 17, 2014, 10:01 PM

Version:     1

NOTE: This is an attempted rework of an older (and poorly architechted) attempt
 at a NES emulator I created with very little software engineering experience.

!! Please be patient while I work to clean this up. Lots of ugliness in the emu. !!

General Description: working emulation of the NES game console written in C++ 
using Microsoft Visual Studio. All critical systems are emulated including the 
CPU, Picture Processing Unit (PPU), Audio Processing Unit (APU) and Memory 
Mapped I/O allowing the CPU to access the various peripheral and memory devices.
Accurate emulation of the 2A03 (6502-based CPU core) including implementation of 
registers, complete instruction set and addressing modes. Per-pixel rendering is
 implemented for the PPU to realistically render background and sprite assets 
within the NTSC frame time. Audio emulation of Rectangle, Triangle and Noise 
channels. Simple Directmedia Layer (SDL) was used for rendering and audio.


Libraries Used:
- Qt 5.3.1 for Windows 32-bit (VS 2010, OpenGL, 537 MB)
[Ui, Drawing via QGLWidget]
(http://download.qt-project.org/official_releases/qt/5.3/5.3.1/qt-opensource-windows-x86-msvc2010_opengl-5.3.1.exe)
- SDL2 2.0.3
[Joystick Input, Audio]
(https://www.libsdl.org/release/SDL2-devel-2.0.3-VC.zip)

Project IDE and Misc:
- Visual Studio 2010 Premium
- Visual Studio Add-in 1.2.3 for Qt5
(http://download.qt-project.org/official_releases/vsaddin/qt-vs-addin-1.2.3-opensource.exe)

Misc. Setup Notes:
- Qt path from within Visual Studio [MenuBar]->Qt5->Qt Options
Name: Qt5.3.1
Path: C:\Qt\5.3.1\5.3\msvc2010_opengl
- Had to manually build SDL2 2.0.3 because of Code Generation > Runtime Library conflicts
between SDL and Qt. Built using Multi-threaded Debug DLL (/MDd). Place the Debug and
Release SDL2.dll in their respective folders next to TinerasNES.exe.

################################################################################