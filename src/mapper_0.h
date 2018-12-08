#pragma once

class Mapper_0 : public IMapper
{
public:
    IMapper();
    ~IMapper();

    virtual void write(int address, unsigned char data);
    virtual unsigned char read(int address);
};
