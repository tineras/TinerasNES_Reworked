#include "controller_widget.h"

ControllerWidget::ControllerWidget() :
    _ui(new Ui::ControllerWidget)
{
    _ui->setupUi(this);
}

ControllerWidget::~ControllerWidget()
{
}
