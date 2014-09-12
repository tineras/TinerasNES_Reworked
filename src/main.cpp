#include "main_window.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow* main_window = new MainWindow();
    return a.exec();
}
