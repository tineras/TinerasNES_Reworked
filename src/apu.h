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
    void init_audio();

    /**** DEBUG ****/
    unsigned int _sample_counter_W;
    unsigned int _sample_counter_R;

    bool _reg_updated;
    unsigned char _reg_update_values[4];
    /***************/

    bool _ch1_enable;
    bool _ch2_enable;
    bool _ch3_enable;
    bool _ch4_enable;
    bool _ch5_enable;

    bool _write_buffer_ready;

    unsigned int _apu_cycles_total;

    unsigned char _apu_play_buffer[1024];
    unsigned char apu_write_buffer[65536];
    unsigned int _apu_play_buffer_pos;
    unsigned int _apu_write_buffer_pos;
    unsigned int _apu_write_buffer_pos_last;
    unsigned short _apu_buffer_pos;
    unsigned int _apu_buffer_len;
    bool _apu_play_buffer_empty;
    bool _apu_write_buffer_first_full_write_complete;

    unsigned char _frame_NTSC_PAL;  // 4 or 5 Frame Count
    unsigned int _frame_counter;    // Frame Counter

    bool _frame_IRQ_enabled;
    bool _dmc_IRQ_enabled;

    char _output_sample;

    bool _dmc_enabled;            // DMC Enable
    bool _noise_enabled;          // Noise Enable
    bool _triangle_enabled;       // Triangle Enable
    bool _rectangle_1_enabled;    // Rectangle/Pulse 1 Enable
    bool _rectangle_2_enabled;    // Rectangle/Pulse 2 Enable

    // UNUSED ??
    int _previous_write;
    unsigned int _dmc_flag;       // DMC IRG Flag Length
    unsigned int _frame_flag;     // Frame Interrupt Flag Length
    bool _frame_IRQ_pending;
    bool _IRQ_next_time;
    // UNUSED ??

    SDL_AudioSpec _audio_spec_desired, _audio_spec_obtained;

    // File stream for nestest file
    ofstream _apu_debug_file_stream;

    APU_noise* _noise;
    APU_rect1* _rect1;
    APU_rect2* _rect2;
    APU_triangle* _triangle;

    CPU* _cpu;
};
#endif //_APU_HEADER