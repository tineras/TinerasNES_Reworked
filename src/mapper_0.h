#pragma once
#include "imapper.h"

class MEM;

class Mapper_0 : public IMapper
{
public:
    Mapper_0(MEM* mem);
    ~Mapper_0();

    virtual unsigned char read(int address);
    virtual void write(int address, unsigned char data);
private:
    MEM* _mem;
};
