#include "nes_input.h"

void NES_INPUT::pressButton(unsigned char button)
{
    joypadOne |= button;
}

void NES_INPUT::releaseButton(unsigned char button)
{
    joypadOne &= button;
}

unsigned char NES_INPUT::readJoypad()
{
    unsigned char data;

    if (j >= 8)
    {
        // There is a reference to this in the Original NES Reference Document
        data = 0x40;
    }
    else
    {
        data = (unsigned char)(((joypad >> j++) & 0x01) + 0x40);
    }
    return data;
}

void NES_INPUT::writeJoypad(unsigned char data)
{
    if ((data & 0x01) == 0x00)
    {
        j = 0;
    }
    else if ((data & 0x01) == 0x01)
    {
        j = 0;
        joypad = joypadOne;
    }
}

//    **********
//    Rest MEM Variables
void NES_INPUT::reset()
{
    j = 0;
    joypad = 0;
    joypadOne = 0;
}

//    **********
//    INPUT Constructor
NES_INPUT::NES_INPUT()
{
    reset();
}

//    **********
//    INPUT Destructor
NES_INPUT::~NES_INPUT()
{

}