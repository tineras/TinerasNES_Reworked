#include "mapper_0.h"

Mapper_0::Mapper_0(MEM* mem) : _mem(mem)
{

}

Mapper_0::~Mapper_0()
{

}

unsigned char Mapper_0::read(int address)
{
    int addrTemp = address - 0x8000;
    unsigned char loc_one = ((addrTemp & 0xF000) >> 12) & 0xFF;
    unsigned char loc_two = ((addrTemp & 0x0F00) >> 8) & 0xFF;
    unsigned char offset = (addrTemp & 0x00FF) & 0xFF;

    unsigned char *returnTempPrt = &(_mem->mapperPRGPtr())[loc_one][loc_two] + offset;
    unsigned char returnTemp = *returnTempPrt;

    return returnTemp;
}

void Mapper_0::write(int address, unsigned char data)
{
    _mem->memPRG()[address - 0x8000] = data;
}
