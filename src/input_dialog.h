#ifndef INPUT_DIALOG_H
#define INPUT_DIALOG_H

#include "common.h"

#include "input_handler.h"
#include "ui_input_dialog.h"
#include "controller_widget.h"

class InputDialog : public QDialog
{
    Q_OBJECT

public:
    InputDialog(InputHandler* input_handler);
    ~InputDialog();

private slots:
    void onButtonUp();
    void onButtonDown();
    void onButtonLeft();
    void onButtonRight();
    void onButtonSelect();
    void onButtonStart();
    void onButtonB();
    void onButtonA();

    void onSetAllButtons();

private:
    void captureButton(QPushButton* button, ButtonType button_type, QString label_text);

    ControllerWidget* _controller_widget_1;
    ControllerWidget* _controller_widget_2;
    InputHandler* _input_handler;

    Ui::InputDialog _ui;
};

#endif
// INPUT_DIALOG_H
