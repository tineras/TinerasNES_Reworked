/*    APU_triangle.cpp
    Author: Aaron Wiginton

    Description: APU Triangle for TinerasNES.
*/ 

/* ************* APU REGISTERS **************

    **** Triangle ****

    $4008   clll llll   (c) linear counter start/length counter clock disable
                        (lll llll) linear counter load
    $4009    (unused)            
    $400A   pppp pppp   (pppp pppp) period low (8 LSB of wavelength)
    $400B   llll lppp   (llll l) length index
                        (ppp) period high

   ************* APU REGISTERS ************** */


#include "APU_triangle.h"

const unsigned char APU_triangle::lengthTable[] = { 10,254, 20,  2, 40,  4, 80,  6, 160,  8, 60, 10, 14, 12, 26, 14,
                                                    12, 16, 24, 18, 48, 20, 96, 22, 192, 24, 72, 26, 16, 28, 32, 30};
const unsigned char APU_triangle::triangleSequence[] = { 0xF, 0xE, 0xD, 0xC, 0xB, 0xA, 0x9, 0x8, 0x7, 0x6, 0x5, 0x4, 0x3, 0x2, 0x1, 0x0,
                                                         0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF};

//    **********
//    APU Triangle Render Sample
char APU_triangle::renderSample()
{
    if(length > 0 && linearCountTemp > 0 && period >= 8)
    {
        volume = triangleSequence[(unsigned int)(seqPos) & 0x1F];

        seqPos += (double)((1 / (double)samplesPerPeriod));

        if(seqPos >= 32)
            seqPos = 0;

        return volume;
    }
    return 0x00;
}

//    **********
//    APU Triangle Update Envelope
void APU_triangle::updateLinearCounter()
{
    if(linearHalt)
    {
        linearCountTemp = linearCount;
    }
    else if (linearCountTemp > 0)
    {
        linearCountTemp--;
    }

    if(!linearCounterDisable)
        linearHalt = false;
}

//    **********
//    APU Triangle Update Frequency
void APU_triangle::updateFrequency()
{
    double freq = 1790000 / (period + 1);
    samplesPerPeriod = (double)(44100 / freq);
}

//    **********
//    APU Triangle Update Length Counter
void APU_triangle::updateLengthCounter()
{
    if(!linearCounterDisable && length > 0)
    {    length--;    }
}

//    **********
//    Init APU
void APU_triangle::init()
{
    ///* Allocate a desired SDL_AudioSpec */
    //audioSpecDesired = malloc(sizeof(SDL_AudioSpec));
    ///* Allocate space for the obtained SDL_AudioSpec */
    //audioSpecObtained = malloc(sizeof(SDL_AudioSpec));

    samplesPerPeriod = 0;
    seqPos = 0;
    volume = 0;

    linearCountTemp = 0;
    linearHalt = false;

    // Triangle
        // Triangle - 0x4008
        linearCounterDisable = false;// Halt length counter
        linearCount = 0;            // Duty Type 0-3

        // Triangle - 0x4009
        /* Unused */

        // Triangle - 0x400A
        period = 0;                    // Period

        // Triangle - 0x400B
        length = 0;                    // Length Index
}

//    **********
//    Rest APU Variables
void APU_triangle::reset()
{
    init();
}

//    **********
//    APU Constructor
APU_triangle::APU_triangle()
{
    reset();
}

//    **********
//    APU Destructor
APU_triangle::~APU_triangle()
{

}