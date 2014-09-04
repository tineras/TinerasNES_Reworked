#include "tinerasnes.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    TinerasNES w;
    w.show();
    return a.exec();
}
