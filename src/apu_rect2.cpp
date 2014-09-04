/*    APU_rect2->cpp
    Author: Aaron Wiginton

    Description: APU Rectangle 2 for TinerasNES.
*/ 

/* ************* APU REGISTERS **************

    **** Square 1/Square 2 ****

    $4000/4 ddle vvvv   (dd) duty cycle type
                                00=12.5%    01000000
                                01=25%        01100000
                                10=50%        01111000
                                11=75%        10011111
                        (l) length counter clock disable (halt) / env decay looping enable
                        (e) env decay disable/constant volume flag (0 - decay, 1 - constant volume)
                        (vvvv)  Constant Volume Mode: 0000=Silence, 1111=Maximum
                                Constant Decay Mode: Decay time (divider's period is set to N + 1)
    $4001/5 eppp nsss   (e) enable sweep
                        (ppp) period/sweep update rate (divider's period is set to P + 1)
                        (n) Negate Flag / decrease/increase (1/0) wavelength
                        (sss) Shift Count / Right Shift Amount
    $4002/6 pppp pppp   (pppp pppp) Period Low (8 LSB of wavelength)
    $4003/7 llll lppp   (llll l) Length Index
                        (ppp) Period High

   ************* APU REGISTERS ************** */


#include "APU_rect2.h"

const unsigned char APU_rect2::lengthTable[] = { 10,254, 20,  2, 40,  4, 80,  6, 160,  8, 60, 10, 14, 12, 26, 14,
                                                 12, 16, 24, 18, 48, 20, 96, 22, 192, 24, 72, 26, 16, 28, 32, 30    };

const unsigned char APU_rect2::duty0[] = { 0, 1, 0, 0, 0, 0, 0, 0, };
const unsigned char APU_rect2::duty1[] = { 0, 1, 1, 0, 0, 0, 0, 0, };
const unsigned char APU_rect2::duty2[] = { 0, 1, 1, 1, 1, 0, 0, 0, };
const unsigned char APU_rect2::duty3[] = { 1, 0, 0, 1, 1, 1, 1, 1, };

//    **********
//    APU Rectangle 2 Render Sample
char APU_rect2::renderSample()
{
    if(length > 0 && !sweepSilence)
    {
        int dutyPos = (unsigned char)((samplesPerPeriodPos++ / samplesPerPeriod) * 8);

        if(samplesPerPeriodPos >= samplesPerPeriod)    
            samplesPerPeriodPos = 0;

        char pos_neg = 0;

        switch (dutyType)
        {
            case 0:
                if (duty0[dutyPos]) pos_neg = 1;
                else pos_neg = 0;
                break;
            case 1:
                if (duty1[dutyPos]) pos_neg = 1;
                else pos_neg = 0;
                break;
            case 2:
                if (duty2[dutyPos]) pos_neg = 1;
                else pos_neg = 0;
                break;
            case 3:
                if (duty3[dutyPos]) pos_neg = 1;
                else pos_neg = 0;
                break;
        }
        
        if(decayDisable)
        {
            return volume * pos_neg;
        }
        else
        {
            return envelope * pos_neg;
        }
    }
    else
    {
        return 0x00;
    }
}

//    **********
//    APU Rectangle 2 Update Envelope
void APU_rect2::updateEnvelope()
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
//    APU Rectangle 2 Update Sweep
void APU_rect2::updateSweep()
{
    if(sweepEnable && !sweepSilence)
    {
        if(sweepCount > 0)
        {
            sweepCount--;
        }
        else
        {
            sweepCount = sweepRate;
            if(!sweepNeg)    // Then sweep forward
            {
                period += period >> sweepShift;
            }
            else if(sweepNeg)    // Sweep backward (Should not sweep below 0
            {
                period -= (period >> sweepShift);
            }

            checkSweepSilence();
        }
    }
    if(sweepReset)
    {
        sweepReset = false;
        sweepCount = sweepRate;
    }
}

//    **********
//    APU Rectangle 2 Check Sweep Silence and Update Frequency
void APU_rect2::checkSweepSilence()
{
    sweepSilence = false;
    if(period < 8 || (!sweepNeg && period > 0x7FF))
    {
        sweepSilence = true;
    }
    if(!sweepSilence)    // Calculate new samples per period
    {
        double freq = 1790000 / 16 / (period + 1);
        samplesPerPeriod = (unsigned int)(44100 / freq);
    }
}

//    **********
//    APU Rectangle 2 Update Length Counter
void APU_rect2::updateLengthCounter()
{
    if(!decayLoopEnable && length > 0)
    {    length--;    }
}

//    **********
//    Init APU
void APU_rect2::init()
{
    volume = 0;
    envelope = 0;
    decayCount = 0;

    samplesPerPeriod = 0;
    samplesPerPeriodPos = 0;

    sweepSilence = false;
    sweepReset = false;
    sweepCount = 0;

    // Rectangle 1
    decayReset = false;

        // Rect 1 - 0x4000
        dutyType = 0;// Duty Type 0-3
        decayLoopEnable = false;            // Halt length counter
        decayDisable = false;    // Decay/Volume Flag
        decayEnvVol = 0;    // Decay Time/Volume
        // Rect 1 - 0x4001
        sweepEnable = false;    // Sweep Enable Flag
        sweepRate = 0;    // 
        sweepNeg = false;    // Negate Flag / decrease/increase (1/0) wavelength
        sweepShift = 0;    // Shift Count / Right Shift Amount
        // Rect 1 - 0x4002/0x4003
        period = 0;        // Period
        // Rect 1 - 0x4003
        length = 0;        // Length Index        
}

//    **********
//    Rest APU Variables
void APU_rect2::reset()
{
    init();
}

//    **********
//    APU Constructor
APU_rect2::APU_rect2()
{
    reset();
}

//    **********
//    APU Destructor
APU_rect2::~APU_rect2()
{

}