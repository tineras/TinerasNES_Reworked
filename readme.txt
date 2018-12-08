################################################################################

File:        readme.txt

Author:      Aaron Wiginton

Project:     TinerasNES NES Emulator (C++)

Created:     September 3, 2014, 8:40 PM

Modified:    December 4, 2018, 11:30 PM

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
channels. Simple Directmedia Layer (SDL) was used for input and audio.


Libraries Used
--------------

- Qt 5.7.0 64-bit for Windows [Ui, Drawing via QGLWidget]
    - http://download.qt.io/official_releases/qt/5.7/5.7.0/qt-opensource-windows-x86-msvc2015_64-5.7.0.exe

- SDL2 2.0.9 [Joystick Input, Audio]
	- https://www.libsdl.org/release/SDL2-2.0.9-win32-x64.zip (DLL)
    - https://www.libsdl.org/release/SDL2-devel-2.0.9-VC.zip (Includes/Libs)

Project IDE and Misc:
- Visual Studio 2017
- Qt5 Visual Studio 2017 Add-in 2.2.2
	- https://download.qt.io/archive/vsaddin/qt-vsaddin-msvc2017-2.2.2.vsix


Installation and Setup
----------------------

Installation Instructions (Assumes Visual Studio is installed):
1. Install Qt 5.7.0 package above (default dir is fine: c:\qt\5.7.0)
2. Install Qt Visual Studio Addin from above
3. Configure Qt Visual Studio settings in the Visual Studio Toolbar
    a. Select a name (anything is fine... like "Qt5.7.0")
    b. Select the proper directory (C:\Qt\Qt5.7.0\5.7\msvc2015_64\)
    c. Set it as the default at the bottom of the dialog
4. Configure Qt MemVault project settings from Solution Explorer
    a. Right click on project in Solution Explorer and select Qt Project Settings
    b. In the Properties tab, select the Version created in the step above (ex. Qt5.7.0)
    c. In the Qt Modules tab, select modules needed by the project
        - Currently: Core, GUI, Print Support, Widgets
        - To find which module is needed, lookup the include string (ex. QClipboard)
          The official Qt page when searching for 'QClipboard' should indicate that it
          is part of the QtGui module. Therefore, you must include the 'Gui' module.


Misc
----

- Place SDL2.dll alongside the TinerasNES executable (in x64/[Debug/Release]).

################################################################################