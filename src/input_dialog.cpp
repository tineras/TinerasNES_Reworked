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

    connect(_controller_widget_1->ui()->button_set_all_buttons, SIGNAL(clicked()), this, SLOT(onSetAllButtons()));

    connect(this, SIGNAL(accepted()), this, SLOT(onAccepted()));

    QStringList joystick_names = _input_handler->joystickNames();
    for (auto joystick_names_it = joystick_names.begin(); joystick_names_it != joystick_names.end(); ++joystick_names_it)
    {
        _controller_widget_1->ui()->combo_box_input_device->addItem(*joystick_names_it);
        _controller_widget_2->ui()->combo_box_input_device->addItem(*joystick_names_it);
    }
}

InputDialog::~InputDialog()
{
    delete _controller_widget_1;
    delete _controller_widget_2;
    delete _input_handler;
}

void InputDialog::captureButton(QPushButton* button, ButtonType button_type, QString label_text)
{
    ControllerWidget* controller_widget = static_cast<ControllerWidget*>(button->parent());

    controller_widget->ui()->label_set_input->setText(label_text);
    controller_widget->ui()->frame_set_input->setVisible(true);

    _input_handler->flushEvents();
    QString button_text = _input_handler->captureButton(this, button_type);
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

void InputDialog::onSetAllButtons()
{
    auto sender = (QPushButton*)QObject::sender();
    auto parent = static_cast<ControllerWidget*>(sender->parent());

    // Not sure if this is necessary since inputs are overwritten
    //_input_handler->clearJoystickInputs();

    captureButton(parent->ui()->button_up, Up, "Up");
    captureButton(parent->ui()->button_down, Down, "Down");
    captureButton(parent->ui()->button_left, Left, "Left");
    captureButton(parent->ui()->button_right, Right, "Right");
    captureButton(parent->ui()->button_select, Select, "Select");
    captureButton(parent->ui()->button_start, Start, "Start");
    captureButton(parent->ui()->button_b, B, "B");
    captureButton(parent->ui()->button_a, A, "A");
}

void InputDialog::keyPressEvent(QKeyEvent* event)
{

    // Call base class key press event
    QWidget::keyPressEvent(event);

    if (event->isAutoRepeat())
        return;

    _input_handler->addKeyboardEvent(SDL_KEYDOWN, event->key());
}

void InputDialog::keyReleaseEvent(QKeyEvent* event)
{
    // Call base class key release event
    QWidget::keyReleaseEvent(event);

    _input_handler->addKeyboardEvent(SDL_KEYUP, event->key());
}

void InputDialog::forceProcessEvents()
{
    QCoreApplication::processEvents();
}

void InputDialog::onAccepted()
{
    // Save inputs
}
