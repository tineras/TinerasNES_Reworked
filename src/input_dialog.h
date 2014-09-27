#ifndef INPUT_DIALOG_H
#define INPUT_DIALOG_H

#include "common.h"

#include "ui_input_dialog.h"
#include "controller_widget.h"

class InputDialog : public QDialog
{
    Q_OBJECT

public:
    InputDialog();
    ~InputDialog();

private slots:
    void onButtonUp();

private:
    void captureButton(QPushButton* button, QString label_text);

    Ui::InputDialog _ui;
    ControllerWidget* _controller_widget_1;
    ControllerWidget* _controller_widget_2;
};

#endif
// INPUT_DIALOG_H
