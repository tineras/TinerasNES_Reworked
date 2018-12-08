#pragma once

class IMapper
{
public:
    IMapper();
    ~IMapper();

    virtual unsigned char write(int address)
    {
        int addrTemp = address - 0x8000;
        unsigned char loc_one = ((addrTemp & 0xF000) >> 12) & 0xFF;
        unsigned char loc_two = ((addrTemp & 0x0F00) >> 8) & 0xFF;
        unsigned char offset = (addrTemp & 0x00FF) & 0xFF;

        unsigned char *returnTempPrt = _mapperPRGPtr[loc_one][loc_two] + offset;
        unsigned char returnTemp = *returnTempPrt;

        return returnTemp;
    }
    virtual void read(int address, unsigned char data) = 0;
};
