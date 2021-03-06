    /* -***-   MEMORY MAP   -***-

       ----- CPU Memory Map -----

    -Purpose-               -Addr-      -Length (Bytes)-
- [32K] Memory Mapped Cartridge PRG-ROM
    PRG-ROM Upper Bank      (C000-FFFF) [16384] // Cartridge RAM - Second Bank
    PRG-ROM Lower Bank      (8000-BFFF) [16384] // Cartridge RAM - First Bank
- [32K] Internal CPU Memory
    SRAM                    (6000-7FFF) [ 8192]
    Expansion ROM           (4020-5FFF) [ 8160]
    I/O Registers           (4000-401F) [   32] // APU (Audio) I/O control, joypads and misc., $4014 used for PPU DMA
    Mirrors                 (2008-3FFF) [ 8184] // Mirrors 2000-2007 every eight bytes from 2008 to 3FFF
    I/O Registers           (2000-2007) [    8] // PPU I/O control and misc.
    Mirrors                 (0800-1FFF) [ 6144] // Mirrors 0000-07FF 3 Times Starting at 0800, 1000 and 1800
    RAM                     (0200-07FF) [ 1536] // RAM (internal)
    Stack                   (0100-01FF) [  256]
    Zero-Page               (0000-00FF) [  256]


       ----- PPU Memory Map -----

    Mirror                  (4000-FFFF)[49152] // Mirrors ($0000-$3FFF) <- Logical, not physical
- [16K] Internal NES VRAM -
    Mirror                  (3F20-3FFF) [ 224] // Mirrors ($3F00-$3F1F)
    Sprite Palette          (3F10-3F1F) [  16] // Points (does not contain) to index of colors that can be used by sprites (only 64 possibilities, bits 6 and 7 can be ignored)
    Image Palette           (3F00-3F0F) [  16] // Points (does not contain) to index of colors that can be used by background (only 64 possibilities, bits 6 and 7 can be ignored)
    Mirror                  (3000-3EFF) [3839] // Mirrors ($2000-$2EFF)
    Attribute Table 3       (2FC0-2FFF) [  64] //
    Name Table 3            (2C00-2FBF) [ 960] // (32 x 30 Tiles. Each tile is 8bits x 8bits.  32 Bytes * 30 Bytes = 960B or one whole 256x240 screen)
    Attribute Table 2       (2BC0-2BFF) [  64] //
    Name Table 2            (2800-2BBF) [ 960] // (32 x 30 Tiles. Each tile is 8bits x 8bits.  32 Bytes * 30 Bytes = 960B or one whole 256x240 screen)
    Attribute Table 1       (27C0-27FF) [  64] //
    Name Table 1            (2400-27BF) [ 960] // (32 x 30 Tiles. Each tile is 8bits x 8bits.  32 Bytes * 30 Bytes = 960B or one whole 256x240 screen)
    Attribute Table 0       (23C0-23FF) [  64] // (1 Byte per tile on the screen; Holds
    Name Table 0            (2000-23BF) [ 960] // (32 x 30 Tiles)
- [8K] Memory Mapped Cartridge CHR-ROM (VRAM/VROM)
    Pattern Table 1         (1000-1FFF) [4096] // (256 x 2 x 8, may be VROM)
    Pattern Table 2         (0000-0FFF) [4096] // (256 x 2 x 8, may be VROM)


       ----- Sprite Memory Map -----     

    SPR-RAM (OAM)           (0000-00FF) [ 256] // Contains Sprite Attributes (64 sprites) x (4 bytes each) = 256 bytes

    */

#include "mem.h"
#include "common.h"
#include "TinerasNES.h"
#include "ppu.h"
#include "apu.h"
#include "nes_input.h"
#include "imapper.h"

#include "mapper_0.h"

//    **********
//    MEM Constructor
MEM::MEM(TinerasNES* tn)
{
    /* Set local tn pointer to point to TinerasNES object TN
    so that its members/objects outside of CPU class can be
    accessed from within CPU class.    */
    _tn = tn;

    reset();
}

//    **********
//    MEM Destructor
MEM::~MEM()
{
}

void MEM::init(PPU* ppu, APU* apu, NES_INPUT* nes_input)
{
    _ppu = ppu;
    _apu = apu;
    _nes_input = nes_input;
}

void MEM::setCHRPRGBanks(char num_CHR_banks, char num_PRG_banks)
{
    _num_CHR_banks = num_CHR_banks;
    _num_PRG_banks = num_PRG_banks;
}

void MEM::setHorizontalVerticalMirrorBit(char value)
{
    _horiz_vert_mirror = (int)value;
}

void MEM::setMapperNumber(char num)
{
    _mapper_num = num;

    switch (_mapper_num)
    {
    case 00:
    {
        _mapper = std::make_shared<Mapper_0>(this);
    }
    break;
    case 02: // UNROM
    case 66:
    {
        // TODO: Implement this properly
        _mapper = std::make_shared<Mapper_0>(this);
    }
    break;
    case 03: // CNROM
    case 67:
    case 64:
    {
        // TODO: Implement this properly
        _mapper = std::make_shared<Mapper_0>(this);
    }
    break;
    default:
    {
        QMessageBox::warning(nullptr, "Error", QString("No implementation for mapper: %1").arg((int)num));
    }
    break;
    }
}

//    **********
//    Create CHR and PRG Memory
void MEM::initCHRPRG(int size_CHR, int size_PRG)
{
    _memPRG.resize(size_PRG);

    // Initialize PRG Mapper Pointers
    for(int i = 0; i < 8; i++)
    {
        for(int j = 0; j < 16; j++)
        {
            _mapperPRGPtr[i][j] = &_memPRG[i * 0x1000 + j * 0x0100];
        }
    }

    if(size_CHR != 0)
    {
        _memCHR.resize(size_CHR);

        // Initialize CHR Mapper Pointers
        for(int i = 0; i < 2; i++)
        {
            for(int j = 0; j < 16; j++)
            {
                _mapperCHRPtr[i][j] = &_memCHR[i * 0x1000 + j * 0x0100];
            }
        }
    }
    else if(size_CHR == 0)
    {
        _memCHR.resize(0x2000);

        // Initialize CHR Mapper Pointers
        for(int i = 0; i < 2; i++)
        {
            for(int j = 0; j < 16; j++)
            {
                _mapperCHRPtr[i][j] = &_memCHR[i * 0x1000 + j * 0x0100];
            }
        }
    }
}

// Read CHR data from ROM into cart memory
void MEM::readCHR(char* mem_CHR, int length)
{
    if (length > 0)
        memcpy(&_memCHR.at(0), mem_CHR, length);
}

// Read PRG data from ROM into cart memory
void MEM::readPRG(char* mem_PRG, int length)
{
    if (length > 0)
        memcpy(&_memPRG.at(0), mem_PRG, length);
}

// TODO: Fix this mess
void MEM::initPRGMapperPtrs(int offset)
{
    for(int i = 0; i < 4; i++)
    {
        for(int j = 0; j < 16; j++)
        {
            _mapperPRGPtr[i + 4][j] = &_memPRG[i * 0x1000 + j * 0x0100 + offset];
        }
    }
}

//    **********
//    Push Stack
void MEM::stackPush(unsigned char &SP, unsigned char data)
{    
    // Push data byte on stack (Remember...stack works in reverse)
    _memCPU[SP + 0x100] = data;

    // Increment Stack Pointer
    SP--;
}

//    **********
//    Pop Stack
unsigned char MEM::stackPop(unsigned char &SP)
{
    // Decrement Stack Pointer (Remember...stack works in reverse)
    SP++;

    // Pull data byte from stack
    return _memCPU[SP + 0x100];
}

//    **********
//    Get Memory (PRG)
unsigned char MEM::getMEM(int address)
{
    // Prevents invalid reads by wrapping around after addition
    address &= 0xFFFF;

    // Catch out of range addressing <<-- This should not happen because of the line above
    if (address > 0xFFFF)
    {
        //MessageBox.Show("[OpCodes.cs, getMemoryByte()] BAD ADDRESS: " + String.Format("{0:x2}", addr));
        //cpu.badOpCode = true;
        return 0;
    }

    // Address Masking at 0800-1FFF
    if ((address < 0x2000))
    {
        return _memCPU[address & 0x07FF];
    }

    // Handle PPU related OpCodes (in Memory class)
    else if ((address >= 0x2000) && (address <= 0x2007))
    {
        return getPPUByte(address);
    }

    // Address Masking for PPU at 2008-3FFF
    else if ((address >= 0x2008) && (address <= 0x3FFF))
    {
        return getPPUByte(address & 0x2007);
    }

    #pragma region APU - Control Register (READ)
    else if (address == 0x4015)
    {
        unsigned char data;

        if(_apu->dmcIRQEnabled())      data |= 0x80;    // Return DMC IRG Flag Status
        if(_apu->frameIRQEnabled())    data |= 0x40;    // Return Frame Interrupt Flag Status

        //if(_apu->dmcLength)          data |= 0x10;    // Return DMC Enabled Status
        if(_apu->noise()->length)    data |= 0x08;    // Return Noise Status
        if(_apu->triangle()->length) data |= 0x04;    // Return Triangle Status
        if(_apu->rect2()->length)    data |= 0x02;    // Return Rectangle/Pulse 2 Status
        if(_apu->rect1()->length)    data |= 0x01;    // Return Rectangle/Pulse 1 Status

        return data;
    }
    #pragma endregion

    // Joystick Read
    else if (address == 0x4016)
    {
        // Read Joypad 1
        return _nes_input->readJoypad();
    }

    // Address Masking for PPU at 2008-3FFF
    else if ((address >= 0x8000) && (address <= 0xFFFF))
    {
        return getMemoryMappedCPUByte(address);
    }

    // Handles all memory spaces not related to PPU and controllers
    else
    {
        return _memCPU[address];
    }

    // Should never happen
    return 0x0;
}

//    **********
//    Set Memory (PRG)
void MEM::setMEM(int address, unsigned char data)
{
    // Prevents invalid reads by wrapping around after addition
    address &= 0xFFFF;

    // Copy to 0x0000-0x07FF and Mirror at 0x0800-0x1FFF
    if (address < 0x2000)
    {
        // To intended memory address
        _memCPU[address & 0x07FF] = data;
    }

    // Handle PPU related OpCodes (in Memory class)
    else if ((address >= 0x2000) && (address <= 0x2007))
    {
        // Set byte to memory
        setPPUByte(address, data);
    }

    // Address Masking for PPU at 2008-3FFF
    else if ((address >= 0x2008) && (address <= 0x3FFF))
    {
        setPPUByte(address & 0x2007, data);
    }

    #pragma region 256 Byte DMA Write to Sprite Memory
    // Handles 256 Byte DMA Write to Sprite Memory
    else if (address == 0x4014)
    {
        for (int i = 0; i < 256; ++i, ++_memCPU[0x2003])
        {
            _memSPRRAM[_memCPU[0x2003]] = _memCPU[0x0100 * data + i];
        }

        // This operation takes 512 cpu cycles
        //_tn->currentCPUCycle += 512 * 15; // OLD
        _tn->bumpCurrentCPUCycle(512 * 15);
    }
    #pragma endregion

    #pragma region Write to Joypads
    // Joypads
    else if (address == 0x4016)
    {
        // Write to Joypad 1
        _nes_input->writeJoypad(data);
    }
    #pragma endregion

    #pragma region Mappers
    else if (address >= 0x8000 && address <= 0xFFFF)
    {
        // Do Mapper Stuff
        setMemoryMappedCPUByte(address, data);
    }
    #pragma endregion
    // Handles all memory spaces not related to PPU, and controllers
    else
    {
        if (address < 0x4000 || address > 0x4020)
        {
            // MessageBox.Show("[OpCodes.cs, setMemoryByte()] BAD ADDRESS: " + String.Format("{0:x2}", addr));
            // memory.memCPU[addr] = data;
        }

        #pragma region APU - Rectangle/Pulse 1 Registers Write
        if (address >= 0x4000 && address <= 0x4003)
        {
            if(address == 0x4000)
            {
                /* $4000/4 ddle vvvv   (dd) duty cycle type
                                            00=12.5%      01000000
                                            01=25%        01100000
                                            10=50%        01111000
                                            11=75%        10011111
                                        (l) length counter clock disable (halt) / env decay looping enable
                                        (e) env decay disable/constant volume flag (0 - decay, 1 - constant volume)
                                        (vvvv)  Constant Volume Mode: 0000=Silence, 1111=Maximum
                                                Constant Decay Mode: Decay time (divider's period is set to N + 1) */

                /**** DEBUG ****/
                _apu->setRegUpdated(true);
                _apu->setRegUpdateValue(0, data);
                /***************/

                _apu->rect1()->dutyType = (data >> 6) & 0x03;
                if((data >> 5) & 0x01)    _apu->rect1()->decayLoopEnable = true;    else _apu->rect1()->decayLoopEnable = false;
                if((data >> 4) & 0x01)    _apu->rect1()->decayDisable = true;    else _apu->rect1()->decayDisable = false;
                _apu->rect1()->decayEnvVol = data & 0x0F;

                if(_apu->rect1()->decayDisable)
                    _apu->rect1()->volume = _apu->rect1()->decayEnvVol;
                else
                    _apu->rect1()->volume = _apu->rect1()->envelope;
            }
            if(address == 0x4001)
            {
                /* $4001/5 eppp nsss    (e) enable sweep
                                        (ppp) period/sweep update rate (divider's period is set to P + 1)
                                        (n) Negate Flag / decrease/increase (1/0) wavelength
                                        (sss) right shift amount    */

                /**** DEBUG ****/
                _apu->setRegUpdated(true);
                _apu->setRegUpdateValue(1, data);
                /***************/

                if(data & 0x80) _apu->rect1()->sweepEnable = true;    else _apu->rect1()->sweepEnable = false;
                _apu->rect1()->sweepRate = ((data & 0x70) >> 4);
                if(data & 0x08) _apu->rect1()->sweepNeg = true;    else _apu->rect1()->sweepNeg = false;
                _apu->rect1()->sweepShift = data & 0x07;

                if(_apu->rect1()->sweepShift == 0)
                    _apu->rect1()->sweepEnable = false;

                _apu->rect1()->sweepReset = true;
                _apu->rect1()->checkSweepSilence();
            }
            if(address == 0x4002)
            {
                /* $4002/6 pppp pppp    (pppp pppp) period low (8 LSB of wavelength)    */

                /**** DEBUG ****/
                _apu->setRegUpdated(true);
                _apu->setRegUpdateValue(2, data);
                /***************/

                _apu->rect1()->period = (_apu->rect1()->period & 0x700) | data;

                _apu->rect1()->checkSweepSilence();
            }
            if(address == 0x4003)
            {
                /* $4003/7 llll lppp    (llll l) length index
                                        (ppp) period high    */    

                /**** DEBUG ****/
                _apu->setRegUpdated(true);
                _apu->setRegUpdateValue(3, data);
                /***************/

                _apu->rect1()->length = _apu->rect1()->lengthTable[(data & 0xF8) >> 3];
                _apu->rect1()->period = (_apu->rect1()->period & 0xFF) | ((data & 0x07) << 8);

                _apu->rect1()->decayReset = true;
                _apu->rect1()->checkSweepSilence();

            }
        }
        #pragma endregion

        #pragma region APU - Rectangle/Pulse 2 Registers Write
        if (address >= 0x4004 && address <= 0x4007)
        {
            if(address == 0x4004)
            {
                /* $4000/4 ddle vvvv   (dd) duty cycle type
                                            00=12.5%      01000000
                                            01=25%        01100000
                                            10=50%        01111000
                                            11=75%        10011111
                                        (l) length counter clock disable (halt) / env decay looping enable
                                        (e) env decay disable/constant volume flag (0 - decay, 1 - constant volume)
                                        (vvvv)  Constant Volume Mode: 0000=Silence, 1111=Maximum
                                                Constant Decay Mode: Decay time (divider's period is set to N + 1) */

                _apu->rect2()->dutyType = (data >> 6) & 0x03;
                if((data >> 5) & 0x01)    _apu->rect2()->decayLoopEnable = true;    else _apu->rect2()->decayLoopEnable = false;
                if((data >> 4) & 0x01)    _apu->rect2()->decayDisable = true;    else _apu->rect2()->decayDisable = false;
                _apu->rect2()->decayEnvVol = data & 0x0F;

                if(_apu->rect2()->decayDisable)
                    _apu->rect2()->volume = _apu->rect2()->decayEnvVol;
                else
                    _apu->rect2()->volume = _apu->rect2()->envelope;
            }
            if(address == 0x4005)
            {
                /* $4001/5 eppp nsss    (e) enable sweep
                                        (ppp) period/sweep update rate (divider's period is set to P + 1)
                                        (n) Negate Flag / decrease/increase (1/0) wavelength
                                        (sss) right shift amount    */

                if(data & 0x80) _apu->rect2()->sweepEnable = true;    else _apu->rect2()->sweepEnable = false;
                _apu->rect2()->sweepRate = ((data & 0x70) >> 4);
                if(data & 0x08) _apu->rect2()->sweepNeg = true;    else _apu->rect2()->sweepNeg = false;
                _apu->rect2()->sweepShift = data & 0x07;
                
                if(_apu->rect2()->sweepShift == 0)
                    _apu->rect2()->sweepEnable = false;

                _apu->rect2()->sweepReset = true;
                _apu->rect2()->checkSweepSilence();
            }
            if(address == 0x4006)
            {
                /* $4002/6 pppp pppp    (pppp pppp) period low (8 LSB of wavelength)    */

                _apu->rect2()->period = (_apu->rect2()->period & 0x700) | data;

                _apu->rect2()->checkSweepSilence();
            }
            if(address == 0x4007)
            {
                /* $4003/7 llll lppp    (llll l) length index
                                        (ppp) period high    */

                _apu->rect2()->length = _apu->rect2()->lengthTable[(data & 0xF8) >> 3];
                _apu->rect2()->period = (_apu->rect2()->period & 0xFF) | ((data & 0x07) << 8);

                _apu->rect2()->decayReset = true;
                _apu->rect2()->checkSweepSilence();
            }
        }
        #pragma endregion

        #pragma region APU - Triangle Registers Write
        if (address >= 0x4008 && address <= 0x400B)
        {
            if(address == 0x4008)
            {
                /* $4008   clll llll    (c) linear counter start (halt)/length counter clock disable
                                        (lll llll) linear counter load    */

                _apu->triangle()->linearCounterDisable = (data >> 7) & 0x01;
                _apu->triangle()->linearCount = data & 0x7F;

                //if(_apu->rect2()->decayDisable)
                //    _apu->rect2()->volume = _apu->rect2()->decayEnvVol;
                //else
                //    _apu->rect2()->volume = _apu->rect2()->envelope;
            }
            if(address == 0x4009)
            {
                /* $4009 Unused */
            }
            if(address == 0x400A)
            {
                /* $400A   pppp pppp   (pppp pppp) period low (8 LSB of wavelength)    */

                _apu->triangle()->period = (_apu->triangle()->period & 0x700) | data;

                _apu->triangle()->updateFrequency();
            }
            if(address == 0x400B)
            {
                /*     $400B   llll lppp   (llll l) length index
                                        (ppp) period high    */    

                if(_apu->triangleEnabled())
                    _apu->triangle()->length = _apu->triangle()->lengthTable[(data & 0xF8) >> 3];
                _apu->triangle()->period = (_apu->triangle()->period & 0xFF) | ((data & 0x07) << 8);

                // Set Halt Flag
                _apu->triangle()->linearHalt = true;

                //_apu->rect2()->decayReset = true;
                _apu->triangle()->updateFrequency();
            }
        }
        #pragma endregion

        #pragma region APU - Noise Registers Write
        if (address >= 0x400C && address <= 0x400F)
        {
            if(address == 0x400C)
            {
                /*    $400C   --le vvvv   (l) loop env/disable length
                                        (e) env disable
                                        (vvvv) vol/env period    */

                if((data >> 5) & 0x01)    _apu->noise()->decayLoopEnable = true;    else _apu->noise()->decayLoopEnable = false;
                if((data >> 4) & 0x01)    _apu->noise()->decayDisable = true;    else _apu->noise()->decayDisable = false;
                _apu->noise()->decayEnvVol = data & 0x0F;

                if(_apu->noise()->decayDisable)
                    _apu->noise()->volume = _apu->noise()->decayEnvVol;
                else
                    _apu->noise()->volume = _apu->noise()->envelope;
            }
            if(address == 0x400D)
            {
                /* $400D Unused */
            }
            if(address == 0x400E)
            {
                /*    $400E   s--- pppp   (s) short/tone mode
                                        (pppp) period index/playback sample rate    */

                _apu->noise()->mode = (data >> 7) & 0x01;
                int _period = (data & 0x0F);
                
                if(_apu->frameNTSCPAL() == 4)
                    _apu->noise()->period = _apu->noise()->periodTable_NTSC[_period];
                else
                    _apu->noise()->period = _apu->noise()->periodTable_PAL[_period];

                _apu->noise()->updateFrequency();
            }
            if(address == 0x400F)
            {
                /*     $400F   llll l---   (llll l) length index    */    

                _apu->noise()->length = _apu->noise()->lengthTable[(data & 0xF8) >> 3];

                _apu->noise()->decayReset = true;
            }
        }
        #pragma endregion

        #pragma region APU - Control Registers
        if (address == 0x4015)
        {
            /*(write)                    
            $4015   ---d nt21   (d) DMC (If clear, the DMC's bytes remaining is set to 0,
                                    otherwise the DMC sample is restarted only if the DMC's bytes remaining is 0)
                                (n) noise enable flag
                                (t) triangle enable flag
                                (2) pulse/rect 2 enable flag
                                (1) pulse/rect 1 enable flag
                                
                                ** Clears DMC Interrupt Flag after Write ($4010.7)*/

            if(data & 0x10)    _apu->setDMCEnabled(true);    else _apu->setDMCEnabled(false);        // DMC Enable/Disable
            if(data & 0x08)    _apu->setNoiseEnabled(true);    else _apu->setNoiseEnabled(false);    // Noise Enable/Disable
            if(data & 0x04)    _apu->setTrianlgeEnabled(true);    else _apu->setTrianlgeEnabled(false);        // Triangle Enable/Disable
            if(data & 0x02)    _apu->setRectangle2Enabled(true);    else _apu->setRectangle2Enabled(false);    // Rect/Pulse 2 Enable/Disable
            if(data & 0x01)    _apu->setRectangle1Enabled(true);    else _apu->setRectangle1Enabled(false);    // Rect/Pulse 1 Enable/Disable

            if(!_apu->triangleEnabled())
                _apu->triangle()->length = 0;
            if(!_apu->rectangle2Enabled())
                _apu->rect2()->length = 0;
            if(!_apu->rectangle1Enabled())
                _apu->rect1()->length = 0;
        }

        if (address == 0x4017)
        {
                /* $4017   mi-- ----    (f) 0 - 4-frame cycle mode
                                            1 - 5-frame cycle mode
                                        (i) Interrupt Inhibit Flag. If set, the frame interrupt flag is cleared, otherwise it is unaffected.

                                        ** Clears frame Interrupt Flag after Write ($4010.7)
                                        ** If an interrupt flag was set at the same moment of the read,
                                            it will read back as 1 but it will not be cleared.    */

            if(data & 0x80)    _apu->setFrameNTSCPAL(5);    else _apu->setFrameNTSCPAL(4);    // 4 or 5 Frame Count
            _apu->setFrameCounter(0);    // Reset Frame Counter
            if(data & 0x40) _apu->setFrameIRQEnabled(false); else _apu->setFrameIRQEnabled(true);
        }
        #pragma endregion

        _memCPU[address] = data;
    }
}

//    **********
//    Get PPU Memory
unsigned char MEM::getPPUByte(int address)
{
    #pragma region Read 0x2000
    // PPU Control Register 1
    if (address == 0x2000)
    {
        return _memCPU[0x2000];
    }
    #pragma endregion

    #pragma region Read 0x2001
    // PPU Control Register 2
    else if (address == 0x2001)
    {
        return _memCPU[0x2001];
    }
    #pragma endregion

    #pragma region Read 0x2002
    // PPU Status Register
    else if (address == 0x2002)
    {
        // Clear the VBLANK Flag here.  This is documented (p. 5 on 2c02 doc)

        char tempByte = _memCPU[0x2002];
        
        // Set Vblank to false
        _memCPU[0x2002] &= 0x7F;

        // Set SpriteHit to false
        _memCPU[0x2002] &= 0xBF;

        //// Also resets 1st/2nd write flipflop used by 2005 and 2006
        //memCPU[0x2005] = 0;
        //memCPU[0x2006] = 0;

        //t = 0;
        // Reading 2002 will clear the latch, NOT WRITING 2002
        _PPULatchToggle = false;

        return tempByte;
    }
    #pragma endregion

    #pragma region Read 0x2003
    // PPU Status Register
    else if (address == 0x2003)
    {
        // Program should not need to read this
        return _memCPU[0x2003];
    }
    #pragma endregion

    #pragma region Read 0x2004
    // Sprite Memory Data
    else if (address == 0x2004)
    {
        // Pg 5 of 117 in "NES Specifications says that 0x2003 is NOT incremented after a read, only writes
        //memCPU[0x2003] += 1;
        
        return _memSPRRAM[_memCPU[0x2003]];;
    }
    #pragma endregion

    #pragma region Read 0x2007
    // PPU Memory Data
    else if (address == 0x2007)
    {
        // Temporary variable so that VRAMAddr can be incremented without affecting the current read
        int VRAMAddrTemp = _VRAMAddr;

        // Determine Horizontal/Vertical write and increment according to $2000.2
        if ((_memCPU[0x2000] & 0x04) == 0x00)
        {
            _VRAMAddr++;
        }
        else if ((_memCPU[0x2000] & 0x04) == 0x04)
        {
            _VRAMAddr += 32;
        }

        // ***** Address Mirrors *****
        // If data is requested from 0000-2FFF
        if (VRAMAddrTemp <= 0x2FFF)
        {
            char temp = _prevByteRead;
            
            // If data is requested from 0000-1FFF
            if (VRAMAddrTemp <= 0x1FFF)
            {
                _prevByteRead = getMemoryMappedPPUByte(VRAMAddrTemp);
                return temp;
            }
            else
            {
            // ------------------------------------NEED TO FIX THIS FOR MASKING-----------------------------------------------
                #pragma region ***** (NAME TABLES) Mirrors  *****
                _prevByteRead = _memPPU[getMaskedAddrData(VRAMAddrTemp)];
                #pragma endregion
                return temp;
            }
        }

        // If data is requested from 3000-3EFF
        else if (VRAMAddrTemp <= 0x3EFF)
        {
            char temp = _prevByteRead;
            _prevByteRead = _memPPU[VRAMAddrTemp - 0x1000];
            return temp;
        }

        // Address Masking for 3F00-3FFF - Palette Mirrors (Masking)
        else if ((VRAMAddrTemp >= 0x3F00) && (VRAMAddrTemp <= 0x3FFF))
        {
            // "Mirroring" (Address Masking)
            if (VRAMAddrTemp > 0x3F1F)
            {
                //byteLastRead = memPPU[VRAMAddrTemp & 0x3F1F];
                return _memPPU[VRAMAddrTemp & 0x3F1F];
            }

            // -- Transparency address masking --
            // If palette memory is accessed, it is masked by 0x3F1F and placed into
            // the appropriate sprite or bg palette within 3F00-3F1F.  Also, Transparency
            // Bytes (like 3F10) are copied down (or up) to their respective BG/Sprite byte
            // So, a write to 3F10 would also write to 3F00 and a write to 3F04 would also
            // write to 3F14...and so on.

            return _memPPU[VRAMAddrTemp];
        }

        // Address Masking for 4000-FFFF
        else if (VRAMAddrTemp >= 0x4000 && VRAMAddrTemp <= 0xFFFF)
        {
            return _memPPU[VRAMAddrTemp & 0x3FFF];
        }

        // Return data from the PPU Memory located at address VRAMAddr
        return _memPPU[VRAMAddrTemp];
    }
    #pragma endregion

    else
    {
        //MessageBox.Show("[MEMORY.cs, getPPUMemoryByte()] PPU Address read to address not accounted for: " + String.Format("{0:x4}", addr));
        return 0x00;
    }
}
//    **********
//    Set PPU Memory
void MEM::setPPUByte(int address, unsigned char data)
{
    #pragma region Write 0x2000
    // PPU Control Register 1
    if (address == 0x2000)
    {
        // Copy to intended memory address
        _memCPU[address] = data;

        _t &= 0x73FF;
        _t |= ((int)(data & 0x03) << 10);
    }
    #pragma endregion

    #pragma region Write 0x2001
    // PPU Control Register 2
    else if (address == 0x2001)
    {
        #pragma region Set BG and Sprite Visibility
        //// Check to see if BG is turned on/off
        //if ((data & 0x08) == 0x08)
        //{ _ppu->bolDrawBG = true; }
        //else
        //{ _ppu->bolDrawBG = false; }

        //// Check to see if Sprites are turned on/off
        //if ((data & 0x10) == 0x10)
        //{ _ppu->bolDrawSP = true; }
        //else
        //{ _ppu->bolDrawSP = false; }

        // Check to see if BG Clipping is turned on (0) / off (1)
        if ((data & 0x02) == 0x02)
        { _ppu->setClipBackground(false); }
        else
        { _ppu->setClipBackground(true); }

        // Check to see if Sprite Clipping is turned on (0) / off (1)
        if ((data & 0x04) == 0x04)
        { _ppu->setClipSprites(false); }
        else
        { _ppu->setClipSprites(true); }
        #pragma endregion

        _memCPU[address] = data;
    }
    #pragma endregion

    #pragma region Write 0x2003
    // Sprite Memory Address
    else if (address == 0x2003)
    {
        // Write the address to 0x2003 so 0x2004 can access it
        _memCPU[address] = data;
    }
    #pragma endregion

    #pragma region Write 0x2004
    // Sprite Memory Data
    else if (address == 0x2004)
    {
        // Write the byte to 0x2004 first.  Don't know if this is necessary. May just be able to bypass this step and write directly to memSPRRAM
        _memSPRRAM[_memCPU[0x2003]] = data;

        // Increment Memory Address in 0x2003.  Mandatory incement after each WRITE of 0x2004
        _memCPU[0x2003] += 1;
    }
    #pragma endregion

    #pragma region Write 0x2005
    // Screen Scroll Offset
    else if (address == 0x2005)// && ((memCPU[0x2002] & 0x80) != 0x80))
    {
        if (!_PPULatchToggle)
        {
            // Copy byte into temporary location
            //VRAMAddrFirstWrite = data;

            // Offset X
            _t &= 0x7FE0;
            _t |= ((data & 0xF8) >> 3);

            // fineX is ONLY updated on the first write to 0x2005
            _fineX = (unsigned char)(data & 0x07);
        }
        else
        {
            // Add High Byte to the temporary VRAMAddr variable so 0x2007 has a full 2-byte address to read
            //VRAMAddr = data + (VRAMAddrFirstWrite & 0xFF) * 16 * 16;

            // Offset Y
            _t &= 0x0C1F;
            _t |= ((data & 0xF8) << 2) | ((data & 0x07) << 12);
            //t &= 0x0FFF;
            //t |= ((data & 0x07) << 12);
        }

        _PPULatchToggle = !_PPULatchToggle;
    }
    #pragma endregion

    #pragma region Write 0x2006
    // PPU Memory Address
    else if (address == 0x2006)   // Written to TWICE to read/write to/from PPU Memory
    {
        if (!_PPULatchToggle)
        {
            //// Increment 0x2006 Counter To Test for High Byte on Second Pass
            //PPULatchToggle = !PPULatchToggle;

            // Copy byte into temporary location
            // VRAMAddr = 0;
            //////////VRAMAddrFirstWrite = data;

            //if (((memCPU[0x2001] & 0x10) == 0x10) || ((memCPU[0x2001] & 0x08) == 0x08))
            //{
                #pragma region Scrolling Data
                // Get first 2006 write bits
                _t &= 0x00FF;
                _t |= ((data & 0x3F) << 8);

                // Clear top two bits
                //t &= 0x3FFF;
                #pragma endregion
            //}
        }
        else if (_PPULatchToggle)
        {
            //// Reset Counter
            //PPULatchToggle = !PPULatchToggle;

            // Add High Byte to the temporary VRAMAddr variable so 0x2007 has a full 2-byte address to read
            ///////////VRAMAddr = data + (VRAMAddrFirstWrite & 0xFF) * 16 * 16;

            //if (((memCPU[0x2001] & 0x10) == 0x10) || ((memCPU[0x2001] & 0x08) == 0x08))
            //{
                #pragma region Scrolling Data
                // Get second 2006 write bits
                _t &= 0x7F00;
                _t |= data;

                _VRAMAddr = _t;
                #pragma endregion
            //}
        }

        _PPULatchToggle = !_PPULatchToggle;
    }
    #pragma endregion

    #pragma region Write 0x2007
    // PPU Memory Data
    else if (address == 0x2007)   // Uses Memory Address in 2006 to read/write
    {
        // Write to Pattern Table (CHR on ROM)
        if (_VRAMAddr < 0x2000)
        {
            // This should never happen
            _memCHR[_VRAMAddr] = data;
        }

        // Address Masking for 2000-2FFF
        if ((_VRAMAddr >= 0x2000) && (_VRAMAddr < 0x3000))
        {
            // ------------------------------------NEED TO FIX THIS FOR MASKING-----------------------------------------------
            #pragma region ***** (NAME TABLES) Mirrors  *****
            _memPPU[getMaskedAddrData(_VRAMAddr)] = data;
            #pragma endregion
        }

        // Address Masking for 3000-3EFF
        else if ((_VRAMAddr >= 0x3000) && (_VRAMAddr <= 0x3EFF))
        {
            _memPPU[_VRAMAddr - 0x1000] = data;
        }

        // Address Masking for 3F00-3FFF - Palette Mirrors (Masking)
        else if ((_VRAMAddr >= 0x3F00) && (_VRAMAddr <= 0x3FFF))
        {
            // "Mirroring" (Address Masking)
            if (_VRAMAddr > 0x3F1F)
            {
                _memPPU[_VRAMAddr & 0x3F1F] = data;
            }

            // -- Transparency address masking --
            // If palette memory is accessed, it is masked by 0x3F1F and placed into
            // the appropriate sprite or bg palette within 3F00-3F1F.  Also, Transparency
            // Bytes (like 3F10) are copied down (or up) to their respective BG/Sprite byte
            // So, a write to 3F10 would also write to 3F00 and a write to3F04 would also
            // write to 3F14...and so on.
            if (_VRAMAddr < 0x3F10 && (_VRAMAddr % 4) == 0)
            {
                _memPPU[_VRAMAddr + 0x10] = data;  // Transparency Byte
            }
            else if (_VRAMAddr >= 0x3F10 && (_VRAMAddr % 4) == 0)
            {
                _memPPU[_VRAMAddr - 0x10] = data;  // Transparency Byte
            }
            _memPPU[_VRAMAddr] = data;
        }

        // Address Masking for 4000-FFFF
        else if (_VRAMAddr >= 0x4000 && _VRAMAddr <= 0xFFFF)
        {
            _memPPU[_VRAMAddr &= 0x3FFF] = data;
        }
        else
        {
            // Get address location from the temporary int that holds the data from two previous 0x2006 writes
            _memPPU[_VRAMAddr] = data;
            //MessageBox.Show("[Memory.cs: SetMemoryByte] Write to 2007");
        }

        // Determine Horizontal/Vertical write and increment according to $2000.2
        if ((_memCPU[0x2000] & 0x04) == 0x00)
        {
            _VRAMAddr++;
        }
        else if ((_memCPU[0x2000] & 0x04) == 0x04)
        {
            _VRAMAddr += 32;
        }
    }
    #pragma endregion

    // This should never happen    
    else
    {
        //MessageBox.Show("[PPU.cs, setPPUMemoryByte()] PPU Address read to address not accounted for: " + String.Format("{0:x4}", addr));
    }
}


//    **********
//    Get Memory Mapped CPU Byte
unsigned char MEM::getMemoryMappedCPUByte(int address)
{
    int addrTemp = address - 0x8000;
    unsigned char loc_one = ((addrTemp & 0xF000) >> 12) & 0xFF;
    unsigned char loc_two = ((addrTemp & 0x0F00) >> 8) & 0xFF;
    unsigned char offset = (addrTemp & 0x00FF) & 0xFF;

    unsigned char *returnTempPrt = _mapperPRGPtr[loc_one][loc_two] + offset;
    unsigned char returnTemp = *returnTempPrt;

    return returnTemp;
}

//    **********
//    Set Memory Mapped CPU Byte
void MEM::setMemoryMappedCPUByte(int address, unsigned char data)
{
#if 1
    switch (_mapper_num)
    {
        case 00:
            {
                // Do nothing, just write to address
                _memPRG[address - 0x8000] = data;
            }
            break;

        case 02:
        case 66:
            {
                int offset = (data & (_num_PRG_banks - 1)) * 0x4000;
                // Copy 0x4000 + offset Pointers to mapperPRGPtr
                for(int i = 0; i < 4; i++)
                {
                    for(int j = 0; j < 16; j++)
                    {
                        _mapperPRGPtr[i][j] = &_memPRG[i * 0x1000 + j * 0x0100 + offset];
                    }
                }
            }
            break;

        case 03:
        case 67:
        case 64:
            {
                int offset = (data & (_num_CHR_banks - 1)) * 0x2000;
                // Copy 0x2000 + offset Pointers to mapperCHRPtr
                for(int i = 0; i < 2; i++)
                {
                    for(int j = 0; j < 16; j++)
                    {
                        _mapperCHRPtr[i][j] = &_memCHR[i * 0x1000 + j * 0x0100 + offset];
                    }
                }
            }
            break;

        default:
            {
                // If unsupported mapper
                //tn->resetSystem();
            }
            break;
    }
#endif
}

//    **********
//    Get Memory Mapped PPU Byte
unsigned char MEM::getMemoryMappedPPUByte(int address)
{
    unsigned char loc_one = ((address & 0x1000) >> 12) & 0xFF;
    unsigned char loc_two = ((address & 0x0F00) >> 8) & 0xFF;
    unsigned char offset = (address & 0x00FF) & 0xFF;

    unsigned char *return_temp_ptr = _mapperCHRPtr[loc_one][loc_two] + offset;
    unsigned char return_temp = *return_temp_ptr;

    return return_temp;
}

//    **********
//    Set Memory Mapped PPU Byte
void MEM::setMemoryMappedPPUByte(int address, unsigned char data)
{

}

//    **********
//    Get PPU Memory for External Classes
unsigned char MEM::getPPU(int address)
{
    if(address < 0x2000)
    {
        return getMemoryMappedPPUByte(address);
    }
    else
    {
        return _memPPU[address];
    }
}

//    **********
//    Get Address Masked Mirror Data
int MEM::getMaskedAddrData(int nametable_address)
{
    if(nametable_address < 0x2000)
    {
        return getMemoryMappedPPUByte(nametable_address);
    }
    // Horizontal Name Table Mirroring
    //if (byteMirror == 0x00 && (ntaddr >= 0x2000) && (ntaddr < 0x2400))
    //{
    //    return ntaddrTemp;
    //}
    if (_horiz_vert_mirror == 0x00 && (nametable_address >= 0x2400) && (nametable_address < 0x2800))
    {
        return nametable_address & 0x23FF;
    }
    //else if (byteMirror == 0x00 && (ntaddr >= 0x2800) && (ntaddr < 0x2C00))
    //{
    //    return ntaddrTemp;
    //}
    else if (_horiz_vert_mirror == 0x00 && (nametable_address >= 0x2C00) && (nametable_address < 0x3000))
    {
        return nametable_address & 0x2BFF;
    }

    // Vertical Name Table Mirroring
    //else if (byteMirror == 0x01 && (ntaddr >= 0x2000) && (ntaddr < 0x2400))
    //{
    //    return ntaddrTemp;
    //}
    //else if (byteMirror == 0x01 && (ntaddr >= 0x2400) && (ntaddr < 0x2800))
    //{
    //    return ntaddrTemp;
    //}
    else if (_horiz_vert_mirror == 0x01 && (nametable_address >= 0x2800) && (nametable_address < 0x2C00))
    {
        return nametable_address - 0x0800;
    }
    else if (_horiz_vert_mirror == 0x01 && (nametable_address >= 0x2C00) && (nametable_address < 0x3000))
    {
        return nametable_address - 0x0800;
    }

    return nametable_address;
}

//    **********
//  VBlank Set/Clear
void MEM::setVBlank(bool vblank_status)
{
    if (vblank_status)
    {
        _memCPU[0x2002] |= 0x80;
    }
    else
    {
        _memCPU[0x2002] &= 0x7F;
    }
}

//    **********
//  Sprite Hit Set/Clear
void MEM::setSpriteHit(bool sprite_hit_status)
{
    if (sprite_hit_status)
    {
        _memCPU[0x2002] |= 0x40;
    }
    else
    {
        _memCPU[0x2002] &= 0xBF;
    }
}

//    **********
//    Rest MEM Variables
void MEM::reset()
{
    // Initialize all char arrays to 0 so they don't have garbage data
    memset(_memSPRRAM, 0, sizeof(_memSPRRAM));
    memset(_memCPU, 0, sizeof(_memCPU));
    memset(_memPPU, 0, sizeof(_memPPU));

    // Set PPU Latch to false
    _PPULatchToggle = false;
    _t = 0;
    _fineX = 0;

    _VRAMAddr = 0;
    _prevByteRead = 0;
}
