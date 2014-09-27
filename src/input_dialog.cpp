#include "input_dialog.h"

InputDialog::InputDialog(InputHandler* input_handler) :
    _input_handler(input_handler)
{
    _ui.setupUi(this);

    _controller_widget_1 = new ControllerWidget();
    _controller_widget_2 = new ControllerWidget();

    _controller_widget_1->ui()->frame_set_input->setVisible(false);
    _controller_widget_2->ui()->frame_set_input->setVisible(false);

    _ui.tab_widget->addTab(_controller_widget_1, "Controller 1");
    _ui.tab_widget->addTab(_controller_widget_2, "Controller 2");

    connect(_controller_widget_1->ui()->button_up, SIGNAL(clicked()), this, SLOT(onButtonUp()));
    connect(_controller_widget_1->ui()->button_down, SIGNAL(clicked()), this, SLOT(onButtonDown()));
    connect(_controller_widget_1->ui()->button_left, SIGNAL(clicked()), this, SLOT(onButtonLeft()));
    connect(_controller_widget_1->ui()->button_right, SIGNAL(clicked()), this, SLOT(onButtonRight()));
    connect(_controller_widget_1->ui()->button_select, SIGNAL(clicked()), this, SLOT(onButtonSelect()));
    connect(_controller_widget_1->ui()->button_start, SIGNAL(clicked()), this, SLOT(onButtonStart()));
    connect(_controller_widget_1->ui()->button_b, SIGNAL(clicked()), this, SLOT(onButtonB()));
    connect(_controller_widget_1->ui()->button_a, SIGNAL(clicked()), this, SLOT(onButtonA()));

    connect(_controller_widget_2->ui()->button_up, SIGNAL(clicked()), this, SLOT(onButtonUp()));
    connect(_controller_widget_2->ui()->button_down, SIGNAL(clicked()), this, SLOT(onButtonDown()));
    connect(_controller_widget_2->ui()->button_left, SIGNAL(clicked()), this, SLOT(onButtonLeft()));
    connect(_controller_widget_2->ui()->button_right, SIGNAL(clicked()), this, SLOT(onButtonRight()));
    connect(_controller_widget_2->ui()->button_select, SIGNAL(clicked()), this, SLOT(onButtonSelect()));
    connect(_controller_widget_2->ui()->button_start, SIGNAL(clicked()), this, SLOT(onButtonStart()));
    connect(_controller_widget_2->ui()->button_b, SIGNAL(clicked()), this, SLOT(onButtonB()));
    connect(_controller_widget_2->ui()->button_a, SIGNAL(clicked()), this, SLOT(onButtonA()));
}

InputDialog::~InputDialog()
{
}

void InputDialog::captureButton(QPushButton* button, ButtonType button_type, QString label_text)
{
    ControllerWidget* controller_widget = static_cast<ControllerWidget*>(button->parent());

    controller_widget->ui()->label_set_input->setText(label_text);
    controller_widget->ui()->frame_set_input->setVisible(true);

    _input_handler->flushEvents();
    QString button_text = _input_handler->captureButton(button_type);
    button->setText(button_text);

    controller_widget->ui()->frame_set_input->setVisible(false);
}

void InputDialog::onButtonUp()
{
    captureButton((QPushButton*)QObject::sender(), Up, "Up");
}

void InputDialog::onButtonDown()
{
    captureButton((QPushButton*)QObject::sender(), Down, "Down");
}

void InputDialog::onButtonLeft()
{
    captureButton((QPushButton*)QObject::sender(), Left, "Left");
}

void InputDialog::onButtonRight()
{
    captureButton((QPushButton*)QObject::sender(), Right, "Right");
}

void InputDialog::onButtonSelect()
{
    captureButton((QPushButton*)QObject::sender(), Select, "Select");
}

void InputDialog::onButtonStart()
{
    captureButton((QPushButton*)QObject::sender(), Start, "Start");
}

void InputDialog::onButtonB()
{
    captureButton((QPushButton*)QObject::sender(), B, "B");
}

void InputDialog::onButtonA()
{
    captureButton((QPushButton*)QObject::sender(), A, "A");
}
