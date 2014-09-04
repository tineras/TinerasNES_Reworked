#ifndef _MEM_HEADER
#define _MEM_HEADER

#include "common.h"

class TinerasNES;
class PPU;
class APU;

class MEM
{
public:
    MEM(TinerasNES* tn);
    ~MEM();

    void init(PPU* ppu, APU* apu);


    void setCHRPRGBanks(char num_CHR_banks, char num_PRG_banks)
    {
        _num_CHR_banks = num_CHR_banks;
        _num_PRG_banks = num_PRG_banks;
    }
    void setHorizontalVerticalMirrorBit(char value) { _horiz_vert_mirror = (int)value; }
    void setMapperNumber(char num) { _mapper_num = num; }
    void initCHRPRG(int size_CHR, int size_PRG);
    void readCHR(char* mem_CHR, int length)
    {
        _memCHR.assign(mem_CHR, mem_CHR + length);
    }
    void readPRG(char* mem_PRG, int length)
    {
        _memPRG.assign(mem_PRG, mem_PRG + length);
    }
    // TODO: Fix this mess
    void initPRGMapperPtrs(int offset)
    {
        for(int i = 0; i < 4; i++)
        {
            for(int j = 0; j < 16; j++)
            {
                mapperPRGPtr[i + 4][j] = &_memPRG[i * 0x1000 + j * 0x0100 + offset];
            }
        }
    }

    unsigned char getMEM(int);
    void setMEM(int, unsigned char);

    unsigned char getPPU(int);

    unsigned char getMemoryMappedCPUByte(int);
    void setMemoryMappedCPUByte(int, unsigned char);
    unsigned char getMemoryMappedPPUByte(int);
    void setMemoryMappedPPUByte(int, unsigned char);

    void stackPush(unsigned char &, unsigned char);
    unsigned char stackPop(unsigned char &);

    void setVBlank(bool);
    void setSpriteHit(bool);

    // Handles nametable masking
    int getMaskedAddrData(int);

    // Reset MEM Variables
    void reset();

    TinerasNES *_tn;

    bool VBlank;

    int t;
    unsigned char fineX;

    unsigned char _memSPRRAM[256];    // SPR-RAM - Contains Sprite Attributes (64 sprites) x (4 bytes each) = 256 bytes (0x100)
    unsigned char _memCPU[65536];    // 64K of CPU Memory (0x10000)
    unsigned char _memPPU[16384];    // 16K of PPU Memory  (0x4000)        

    std::vector<unsigned char> _memPRG;    // Cart PRoGram Memory
    std::vector<unsigned char> _memCHR;    // Cart CHaRacter Memory

    // Mappers
    unsigned char* mapperPRGPtr[0x08][0x10]; // [0-8] * 0x1000, [0-F] * 0x0100
    unsigned char* mapperCHRPtr[0x02][0x10]; // [0-8] * 0x1000, [0-F] * 0x0100

private:
    unsigned char getPPUByte(int);
    void setPPUByte(int, unsigned char);

    int VRAMAddr;
    bool PPULatchToggle;

    // PPU Access (0x2006) Counter
    /* 0x2006 is written to 2 times followed by a read/write to 0x2007.
       This happens because each memory location can only take 1 byte so
       two writes are necessary to get the full address.  This counter
       (intPPUAccess) is used in the routines below to make
       sure both bytes are written to the appropriate location before
       0x2007 is called for reading/written to the location specified by
       0x2006.  A variable (intPPUAccess) has also been setup to hold the
       new 2-byte address.*/

    char prevByteRead;

    char _num_CHR_banks;
    char _num_PRG_banks;
    char _horiz_vert_mirror;    // Horizontal Mirror Bit (Horizontal(0)/Vertical(1))
    char _mapper_num;

    PPU* _ppu;
    APU* _apu;
};
#endif //_MEM_HEADER
