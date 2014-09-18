#ifndef _NES_INPUT_HEADER
#define _NES_INPUT_HEADER

#include "SDL.h"

class NES_INPUT
{
public:
    NES_INPUT();
    ~NES_INPUT();

    void pressButton(unsigned char button);
    void releaseButton(unsigned char button);

    unsigned char readJoypad();
    void writeJoypad(unsigned char);

    void handleSDLJoystickEvents();

private:
    SDL_Event _joystick_event;
    SDL_Joystick* _joystick;

    unsigned char _joypad_one;
    unsigned char _j, _joypad;
};
#endif //_NES_INPUT_HEADER
