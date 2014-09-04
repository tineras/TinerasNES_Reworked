/*    APU_noise.h
    Author: Aaron Wiginton

    Description: APU Noise for TinerasNES.
*/

#ifndef _APU_NOISE_HEADER
#define _APU_NOISE_HEADER

//using namespace std;

class APU_noise
{
public:
    const static unsigned char lengthTable[];
    const static unsigned int periodTable_NTSC[];
    const static unsigned int periodTable_PAL[];

    unsigned short sr;

    char volume;
    char envelope;
    char decayCount;

    bool decayReset;

    // Samples Per Period Reset
    unsigned int samplesPerPeriod;

    unsigned int sampleCount;
    unsigned char xorResult;

    // Noise - 0x400C
    bool decayLoopEnable;        // Halt length counter
    bool decayDisable;            // Decay/Volume Flag
    unsigned char decayEnvVol;    // Decay Time/Volume

    // Noise - 0x400D

    // Noise - 0x400E
    unsigned char mode;            // Mode
    int period;                    // Period

    // Noise - 0x400F
    unsigned char length;        // Length Index

    void updateEnvelope(); 
    void updateFrequency();
    void updateLengthCounter();

    APU_noise();
    ~APU_noise();

    char renderSample();

    // Reset APU Variables
    void reset();

private:

    void init();    // Init APU
};
#endif //_APU_NOISE_HEADER