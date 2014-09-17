#ifndef RAM_VIEWER_H
#define RAM_VIEWER_H

#include "common.h"

#include "ui_ram_viewer.h"
#include "tinerasnes.h"

class RamViewer : public QDialog
{
    Q_OBJECT

public:
    RamViewer(TinerasNES* tineras_nes);
    ~RamViewer();

private slots:
    void updateText();

private:
    QTimer _update_timer;

    TinerasNES* _tineras_nes;
    Ui::RamViewer _ui;
};

#endif
// RAM_VIEWER_H
