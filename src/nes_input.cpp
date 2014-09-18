#include "nes_input.h"

//    **********
//    INPUT Constructor
NES_INPUT::NES_INPUT() :
    _joystick(nullptr),
    _j(0),
    _joypad(0),
    _joypad_one(0)
{
    SDL_JoystickEventState(SDL_ENABLE);

    int num = SDL_NumJoysticks();

    _joystick = SDL_JoystickOpen(0);

    // This must be set in order to detect joystick input
    SDL_SetHint(SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS, "1");
}

//    **********
//    INPUT Destructor
NES_INPUT::~NES_INPUT()
{
    SDL_JoystickClose(_joystick);
}

void NES_INPUT::pressButton(unsigned char button)
{
    _joypad_one |= button;
}

void NES_INPUT::releaseButton(unsigned char button)
{
    _joypad_one &= button;
}

unsigned char NES_INPUT::readJoypad()
{
    unsigned char data;

    if (_j >= 8)
    {
        // There is a reference to this in the Original NES Reference Document
        data = 0x40;
    }
    else
    {
        data = (unsigned char)(((_joypad >> _j++) & 0x01) + 0x40);
    }
    return data;
}

void NES_INPUT::writeJoypad(unsigned char data)
{
    if ((data & 0x01) == 0x00)
    {
        _j = 0;
    }
    else if ((data & 0x01) == 0x01)
    {
        _j = 0;
        _joypad = _joypad_one;
    }
}

void NES_INPUT::handleSDLJoystickEvents()
{
    while(SDL_PollEvent(&_joystick_event) != 0)
    {
        switch(_joystick_event.type)
        {
        case SDL_JOYBUTTONDOWN:
            {
                if (_joystick_event.jbutton.button == 0)
                {
                    pressButton(0x10);
                }
                else if (_joystick_event.jbutton.button == 1)
                {
                    pressButton(0x20);
                }
                else if (_joystick_event.jbutton.button == 2)
                {
                    pressButton(0x40);
                }
                else if (_joystick_event.jbutton.button == 3)
                {
                    pressButton(0x80);
                }
                else if (_joystick_event.jbutton.button == 4)
                {
                    pressButton(0x08);
                }
                else if (_joystick_event.jbutton.button == 5)
                {
                    pressButton(0x04);
                }
                else if (_joystick_event.jbutton.button == 10)
                {
                    pressButton(0x01);
                }
                else if (_joystick_event.jbutton.button == 12)
                {
                    pressButton(0x02);
                }
            }
            break;
        case SDL_JOYBUTTONUP:
            {
                if (_joystick_event.jbutton.button == 0)
                {
                    releaseButton(0xEF);
                }
                else if (_joystick_event.jbutton.button == 1)
                {
                    releaseButton(0xDF);
                }
                else if (_joystick_event.jbutton.button == 2)
                {
                    releaseButton(0xBF);
                }
                else if (_joystick_event.jbutton.button == 3)
                {
                    releaseButton(0x7F);
                }
                else if (_joystick_event.jbutton.button == 4)
                {
                    releaseButton(0xF7);
                }
                else if (_joystick_event.jbutton.button == 5)
                {
                    releaseButton(0xFB);
                }
                else if (_joystick_event.jbutton.button == 10)
                {
                    releaseButton(0xFE);
                }
                else if (_joystick_event.jbutton.button == 12)
                {
                    releaseButton(0xFD);
                }
            }
            break;
        case SDL_HAT_DOWN:
            {
                if (_joystick_event.jhat.value & SDL_HAT_LEFT)
                {
                }
            }
        case SDL_HAT_UP:
            {
                if (_joystick_event.jhat.value & SDL_HAT_LEFT)
                {
                }
            }
        default:
            break;
        }
    }
}
