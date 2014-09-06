/*    APU.cpp
    Author: Aaron Wiginton

    Description: APU for TinerasNES.
*/

#ifndef _APU_HEADER
#define _APU_HEADER

#include "common.h"

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

    void updateEnvelope(); 
    void updateSweep();
    void checkSweepSilence();
    void updateLengthCounter();

    void renderFrame(int);
    void APU_play();
    void APU_pause();

    // Reset APU Variables
    void reset();

    void apu_callback(Uint8*, int);

    /**** DEBUG ****/
    unsigned int sampleCounter_W;
    unsigned int sampleCounter_R;

    bool regUpdated;
    unsigned char regUpdateValues[4];
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

    unsigned char frameNTSC_PAL;// 4 or 5 Frame Count
    unsigned int frameCounter;    // Frame Counter
    bool frameIRQEnabled;
    bool dmcIRQEnabled;

    char outputSample;

    bool dmcEnable;        // DMC Enable
    bool noiseEnable;    // Noise Enable
    bool triEnable;        // Triangle Enable
    bool rect2Enable;    // Rectangle/Pulse 2 Enable
    bool rect1Enable;    // Rectangle/Pulse 1 Enable

    bool frameIRQPending;
    bool IRQNextTime;

private:
    void init_audio();    // Init APU

    // File stream for nestest file
    ofstream apuDebugFileStream;

    APU_noise* _noise;
    APU_rect1* _rect1;
    APU_rect2* _rect2;
    APU_triangle* _triangle;

    CPU* _cpu;
};
#endif //_APU_HEADER