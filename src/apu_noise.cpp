/*    APU_noise->cpp
    Author: Aaron Wiginton

    Description: APU Noise for TinerasNES.
*/ 

/* ************* APU REGISTERS **************

    **** Noise ****

    $400C   --lv vvvv   (l) loop env/disable length
                        (vvvv) vol/env period
    $400D    (unused)
    $400E   s--- pppp   (s) short/tone mode
                        (pppp) period index/playback sample rate
    $400F   llll l---   (l) length index

   ************* APU REGISTERS ************** */


#include "APU_noise.h"

const unsigned char APU_noise::lengthTable[] = { 10,254, 20,  2, 40,  4, 80,  6, 160,  8, 60, 10, 14, 12, 26, 14,
                                                 12, 16, 24, 18, 48, 20, 96, 22, 192, 24, 72, 26, 16, 28, 32, 30};

const unsigned int APU_noise::periodTable_NTSC[] = { 4, 8, 16, 32, 64, 96, 128, 160, 202, 254, 380, 508, 762, 1016, 2034, 4068};
const unsigned int APU_noise::periodTable_PAL[] = { 4, 7, 14, 30, 60, 88, 118, 148, 188, 236, 354, 472, 708, 944, 1890, 3778};

//    **********
//    APU Noise Render Sample
char APU_noise::renderSample()
{
    char OUT = 0;
    //updateFrequency();
    if(length > 0)
    {
        sampleCount++;

        if(sampleCount >= samplesPerPeriod)
        {
            sampleCount = 0;

            if(mode == 0)
                xorResult = (sr & 0x01) ^ ((sr >> 1) & 0x01);
            else
                xorResult = (sr & 0x01) ^ ((sr >> 6) & 0x01);

            sr = (xorResult << 14) | ((sr >> 1) & 0x7FFF);
        }

        if((sr & 0x01) == 0)
        {
            if(decayDisable)
                OUT = volume;
            else
                OUT = envelope;
        }
    }
    return OUT;
}

//    **********
//    APU Noise Update Envelope
void APU_noise::updateEnvelope()
{
    if(decayReset)
    {
        decayCount = decayEnvVol;
        envelope = 0x0F;
        decayReset = false;

        if(!decayDisable)
        {    volume = 0x0F;    }
    }
    else
    {
        if(decayCount > 0)
        {
            decayCount--;
        }
        else
        {
            decayCount = decayEnvVol;

            if(envelope > 0)
                envelope--;
            else if(decayLoopEnable)
                envelope = 0x0F;

            if(!decayDisable)
                volume = envelope;
        }
    }
}

//    **********
//    APU Noise Check Sweep Silence and Update Frequency
void APU_noise::updateFrequency()
{
    double freq = 1790000 / 2 / (period + 1);
    samplesPerPeriod = (unsigned int)(44100 / freq);
}

//    **********
//    APU Noise Update Length Counter
void APU_noise::updateLengthCounter()
{
    if(!decayLoopEnable && length > 0)
    {    length--;    }
}

//    **********
//    Init APU
void APU_noise::init()
{
    sr = 1;

    volume = 0;
    envelope = 0;
    decayCount = 0;

    sampleCount = 0;
    samplesPerPeriod = 0;

    // Noise
    decayReset = false;

    // Rect 1 - 0x400C
    decayLoopEnable = false;    // Halt length counter
    decayDisable = false;        // Decay/Volume Flag
    decayEnvVol = 0;            // Decay Time/Volume

    // Rect 1 - 0x400D

    // Rect 1 - 0x400E
    period = 0;                    // Period

    // Rect 1 - 0x400F
    length = 0;                    // Length Index
}

//    **********
//    Rest APU Variables
void APU_noise::reset()
{
    init();
}

//    **********
//    APU Constructor
APU_noise::APU_noise()
{
    reset();
}

//    **********
//    APU Destructor
APU_noise::~APU_noise()
{

}