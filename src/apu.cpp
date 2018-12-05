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
#include <QAudioDeviceInfo>

//    **********
//    APU Constructor
APU::APU() :
    _rect1(new APU_rect1()),
    _rect2(new APU_rect2()),
    _triangle(new APU_triangle()),
    _noise(new APU_noise())
{
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
    delete _rect1;
    delete _rect2;
    delete _triangle;
    delete _noise;

    // Close APU Debugging File Stream
    //apuDebugFileStream.close();

    // Closes Audio Device
    SDL_CloseAudio();
}

void APU::init(CPU* cpu)
{
    _cpu = cpu;
}

static const int _frame_update[2][5] = { { 3728, 7456, 11185, 14914, 14914 },
                                         { 3728, 7456, 11185, 18640, 18640 } };

//    **********
//    APU something
void APU::renderFrame()
{
    #pragma region Frame Sequencer Updates (Envelope, Sweep, Length)
    for(int i = 0; i < 20; i++)
    {
        _apu_cycles_total++;
        if (_apu_cycles_total == _frame_update[_frame_NTSC_PAL - 4][0] || _apu_cycles_total == _frame_update[_frame_NTSC_PAL - 4][2])
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
        else if(_apu_cycles_total == _frame_update[_frame_NTSC_PAL - 4][1] || _apu_cycles_total == _frame_update[_frame_NTSC_PAL - 4][3])
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

            if (_frame_IRQ_enabled && (_frame_NTSC_PAL != 5) && _apu_cycles_total == _frame_update[_frame_NTSC_PAL - 4][3])
            {
                _cpu->IRQPending = true;
            }
        }

        if(_apu_cycles_total >= (unsigned int)_frame_update[_frame_NTSC_PAL - 4][3])
            _apu_cycles_total = 0;
    }
    #pragma endregion

    #pragma region Mix Output Samples
    // Clear Output Sample
    _output_sample = 0x00;

    if(_rectangle_1_enabled && _ch1_enable)
        _output_sample += _rect1->renderSample();
    if(_rectangle_2_enabled && _ch2_enable)
        _output_sample += _rect2->renderSample();
    if(_triangle_enabled && _ch3_enable)
        _output_sample += _triangle->renderSample();
    if(_noise_enabled && _ch4_enable)
        _output_sample += _noise->renderSample();

    // Increase Volume
    //outputSample *= 2;
    #pragma endregion

    #pragma region Write Samples to Buffer(s)
    // Write Samples to Write Buffer
    if(_apu_write_buffer_pos < sizeof(_apu_write_buffer))
        _apu_write_buffer[_apu_write_buffer_pos++] = _output_sample;

    // Wait for Write Buffer to fill to sizeof(apuPlayBuffer)
    if(_apu_write_buffer_pos % sizeof(_apu_play_buffer) == 0)
        _write_buffer_ready = true;

    // Check for first full write
    if(_apu_write_buffer_pos >= sizeof(_apu_play_buffer))
        _apu_write_buffer_first_full_write_complete = true;

    // Write sizeof(apuPlayBuffer) to Play Buffer
    if(_apu_play_buffer_empty && _apu_write_buffer_first_full_write_complete && _write_buffer_ready)
    {
        _write_buffer_ready = false;

        // Write sizeof(apuPlayBuffer) to Play Buffer
        for(int i = 0; i < sizeof(_apu_play_buffer); i++)
        {
            _apu_play_buffer[i] = _apu_write_buffer[_apu_play_buffer_pos++];
        }

        // Reset Play Buffer Position
        if(_apu_play_buffer_pos >= sizeof(_apu_write_buffer))
            _apu_play_buffer_pos = 0;

        // Reset Write Buffer Position
        if(_apu_play_buffer_pos == (sizeof(_apu_write_buffer) - sizeof(_apu_play_buffer)))
            _apu_write_buffer_pos = 0;

        // Indicate that Play Buffer is Full
        _apu_play_buffer_empty = false;

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
//    APU Callback
void APU_callback(void* userdata, Uint8* stream, int len)    // APU Stuff
{
    APU *apu = (APU*)userdata;
    apu->apu_callback(stream, len);
}

//    **********
//    APU Callback
void APU::apu_callback(Uint8* stream, int len)    // APU Stuff
{
    memcpy(stream, _apu_play_buffer, len);
    _apu_play_buffer_empty = true;
}

//    **********
//    Init APU
void APU::init_audio()
{
    /**** DEBUG ****/
    _sample_counter_W = 0;
    _sample_counter_R = 0;

    _reg_updated = false;
    /***************/

    _ch1_enable = true;
    _ch2_enable = true;
    _ch3_enable = true;
    _ch4_enable = true;
    _ch5_enable = true;

    _write_buffer_ready = false;

    _apu_cycles_total = 0;

    memset(_apu_play_buffer, 0, sizeof(_apu_play_buffer));
    memset(_apu_write_buffer, 0, sizeof(_apu_write_buffer));

    _apu_play_buffer_pos = 0;
    _apu_write_buffer_pos = 0;
    _apu_write_buffer_pos_last = 0;
    _apu_buffer_pos = 0;
    _apu_buffer_len = sizeof(_apu_play_buffer);
    _apu_play_buffer_empty = true;
    _apu_write_buffer_first_full_write_complete = false;

    _frame_NTSC_PAL = 4; // 4 or 5 Frame Count
    _frame_counter = 0;  // Frame Counter

    _frame_IRQ_enabled = false;
    _dmc_IRQ_enabled = false;

    _output_sample = 0x00;

    _dmc_enabled = false;         // DMC Enable
    _noise_enabled = false;       // Noise Enable
    _triangle_enabled = false;    // Triangle Enable
    _rectangle_2_enabled = false; // Rectangle/Pulse 2 Enable
    _rectangle_1_enabled = false; // Rectangle/Pulse 1 Enable

    // UNUSED ??
    _previous_write = false;
    _dmc_flag = false;           // DMC IRG Flag Length
    _frame_flag = false;         // Frame Interrupt Flag Length
    _frame_IRQ_pending = false;
    _IRQ_next_time = false;
    // UNUSED ??

    // Closes Audio Device before attempting to Open it
    SDL_CloseAudio();

    // Device Setup
    _audio_spec_desired.freq     = 44100;           // Sampling Rate;
    _audio_spec_desired.format   = AUDIO_S8;        // Format
    _audio_spec_desired.channels = 1;               // Number of Channels
    _audio_spec_desired.samples  = _apu_buffer_len; // Number of Samples in Buffer
    _audio_spec_desired.callback = APU_callback;    // Callback
    _audio_spec_desired.userdata = this;

    QAudioDeviceInfo ad_info(QAudioDeviceInfo::defaultOutputDevice());
    std::string default_audio_device_name = ad_info.deviceName().toStdString();
    printf("Default audio device name: %s\n", default_audio_device_name.c_str());

    printf("Enumerating audio devices...\n");
    int default_id = NULL;
    for (int i = 0; i < SDL_GetNumAudioDevices(0); i++)
    {
        printf("Id (%d) Audio device name: %s\n", i, SDL_GetAudioDeviceName(i, 0));

        if (default_audio_device_name.compare(SDL_GetAudioDeviceName(i, 0)) == 0)
        {
            default_id = i;
            break;
        }
    }

    // Passing 'NULL' for the device name will get the default device (https://wiki.libsdl.org/SDL_OpenAudioDevice)
    int playback_device_id = SDL_OpenAudioDevice(SDL_GetAudioDeviceName(default_id, 0), 0, &_audio_spec_desired, &_audio_spec_obtained, 0);

    if (_audio_spec_desired.format != _audio_spec_obtained.format)
    {
        printf("Obtained audio format does not match the desired audio format\n");
    }

    if (playback_device_id < 0)
    {
        printf("Failed to initialize SDL Audio. Error: %s\n", SDL_GetError());

        //throw; // TODO: Do something with this
    }
    else
    {
        printf("Successfully opened audio device: %s\n", SDL_GetAudioDeviceName(playback_device_id, 0));

        SDL_PauseAudioDevice(playback_device_id, 0);
    }
}

//    **********
//    APU Enable/Disable
void APU::setEnabled(bool enable)
{
    SDL_PauseAudio(enable ? 0 : 1);
}

//    **********
//    Rest APU Variables
void APU::reset()
{
    init_audio();
}
