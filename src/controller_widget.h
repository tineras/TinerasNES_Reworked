#ifndef CONTROLLER_WIDGET_H
#define CONTROLLER_WIDGET_H

#include "common.h"

#include "ui_controller_widget.h"

class ControllerWidget : public QWidget
{
    Q_OBJECT

public:
    ControllerWidget();
    ~ControllerWidget();

    Ui::ControllerWidget* ui() { return _ui; }

private:
    Ui::ControllerWidget* _ui;
};

#endif
// CONTROLLER_WIDGET_H
