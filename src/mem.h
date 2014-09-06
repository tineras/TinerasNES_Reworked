#ifndef _MEM_HEADER
#define _MEM_HEADER

#include "common.h"

class TinerasNES;
class PPU;
class APU;
class NES_INPUT;

class MEM
{
public:
    MEM(TinerasNES* tn);
    ~MEM();

    void init(PPU* ppu, APU* apu, NES_INPUT* nes_input);

    void setCHRPRGBanks(char num_CHR_banks, char num_PRG_banks);
    void setHorizontalVerticalMirrorBit(char value);
    void setMapperNumber(char num);
    void initCHRPRG(int size_CHR, int size_PRG);
    void readCHR(char* mem_CHR, int length);
    void readPRG(char* mem_PRG, int length);
    void initPRGMapperPtrs(int offset);

    unsigned char getMEM(int address);
    void setMEM(int address, unsigned char data);

    unsigned char getPPU(int address);

    unsigned char getMemoryMappedCPUByte(int address);
    void setMemoryMappedCPUByte(int address, unsigned char data);

    unsigned char getMemoryMappedPPUByte(int address);
    void setMemoryMappedPPUByte(int address, unsigned char data);

    void stackPush(unsigned char &, unsigned char);
    unsigned char stackPop(unsigned char &);

    void setVBlank(bool vblank_status);
    void setSpriteHit(bool sprite_hit_status);

    // Handles nametable masking
    int getMaskedAddrData(int nametable_address);

    inline unsigned char memSPRRAM(int address) { return _memSPRRAM[address]; }
    inline unsigned char memCPU(int address) { return _memCPU[address]; }
    inline unsigned char memPPU(int address) { return _memPPU[address]; }

    inline void setMemCPU(int address, unsigned char data) { _memCPU[address] = data; }

    // Reset MEM Variables
    void reset();

    int _t;
    unsigned char _fineX;

    // Mappers
    unsigned char* _mapperPRGPtr[0x08][0x10]; // [0-8] * 0x1000, [0-F] * 0x0100
    unsigned char* _mapperCHRPtr[0x02][0x10]; // [0-8] * 0x1000, [0-F] * 0x0100

private:
    unsigned char getPPUByte(int address);
    void setPPUByte(int address, unsigned char data);

    int _VRAMAddr;
    bool _PPULatchToggle;

    // PPU Access (0x2006) Counter
    /* 0x2006 is written to 2 times followed by a read/write to 0x2007.
       This happens because each memory location can only take 1 byte so
       two writes are necessary to get the full address.  This counter
       (intPPUAccess) is used in the routines below to make
       sure both bytes are written to the appropriate location before
       0x2007 is called for reading/written to the location specified by
       0x2006.  A variable (intPPUAccess) has also been setup to hold the
       new 2-byte address.*/

    char _prevByteRead;

    char _num_CHR_banks;
    char _num_PRG_banks;
    char _horiz_vert_mirror;    // Horizontal Mirror Bit (Horizontal(0)/Vertical(1))
    char _mapper_num;

    // TODO: Make private and pass pointers to ppu for access
    unsigned char _memSPRRAM[256];  // SPR-RAM - Contains Sprite Attributes (64 sprites) x (4 bytes each) = 256 bytes (0x100)
    unsigned char _memCPU[65536];   // 64K of CPU Memory (0x10000)
    unsigned char _memPPU[16384];   // 16K of PPU Memory  (0x4000)

    std::vector<unsigned char> _memPRG;    // Cart PRoGram Memory
    std::vector<unsigned char> _memCHR;    // Cart CHaRacter Memory

    TinerasNES *_tn;
    PPU* _ppu;
    APU* _apu;
    NES_INPUT* _nes_input;
};
#endif //_MEM_HEADER
