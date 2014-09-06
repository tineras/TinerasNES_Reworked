#ifndef TINERASNES_H
#define TINERASNES_H

#include "common.h"

#include <QtWidgets/QMainWindow>
#include "ui_tinerasnes.h"

// https://wiki.libsdl.org/SDL_SetMainReady
// To prevent using SDL's main as an entry point
#define SDL_MAIN_HANDLED
#include "SDL.h"

class CPU;
class MEM;
class PPU;
class APU;
class NES_INPUT;

// NES Emu Resolution
static const int k_nes_res_x = 256;
static const int k_nes_res_y = 240;

static const int k_max_cpu_cycle = 29780 * 15; // Max CPU cycles per frame (29780)
static const double k_frame_time = 1.0 / 60 * 1000; // Time for a single frame (in ms)

class PanelWidget : public QWidget
{
public:
    PanelWidget(QObject* parent) : _should_draw(false), _draw_buffer(nullptr)
    {
    }
    ~PanelWidget()
    {
    }

    void setShouldDraw(bool should_draw) { _should_draw = should_draw; }
    void setDrawBuffer(unsigned char* buffer) { _draw_buffer = buffer; }
    void setImage()
    {
        //_img_bg = QImage(k_nes_res_x, k_nes_res_y, QImage::Format_RGB32);
        _img = QImage(_draw_buffer, k_nes_res_x, k_nes_res_y, QImage::Format_RGB32);
    }
    void setBG(unsigned char red, unsigned char green, unsigned char blue)
    {
        //_img_bg.fill(QColor(red, green, blue, 255));
    }

protected:
    void paintEvent(QPaintEvent* event)
    {
        QWidget::paintEvent(event);

        if (_should_draw)
        {
            if (this->isVisible())
            {
                if (_draw_buffer == nullptr)
                    return;

                QPainter _painter(this);

                _img_scaled = _img.scaled(this->width(), this->height(), Qt::IgnoreAspectRatio);

                _painter.drawImage(0, 0, _img_scaled);
            }
        }
    }
    
    void resizeEvent(QResizeEvent* event)
    {
        QWidget::resizeEvent(event);

        if (_draw_buffer != nullptr)
        {
            _img_scaled = _img.scaled(this->width(), this->height(), Qt::IgnoreAspectRatio);
        }
    }

private:
    //QByteArray _draw_buffer;
    //QImage _img_bg;
    //QImage _img_bg_scaled;
    QImage _img;
    QImage _img_scaled;
    bool _should_draw;
    unsigned char* _draw_buffer;
};

class TinerasNES : public QMainWindow
{
    Q_OBJECT

public:
    TinerasNES(QWidget *parent = 0);
    ~TinerasNES();

    int currentCPUCycle() { return _current_cpu_cycle; }
    void bumpCurrentCPUCycle(int add_cycles) { _current_cpu_cycle += add_cycles; }

protected:
    virtual void closeEvent(QCloseEvent* event);
    virtual void keyPressEvent(QKeyEvent* event);
    virtual void keyReleaseEvent(QKeyEvent* event);

private slots:
    void run();
    void openFile();
    void quit();

private:
    void init();
    void initSDL();

    bool _running;

    PanelWidget* _panel_widget;

    unsigned int _master_cpu_cycle;
    unsigned int _current_cpu_cycle;

    int _apu_frame_count;

    QElapsedTimer _frame_timer;

    Ui::MenuBar _ui;
    CPU* _cpu;
    MEM* _mem;
    PPU* _ppu;
    APU* _apu;
    NES_INPUT* _nes_input;
};

#endif // TINERASNES_H
