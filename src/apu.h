/*    APU.cpp
    Author: Aaron Wiginton

    Description: APU for TinerasNES.
*/

#ifndef _APU_HEADER
#define _APU_HEADER

#include "common.h"

#include "SDL_audio.h"

#include "apu_noise.h"
#include "apu_rect1.h"
#include "apu_rect2.h"
#include "apu_triangle.h"

typedef unsigned char Uint8;

class APU_noise;
class APU_rect1;
class APU_rect2;
class APU_triangle;
class CPU;

class APU
{
public:
    APU();
    ~APU();

    void init(CPU* cpu);

    APU_noise* noise() { return _noise; }
    APU_rect1* rect1() { return _rect1; }
    APU_rect2* rect2() { return _rect2; }
    APU_triangle* triangle() { return _triangle; }

    inline bool dmcIRQEnabled() { return _dmc_IRQ_enabled; }
    inline bool frameIRQEnabled() { return _frame_IRQ_enabled; }
    inline void setFrameIRQEnabled(bool frame_IRQ_enabled) { _frame_IRQ_enabled = frame_IRQ_enabled; }

    //inline bool DMCEnabled() { return _dmc_enabled; }
    inline bool triangleEnabled() { return _triangle_enabled; }
    inline bool rectangle2Enabled() { return _rectangle_2_enabled; }
    inline bool rectangle1Enabled() { return _rectangle_1_enabled; }

    inline void setDMCEnabled(bool dmc_enabled) { _dmc_enabled = dmc_enabled; }
    inline void setNoiseEnabled(bool noise_enabled) { _noise_enabled = noise_enabled; }
    inline void setTrianlgeEnabled(bool triangle_enabled) { _triangle_enabled = triangle_enabled; }
    inline void setRectangle1Enabled(bool rectangle_1_enabled) { _rectangle_1_enabled = rectangle_1_enabled; }
    inline void setRectangle2Enabled(bool rectangle_2_enabled) { _rectangle_2_enabled = rectangle_2_enabled; }

    inline unsigned char frameNTSCPAL() { return _frame_NTSC_PAL; }
    inline void setFrameNTSCPAL(unsigned char frame_NTSC_PAL) { _frame_NTSC_PAL = frame_NTSC_PAL; }

    inline void setFrameCounter(int frame_counter) { _frame_counter = 0; }

    // DEBUG
    inline bool regUpdated() { return _reg_updated; }
    inline void setRegUpdated(bool reg_updated) { _reg_updated = reg_updated; }
    void setRegUpdateValue(int index, unsigned char value) { _reg_update_values[index] = value; }
    // DEBUG

    void updateEnvelope(); 
    void updateSweep();
    void checkSweepSilence();
    void updateLengthCounter();

    void renderFrame();
    void play();
    void pause();

    // Reset APU Variables
    void reset();

    void apu_callback(Uint8* stream, int len);

private:
    void init_audio();    // Init APU

    /**** DEBUG ****/
    unsigned int sampleCounter_W;
    unsigned int sampleCounter_R;

    bool _reg_updated;
    unsigned char _reg_update_values[4];
    /***************/

    const static int frameUpdate[2][5];

    bool ch1Enable;
    bool ch2Enable;
    bool ch3Enable;
    bool ch4Enable;
    bool ch5Enable;

    int prevWrite;
    bool WriteBufferReady;

    unsigned int apuCyclesTotal;

    unsigned char apuPlayBuffer[1024];
    unsigned char apuWriteBuffer[65536];
    unsigned int apuPlayBufferPos;
    unsigned int apuWriteBufferPos;
    unsigned int apuWriteBufferPos_Last;
    unsigned short apuBufferPos;
    unsigned int apuBufferLen;
    bool apuPlayBufferEmpty;
    bool apuWriteBuffer_FirstFullWriteComplete;

    unsigned int dmcFlag;        // DMC IRG Flag Length
    unsigned int frameFlag;        // Frame Interrupt Flag Length

    unsigned char _frame_NTSC_PAL;// 4 or 5 Frame Count
    unsigned int _frame_counter;    // Frame Counter
    bool _frame_IRQ_enabled;
    bool _dmc_IRQ_enabled;

    char outputSample;

    bool _dmc_enabled;      // DMC Enable
    bool _noise_enabled;    // Noise Enable
    bool _triangle_enabled;      // Triangle Enable
    bool _rectangle_1_enabled;    // Rectangle/Pulse 1 Enable
    bool _rectangle_2_enabled;    // Rectangle/Pulse 2 Enable

    bool frameIRQPending;
    bool IRQNextTime;

    SDL_AudioSpec audioSpecDesired, audioSpecObtained;

    // File stream for nestest file
    ofstream apuDebugFileStream;

    APU_noise* _noise;
    APU_rect1* _rect1;
    APU_rect2* _rect2;
    APU_triangle* _triangle;

    CPU* _cpu;
};
#endif //_APU_HEADER