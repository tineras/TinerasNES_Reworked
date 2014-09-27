#ifndef TINERASNES_H
#define TINERASNES_H

#include "common.h"

// https://wiki.libsdl.org/SDL_SetMainReady
// To prevent using SDL's main as an entry point
#define SDL_MAIN_HANDLED
#include "SDL.h"

class CPU;
class MEM;
class PPU;
class APU;
class NES_INPUT;

class MainWindow;

// NES Emu Resolution
static const int k_nes_res_x = 256;
static const int k_nes_res_y = 240;

static const int k_max_cpu_cycle = 29780 * 15; // Max CPU cycles per frame (29780)
static const double k_frame_time = 1.0 / 65 * 1000; // Time for a single frame (in ms)

class TinerasNES : public QObject
{
    Q_OBJECT

public:
    TinerasNES();
    ~TinerasNES();

    void run();
    void onKeyPressEvent(unsigned char button);
    void onKeyReleaseEvent(unsigned char button);
    void openFile(QString filename);

    bool running() { return _running; }
    void setRunning(bool running) { _running = running; }
    int currentCPUCycle() { return _current_cpu_cycle; }
    void bumpCurrentCPUCycle(int add_cycles) { _current_cpu_cycle += add_cycles; }

    bool drawingFrame() { return _drawing_frame; }
    void setDrawingFrame(bool drawing_frame) { _drawing_frame = drawing_frame; }

    unsigned char* pixels() { return _draw_buffer; }

    MEM* mem() { return _mem; }

    void quit();

signals:
    void repaintGLWidget();

public slots:
    void idle();

private:
    void init();
    void initSDL();

    bool _running;
    bool _drawing_frame;
    bool _quit;

    unsigned int _master_cpu_cycle;
    unsigned int _current_cpu_cycle;
    int _apu_frame_count;

    unsigned char* _draw_buffer;

    QElapsedTimer _frame_timer;

    CPU* _cpu;
    MEM* _mem;
    PPU* _ppu;
    APU* _apu;
    NES_INPUT* _nes_input;
};

#endif // TINERASNES_H
