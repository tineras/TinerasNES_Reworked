#include "input_dialog.h"

InputDialog::InputDialog()
{
    _ui.setupUi(this);

    _controller_widget_1 = new ControllerWidget();
    _controller_widget_2 = new ControllerWidget();

    _controller_widget_1->ui()->frame_set_input->setVisible(false);
    _controller_widget_2->ui()->frame_set_input->setVisible(false);

    _ui.tab_widget->addTab(_controller_widget_1, "Controller 1");
    _ui.tab_widget->addTab(_controller_widget_2, "Controller 2");
    connect(_controller_widget_1->ui()->button_up, SIGNAL(clicked()), this, SLOT(onButtonUp()));
}

InputDialog::~InputDialog()
{
}

void InputDialog::captureButton(QPushButton* button, QString label_text)
{
    ControllerWidget* controller_widget = static_cast<ControllerWidget*>(button->parent());

    controller_widget->ui()->label_set_input->setText(label_text);
    controller_widget->ui()->frame_set_input->setVisible(true);

    // DEBUG: REMOVE ME
    QDialog d;
    d.exec();

    // Get/set input here... *blocking*
    button->setText("joy_btn_2"); // whatever was returned by sdl

    controller_widget->ui()->frame_set_input->setVisible(false);
}

void InputDialog::onButtonUp()
{
    captureButton((QPushButton*)QObject::sender(), "Up");
}
