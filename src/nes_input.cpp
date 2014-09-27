#include "nes_input.h"

//    **********
//    INPUT Constructor
NES_INPUT::NES_INPUT() :
    _j(0),
    _joypad(0),
    _joypad_one(0)
{
}

//    **********
//    INPUT Destructor
NES_INPUT::~NES_INPUT()
{
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
