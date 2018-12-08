#include "mapper_0.h"

Mapper_0::Mapper_0()
{

}

Mapper_0::~Mapper_0()
{

}

unsigned char Mapper_0::read(int address)
{

}

void Mapper_0::write(int address, unsigned char data)
{
    _memPRG[address - 0x8000] = data;
}
