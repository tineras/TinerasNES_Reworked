#ifndef _NES_INPUT_HEADER
#define _NES_INPUT_HEADER

class NES_INPUT
{
public:
    NES_INPUT();
    ~NES_INPUT();

    void pressButton(unsigned char button);
    void releaseButton(unsigned char button);

    unsigned char readJoypad();
    void writeJoypad(unsigned char);

private:
    unsigned char _joypad_one;
    unsigned char _j, _joypad;
};
#endif //_NES_INPUT_HEADER
