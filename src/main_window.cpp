#include "main_window.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    _emu_thread(nullptr),
    _tineras_nes(nullptr),
    _GLWidget(nullptr),
    _show_menu(true)
{
    _ui.setupUi(this);

    _ui.gridLayout->setContentsMargins(0, 0, 0, 0);
    _ui.statusBar->setEnabled(false);
    _ui.statusBar->setVisible(false);

    _emu_thread = new QThread;
    _tineras_nes = new TinerasNES();

    connect(this, SIGNAL(idle()), _tineras_nes, SLOT(idle()));
    connect(_tineras_nes, SIGNAL(repaintGLWidget()), this, SLOT(repaintGLWidget()));

    _tineras_nes->moveToThread(_emu_thread);
    _emu_thread->start();

    // TODO: Properly exit thread once it's complete

    _GLWidget = new GLWidget(this);
    _ui.gridLayout->addWidget(_GLWidget, 0, 0, 1, 1);

    connect(_ui.actionOpen, SIGNAL(triggered()), this, SLOT(openFile()));
    connect(_ui.actionExit, SIGNAL(triggered()), this, SLOT(quit()));

    this->show();
}

MainWindow::~MainWindow()
{
    delete _emu_thread;
    delete _tineras_nes;
    delete _GLWidget;
}

unsigned char* MainWindow::pixels()
{
    return _tineras_nes->pixels();
}

void MainWindow::openFile()
{
#if 0
    QString filename = QFileDialog::getOpenFileName(dynamic_cast<QWidget*>(sender()), "Select NES Rom", "c:/emu/TestRoms", "NES file (*.nes);;Zipped NES file (*.zip)");
#else
    QString filename = QFileDialog::getOpenFileName(this, "Select NES Rom", "E:/Emulators/TestRoms", "NES file (*.nes);;Zipped NES file (*.zip)");
#endif

    if (!filename.isEmpty())
    {
        // TODO: Reset state on each file open. This is currently broken.

        _tineras_nes->openFile(filename);

        emit idle();

        _timer.setInterval(17);
        connect(&_timer, SIGNAL(timeout()), this, SLOT(repaintGLWidget()));
        _timer.start();

        _ui.menuBar->hide();
    }
}

void MainWindow::keyPressEvent(QKeyEvent* event)
{
    // Call base class key press event
    QWidget::keyPressEvent(event);

    switch(event->key())
    {
    case Qt::Key_Enter:
    case Qt::Key_Return:
        if (event->modifiers() == Qt::AltModifier)
        {
            if (!this->isFullScreen())
            {
                _ui.menuBar->hide();
                this->showFullScreen();
            }
            else
            {
                this->showNormal();
            }
        }
        break;
    case Qt::Key_Escape:
        if (this->isFullScreen())
        {
            this->showNormal();
        }
    default:
        break;
    }

    // TODO: Fix this. It's not doing anything since I moved the 'new' back to the constructor
    if (_tineras_nes->running())
        _tineras_nes->onKeyPressEvent(event->key());
}

void MainWindow::keyReleaseEvent(QKeyEvent* event)
{
    // Call base class key press event
    QWidget::keyPressEvent(event);
    
    if (_tineras_nes->running())
        _tineras_nes->onKeyReleaseEvent(event->key());
}

void MainWindow::mousePressEvent(QMouseEvent* event)
{
    QWidget::mousePressEvent(event);

    if (_show_menu)
        _show_menu = false;
    else
        _show_menu = true;

    _show_menu ? _ui.menuBar->show() : _ui.menuBar->hide();
}

void MainWindow::quit()
{
    QApplication::quit();
}

void MainWindow::repaintGLWidget()
{
    if (_tineras_nes->drawingFrame())
    {
        _GLWidget->updateGL();

        _tineras_nes->setDrawingFrame(false);
    }
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    _tineras_nes->quit();

    QWidget::closeEvent(event);
}
