#include "main_window.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    _ui.setupUi(this);

    _main_thread = new QThread;
    _tineras_nes = new TinerasNES(this);
    _tineras_nes->moveToThread(_main_thread);
    _main_thread->start();

    //_ui.gridLayout->addWidget(_panel_widget);

    connect(_ui.actionOpen, SIGNAL(triggered()), this, SLOT(openFile()));
    connect(_ui.actionExit, SIGNAL(triggered()), this, SLOT(quit()));
    //connect(this, SIGNAL(onKeyPressEvent(QKeyEvent* event)), _tineras_nes, SLOT(onKeyPressEvent(QKeyEvent* event)));
    //connect(this, SIGNAL(onKeyReleaseEvent(QKeyEvent* event)), _tineras_nes, SLOT(onKeyReleaseEvent(QKeyEvent* event)));

    this->show();
}

MainWindow::~MainWindow()
{

}

void MainWindow::openFile()
{
    QMetaObject::invokeMethod(_tineras_nes, "openFile", Qt::QueuedConnection);
}

void MainWindow::keyPressEvent(QKeyEvent* event)
{
    // Call base class key press event
    QWidget::keyPressEvent(event);

    QMetaObject::invokeMethod(_tineras_nes, "onKeyPressEvent", Qt::QueuedConnection, Q_ARG(int, event->key()));
}

void MainWindow::keyReleaseEvent(QKeyEvent* event)
{
    // Call base class key press event
    QWidget::keyPressEvent(event);

    QMetaObject::invokeMethod(_tineras_nes, "onKeyReleaseEvent", Qt::QueuedConnection, Q_ARG(int, event->key()));
}

void MainWindow::quit()
{
    QApplication::quit();
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    QWidget::closeEvent(event);
}
