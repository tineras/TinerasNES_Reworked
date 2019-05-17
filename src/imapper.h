#pragma once
#include "mem.h"

class IMapper
{
public:
    IMapper() {};

    virtual unsigned char read(int address) = 0;
    virtual void write(int address, unsigned char data) = 0;
};
