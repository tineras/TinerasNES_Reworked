#include "tinerasnes.h"
#include "main_window.h"

#include "cpu.h"
#include "mem.h"
#include "ppu.h"
#include "apu.h"
#include "nes_input.h"

TinerasNES::TinerasNES() :
    _cpu(nullptr),
    _mem(nullptr),
    _ppu(nullptr),
    _apu(nullptr),
    _nes_input(nullptr),
    _running(false),
    _drawing_frame(false),
    _quit(false),
    _draw_buffer(nullptr)
{
    initSDL();
}

TinerasNES::~TinerasNES()
{

}

void TinerasNES::init()
{
    resetAll();

    _cpu = new CPU();
    _mem = new MEM(this);
    _ppu = new PPU(this);
    _apu = new APU();
    _nes_input = new NES_INPUT();

    _cpu->init(_mem);
    _mem->init(_ppu, _apu, _nes_input);
    _ppu->init(_cpu, _mem);
    _apu->init(_cpu);

    // Enable Audio
    _apu->setEnabled(true);
}

void TinerasNES::resetAll()
{
    if (_cpu != nullptr)
        delete _cpu;

    if (_mem != nullptr)
        delete _mem;

    if (_ppu != nullptr)
        delete _ppu;

    if (_apu != nullptr)
        delete _apu;

    if (_nes_input != nullptr)
        delete _nes_input;

    _cpu = nullptr;
    _mem = nullptr;
    _ppu = nullptr;
    _apu = nullptr;
    _nes_input = nullptr;

    _running = false;
    _drawing_frame = false;
    _quit = false;

    _master_cpu_cycle = 0;
    _current_cpu_cycle = 0;
    _apu_frame_count = 0;
    _draw_buffer = nullptr;

    _frame_timer.restart();
}

void TinerasNES::initSDL()
{
    // https://wiki.libsdl.org/SDL_SetMainReady
    SDL_SetMainReady();
    SDL_Init(SDL_INIT_EVERYTHING);
}

void TinerasNES::idle()
{
    while (!_quit)
    {
        if (!_running)
        {
            QThread::msleep(50);
        }
        else if (_running)
        {
            run();
        }
    }
}

void TinerasNES::run()
{
    _draw_buffer = _ppu->frameBuffer;

    _frame_timer.start();

    while(_running)
    {
        if (_drawing_frame)
            continue;

        // Run CPU - Execute Opcode
        int elapsed_cpu_cycles = _cpu->execOpCode();
        _current_cpu_cycle += elapsed_cpu_cycles * 15;

        // Run APU
        _apu_frame_count += elapsed_cpu_cycles;
        if(_apu_frame_count >= 40) // Renders a frame roughly 44100 times per second
        {
            _apu->renderFrame();

            // TODO: Check...What in the world am I doing here??
            _apu_frame_count -= 40;
        }

        // Run PPU
        _ppu->run(_current_cpu_cycle);

        // Ready to render
        if(_ppu->readyToRender())
        {
            while (_frame_timer.elapsed() < k_frame_time) {}
            _frame_timer.restart();

            {
                // TODO: HACK...FIX THIS (SETTING BG Color)
                // Should be respecting BG transparency instead
                for (int i = 0; i < (256 * 240 * 4); i+=4)
                {
                    if (_ppu->frameBuffer[i + 3] == 0)
                    {
                        _ppu->frameBuffer[i]     = _ppu->BGColorR;
                        _ppu->frameBuffer[i + 1] = _ppu->BGColorG;
                        _ppu->frameBuffer[i + 2] = _ppu->BGColorB;
                        _ppu->frameBuffer[i + 3] = 255;
                    }
                }

                _drawing_frame = true;
                emit repaintGLWidget();

                // Reset CPU Cycle
                _current_cpu_cycle = 0;
                _ppu->setReadyToRender(false);
            }
        }
    }
}

void TinerasNES::onKeyPressEvent(unsigned char button)
{
    _nes_input->pressButton(button);
}

void TinerasNES::onKeyReleaseEvent(unsigned char button)
{
    _nes_input->releaseButton(button);
}

void TinerasNES::openFile(QString filename)
{
    init();

    if (filename.isEmpty())
        return;

    //--------------------------------------------------------------//
    // Courtesy of NEStreme Emu
    // Name: LoadROMToMem()
    // Desc: Loads the ROM into memory for use. The .NES file is 
    //       structure as follow (copied from Yoshi's nes doc.)
    //
    //    +--------+------+------------------------------------------+
    //    | Offset | Size | Content(s)                               |
    //    +--------+------+------------------------------------------+
    //    |   0    |  3   | 'NES'                                    |
    //    |   3    |  1   | $1A                                      |
    //    |   4    |  1   | 16K PRG-ROM page count                   |
    //    |   5    |  1   | 8K CHR-ROM page count                    |
    //    |   6    |  1   | ROM Control Byte #1                      |
    //    |        |      |   %####vTsM                              |
    //    |        |      |    |  ||||+- 0=Horizontal mirroring      |
    //    |        |      |    |  ||||   1=Vertical mirroring        |
    //    |        |      |    |  |||+-- 1=SRAM enabled              |
    //    |        |      |    |  ||+--- 1=512-byte trainer present  |
    //    |        |      |    |  |+---- 1=Four-screen mirroring     |
    //    |        |      |    |  |                                  |
    //    |        |      |    +--+----- Mapper # (lower 4-bits)     |
    //    |   7    |  1   | ROM Control Byte #2                      |
    //    |        |      |   %####0000                              |
    //    |        |      |    |  |                                  |
    //    |        |      |    +--+----- Mapper # (upper 4-bits)     |
    //    |  8-15  |  8   | $00                                      |
    //    | 16-..  |      | Actual 16K PRG-ROM pages (in linear      |
    //    |  ...   |      | order). If a trainer exists, it precedes |
    //    |  ...   |      | the first PRG-ROM page.                  |
    //    | ..-EOF |      | CHR-ROM pages (in ascending order).      |
    //    +--------+------+------------------------------------------+
    //
    //--------------------------------------------------------------//
    //--------------------------------------------------------------// 

    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly))
        return;

    int index = 0;
    QByteArray q_rom_blob = file.readAll();
    char* rom_blob = q_rom_blob.data();

    // (FIX) Variable to read in from cart
    char header[16];
    memset(header, 0, sizeof(header));

    // Allocate chars for header
    char h_num_PRG_banks;       // Number of PRG Banks
    char h_num_CHR_banks;       // Number of CHR Banks
    // Control Byte 1
    char h_rom_control_byte_1;
    char h_mirroring;           // Horizontal(0)/Vertical(1) Mirroring
    char h_sram_enabled;        // SRAM Enabled
    char h_trainer_present;     // 512 byte Trainer present
    char h_v;                   // Four screen Mirroring (??? LOOK ME UP)
    char h_mapper_num_lsb;      // 4 LSB of Mapper #
    // Control Byte 2
    char h_rom_control_byte_2;
    char h_mapper_num_msb;      // 4 MSB of Mapper #

    // -- Begin Reading Data --

    // Read header (16 bytes)
    memcpy(header, &rom_blob[index], 16);
    index += 16;

    h_num_PRG_banks = header[4];
    h_num_CHR_banks = header[5];
    _mem->setCHRPRGBanks(h_num_CHR_banks, h_num_PRG_banks);

    // Get control byte info (includes mapper)
    h_rom_control_byte_1 = header[6];
    h_mirroring = h_rom_control_byte_1 & 0x01;
    _mem->setHorizontalVerticalMirrorBit(h_mirroring);
    h_sram_enabled = (h_rom_control_byte_1 & 0x02) >> 1;
    h_trainer_present = (h_rom_control_byte_1 & 0x04) >> 2;
    h_v = (h_rom_control_byte_1 & 0x08) >> 3;
    h_mapper_num_lsb = (h_rom_control_byte_1 & 0xF0) >> 4;
    h_rom_control_byte_2 = header[7];
    h_mapper_num_msb = (h_rom_control_byte_2 & 0xF0);

    _mem->setMapperNumber(h_mapper_num_msb | h_mapper_num_lsb);

    // Read 512 bytes of trainer if present
    char trainer[512];
    if (h_trainer_present)
    {
        memcpy(trainer, &rom_blob[index], 512);
        index += 512;
    }

    int size_CHR = h_num_CHR_banks * 0x2000;
    int size_PRG = h_num_PRG_banks * 0x4000;
    if (h_num_PRG_banks == 1) // TODO: Check...Why am I doing this?
        size_PRG = 0x8000;

    _mem->initCHRPRG(size_CHR, size_PRG);

    _mem->readPRG(&rom_blob[index], h_num_PRG_banks * 0x4000);
    index += h_num_PRG_banks * 0x4000;
    if (size_CHR > 0)
    {
        _mem->readCHR(&rom_blob[index], size_CHR);
        index += size_CHR;
    }

    int offset = (h_num_PRG_banks - 1) * 0x4000;
    _mem->initPRGMapperPtrs(offset);

    // Set Program Counter for NESTest CPU-only debugging
    //cpu->setPC(0x4000);

    // Set Program Counter for Normal Operation
    _cpu->setPC(_mem->getMemoryMappedCPUByte(0xFFFC) + (_mem->getMemoryMappedCPUByte(0xFFFD) << 8));

    // Set Joypad Values
    _mem->setMemCPU(0x4016, 0x40);
    _mem->setMemCPU(0x4017, 0x40);

    _running = true;
}

void TinerasNES::quit()
{
    _running = false;
    _quit = true;
}
