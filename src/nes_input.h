/*    NES_INPUT.h
    Author: Aaron Wiginton

    Description: Handles keyboard and mouse input.
*/

#ifndef _NES_INPUT_HEADER
#define _NES_INPUT_HEADER

//class TinerasNES;

class NES_INPUT
{
public:
    NES_INPUT();
    ~NES_INPUT();

    unsigned char joypadOne;
    unsigned char j, joypad;

    unsigned char readJoypad();
    void writeJoypad(unsigned char);
    
    // Reset Input Variables
    void reset();

private:


};
#endif //_NES_INPUT_HEADER