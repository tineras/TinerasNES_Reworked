/*    APU.cpp
    Author: Aaron Wiginton

    Description: APU for TinerasNES.
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


    **** Triangle ****

    $4008   clll llll   (c) linear counter start/length counter clock disable
                        (lll llll) linear counter load
    $4009    (unused)            
    $400A   pppp pppp   (pppp pppp) period low (8 LSB of wavelength)
    $400B   llll lppp   (llll l) length index
                        (ppp) period high


    **** Noise ****

    $400C   --le vvvv   (l) loop env/disable length
                        (e) env disable
                        (vvvv) vol/env period
    $400D    (unused)
    $400E   s--- pppp   (s) short/tone mode
                        (pppp) period index/playback sample rate
    $400F   llll l---   (l) length index


    **** DMC ****

    $4010   il-- ffff   (i) IRQ enable
                        (l) loop
                        (ffff) frequency index
    $4011   -ddd dddd   (ddd dddd) DAC
    $4012   aaaa aaaa   (aaaa aaaa) sample address
    $4013   llll llll   (llll llll) sample length


    **** Common/Control ****

    (write)                    
    $4015   ---d nt21   (d) DMC (If clear, the DMC's bytes remaining is set to 0,
                                otherwise the DMC sample is restarted only if the DMC's bytes remaining is 0)
                        (n) noise enable flag
                        (t) triangle enable flag
                        (2) pulse/rect 2 enable flag
                        (1) pulse/rect 1 enable flag
                        
                        ** Clears DMC Interrupt Flag after Write ($4010.7)

    (read)                    
    $4015   if-d nt21   (i) DMC IRQ (interrupt flag)
                        (f) frame IRQ (interrupt flag)
                        (d) 1 if DMC Bytes Remaining is non-zero
                        (n) 1 if noise is non-zero
                        (t) 1 if triangle is non-zero
                        (2) 1 if pulse/rect 2 is non-zero
                        (1) 1 if pulse/rect 1 is non-zero


    **** Frame Counter ****

    $4017   mi-- ----   (f) 0 - 4-frame cycle mode
                            1 - 5-frame cycle mode
                        (i) Interrupt Inhibit Flag. If set, the frame interrupt flag is cleared, otherwise it is unaffected.

                        ** Clears frame Interrupt Flag after Write ($4010.7)
                        ** If an interrupt flag was set at the same moment of the read,
                            it will read back as 1 but it will not be cleared.


   ************* APU REGISTERS ************** */


#include "apu.h"
#include "cpu.h"
//#include "SDL_audio.h"

const int APU::frameUpdate[2][5] =    {    { 3728, 7456, 11185, 14914, 14914 },
                                        { 3728, 7456, 11185, 18640, 18640} };

void APU_callback(void*, Uint8*, int);    // APU Callback
//SDL_AudioSpec audioSpecDesired, audioSpecObtained;

//    **********
//    APU something
void APU::renderFrame(int apuCycles)
{
    #pragma region Frame Sequencer Updates (Envelope, Sweep, Length)
    for(int i = 0; i < 20; i++)
    {
        apuCyclesTotal++;
        if (apuCyclesTotal == frameUpdate[frameNTSC_PAL - 4][0] || apuCyclesTotal == frameUpdate[frameNTSC_PAL - 4][2])
        {
            // Rectangle 1
            _rect1->updateEnvelope();

            // Rectangle 2
            _rect2->updateEnvelope();

            // Triangle
            _triangle->updateLinearCounter();

            // Noise
            _noise->updateEnvelope();

        }
        else if(apuCyclesTotal == frameUpdate[frameNTSC_PAL - 4][1] || apuCyclesTotal == frameUpdate[frameNTSC_PAL - 4][3])
        {
            // Rectangle 1
            _rect1->updateEnvelope();
            _rect1->updateSweep();
            _rect1->updateLengthCounter();

            // Rectangle 2
            _rect2->updateEnvelope();
            _rect2->updateSweep();
            _rect2->updateLengthCounter();

            // Triangle
            _triangle->updateLinearCounter();
            _triangle->updateLengthCounter();

            // Noise
            _noise->updateEnvelope();
            _noise->updateLengthCounter();

            if (frameIRQEnabled && (frameNTSC_PAL != 5) && apuCyclesTotal == frameUpdate[frameNTSC_PAL - 4][3])
            {
                _cpu->IRQPending = true;
            }
        }

        if(apuCyclesTotal >= (unsigned int)frameUpdate[frameNTSC_PAL - 4][3])
            apuCyclesTotal = 0;        
    }
    #pragma endregion

    #pragma region Mix Output Samples
    // Clear Output Sample
    outputSample = 0x00;

    if(rect1Enable && ch1Enable)
        outputSample += _rect1->renderSample();
    if(rect2Enable && ch2Enable)
        outputSample += _rect2->renderSample();
    if(triEnable && ch3Enable)
        outputSample += _triangle->renderSample();
    if(noiseEnable && ch4Enable)
        outputSample += _noise->renderSample();

    // Increase Volume
    //outputSample *= 2;
    #pragma endregion

    #pragma region Write Samples to Buffer(s)
    // Write Samples to Write Buffer
    if(apuWriteBufferPos < sizeof(apuWriteBuffer))
        apuWriteBuffer[apuWriteBufferPos++] = outputSample;

    // Wait for Write Buffer to fill to sizeof(apuPlayBuffer)
    if(apuWriteBufferPos % sizeof(apuPlayBuffer) == 0)
        WriteBufferReady = true;

    // Check for first full write
    if(apuWriteBufferPos >= sizeof(apuPlayBuffer))
        apuWriteBuffer_FirstFullWriteComplete = true;

    // Write sizeof(apuPlayBuffer) to Play Buffer
    if(apuPlayBufferEmpty && apuWriteBuffer_FirstFullWriteComplete && WriteBufferReady)
    {
        WriteBufferReady = false;

        // Write sizeof(apuPlayBuffer) to Play Buffer
        for(int i = 0; i < sizeof(apuPlayBuffer); i++)
        {
            apuPlayBuffer[i] = apuWriteBuffer[apuPlayBufferPos++];
        }

        // Reset Play Buffer Position
        if(apuPlayBufferPos >= sizeof(apuWriteBuffer))
            apuPlayBufferPos = 0;

        // Reset Write Buffer Position
        if(apuPlayBufferPos == (sizeof(apuWriteBuffer) - sizeof(apuPlayBuffer)))
            apuWriteBufferPos = 0;

        // Indicate that Play Buffer is Full
        apuPlayBufferEmpty = false;

        /******** DEBUGGING ********/
        //if(regUpdated)
        //{
        //    regUpdated = false;
        //    printf("$4000: %X, $4001: %X, $4002: %X, $4003: %X\n", regUpdateValues[0], regUpdateValues[1], regUpdateValues[2], regUpdateValues[3]);
        //}

        //printf("Write_Num: %u \n", numWrites);
        //printf("Play_Num: %u \n", numPlays);

        //// Write filestream for nestest debug
        //char charAPUStream[75];

        //if(apuWriteBufferPos == sizeof(apuPlayBuffer))
        //{
        //    prevWrite = sizeof(apuPlayBuffer);
        //    sprintf(charAPUStream, "Num Writes,Num Plays,Write Pos,Play Pos,Write Diff,W_P DIFF\n");
        //    apuDebugFileStream << charAPUStream;
        //}

        /**************************/
    }
    #pragma endregion
}

//    **********
//    Init APU
void APU::init_audio()
{
    /**** DEBUG ****/

    sampleCounter_W = 0;
    sampleCounter_R = 0;

    regUpdated = false;

    /***************/

    ///* Allocate a desired SDL_AudioSpec */
    //audioSpecDesired = malloc(sizeof(SDL_AudioSpec));
    ///* Allocate space for the obtained SDL_AudioSpec */
    //audioSpecObtained = malloc(sizeof(SDL_AudioSpec));

    apuCyclesTotal = 0;

    WriteBufferReady = false;

    dmcEnable = false;        // DMC Enable
    noiseEnable = false;    // Noise Enable
    triEnable = false;        // Triangle Enable
    rect2Enable = false;    // Rectangle/Pulse 2 Enable
    rect1Enable = false;    // Rectangle/Pulse 1 Enable

    frameNTSC_PAL = 4;    // 4 or 5 Frame Count
    frameCounter = 0;    // Frame Counter

    frameIRQEnabled = false;
    dmcIRQEnabled = false;
    frameIRQPending = false;
    IRQNextTime = false;

    apuBufferPos = 0;
    apuBufferLen = sizeof(apuPlayBuffer);
    memset(apuPlayBuffer, 0, sizeof(apuPlayBuffer));
    memset(apuWriteBuffer, 0, sizeof(apuWriteBuffer));

    apuPlayBufferPos = 0;
    apuWriteBufferPos = 0;
    apuWriteBufferPos_Last = 0;

    apuPlayBufferEmpty = true;
    apuWriteBuffer_FirstFullWriteComplete = false;

    //// Closes Audio Device before attempting to Open it
    //SDL_CloseAudio();

    //// Device Setup
    //audioSpecDesired.freq     = 44100;            // Sampling Rate;    
    //audioSpecDesired.format   = AUDIO_S8;        // Format
    //audioSpecDesired.channels = 1;                // Number of Channels
    //audioSpecDesired.samples  = apuBufferLen;    // Number of Samples in Buffer
    //audioSpecDesired.callback = APU_callback;    // Callback
    //audioSpecDesired.userdata = this;            

    //if (SDL_OpenAudio(&audioSpecDesired, &audioSpecObtained) < 0)
    //{
    //    // Open Audio FAIL
    //    printf("SDL Audio Init Failed: APU.cpp:audio_init()\n");
    //    fprintf(stderr, "Couldn't open audio: %s\n", SDL_GetError());
    //}
}

//    **********
//    APU Callback
void APU_callback(void* userdata, Uint8* stream, int len)    // APU Stuff
{
    APU *apu = (APU *)userdata;
    apu->apu_callback(stream, len);
}

//    **********
//    APU Callback
void APU::apu_callback(Uint8* stream, int len)    // APU Stuff
{
    memcpy(stream, apuPlayBuffer, len);
    apuPlayBufferEmpty = true;
}

//    **********
//    APU Play
void APU::APU_play()
{
    //SDL_PauseAudio(0);
}

//    **********
//    APU Pause
void APU::APU_pause()
{
    //SDL_PauseAudio(1);
}

//    **********
//    Rest APU Variables
void APU::reset()
{
    init_audio();
}

void APU::init(CPU* cpu)
{
    _cpu = cpu;
}

//    **********
//    APU Constructor
APU::APU()
{
    _rect1 = new APU_rect1();
    _rect2 = new APU_rect2();
    _triangle = new APU_triangle();
    _noise = new APU_noise();

    // File Stream for APU Debugging
    //apuDebugFileStream.open("apuDebug.txt");

    // Set 'APU (this)' class pointer to local 'apu' pointer
    //apu = this;

    reset();
}

//    **********
//    APU Destructor
APU::~APU()
{
    // Close APU Debugging File Stream
    //apuDebugFileStream.close();

    // Closes Audio Device
    //SDL_CloseAudio();
}