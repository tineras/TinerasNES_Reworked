/*    APU_rect2.h
    Author: Aaron Wiginton

    Description: APU Rectangle 2 for TinerasNES.
*/

#ifndef _APU_rect2_HEADER
#define _APU_rect2_HEADER

using namespace std;

class APU_rect2
{
public:
    const static unsigned char lengthTable[];

    const static unsigned char duty0[];
    const static unsigned char duty1[];
    const static unsigned char duty2[];
    const static unsigned char duty3[];

    char volume;
    char envelope;
    char decayCount;

    bool decayReset;

    // Samples Per Period Reset
    double samplesPerPeriod;
    double samplesPerPeriodPos;

    bool sweepSilence;
    bool sweepReset;
    unsigned char sweepCount;

    // Rect 2 - 0x4000
    unsigned char dutyType;        // Duty Type 0-3
    bool decayLoopEnable;        // Halt length counter
    bool decayDisable;            // Decay/Volume Flag
    unsigned char decayEnvVol;    // Decay Time/Volume

    // Rect 2 - 0x4001
    bool sweepEnable;            // Sweep Enable Flag
    unsigned char sweepRate;    // Sweep Rate
    bool sweepNeg;                // Negate Flag / decrease/increase (1/0) wavelength
    unsigned char sweepShift;    // Shift Count / Right Shift Amount

    // Rect 2 - 0x4002/0x4003
    int period;                    // Period

    // Rect 2 - 0x4003
    unsigned char length;        // Length Index

    void updateEnvelope(); 
    void updateSweep();
    void checkSweepSilence();
    void updateLengthCounter();

    APU_rect2();
    ~APU_rect2();

    char renderSample();

    // Reset APU Variables
    void reset();

private:

    void init();    // Init APU
};
#endif //_APU_rect2_HEADER