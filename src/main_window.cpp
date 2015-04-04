#include "main_window.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    _emu_thread(nullptr),
    _tineras_nes(nullptr),
    _GLWidget(nullptr),
    _ram_viewer(nullptr),
    _input_handler(new InputHandler),
    _input_dialog(nullptr),
    _show_menu(true)
{
    _ui.setupUi(this);

    _ui.gridLayout->setContentsMargins(0, 0, 0, 0);
    _ui.status_bar->setEnabled(false);
    _ui.status_bar->setVisible(false);

    _emu_thread = new QThread;
    _tineras_nes = new TinerasNES();

    connect(this, SIGNAL(idle()), _tineras_nes, SLOT(idle()));
    connect(_tineras_nes, SIGNAL(handleSDLEvents()), this, SLOT(handleSDLEvents()));
    connect(_tineras_nes, SIGNAL(repaintGLWidget()), this, SLOT(repaintGLWidget()));

    _tineras_nes->moveToThread(_emu_thread);
    _emu_thread->start();

    // TODO: Properly exit thread once it's complete

    _GLWidget = new GLWidget(this);
    _ui.gridLayout->addWidget(_GLWidget, 0, 0, 1, 1);

    connect(_ui.action_open, SIGNAL(triggered()), this, SLOT(openFile()));
    connect(_ui.action_exit, SIGNAL(triggered()), this, SLOT(quit()));

    // DEBUG
    connect(_ui.action_ram_viewer, SIGNAL(triggered()), this, SLOT(showRamViewer()));
    connect(_ui.action_test_1, SIGNAL(triggered()), this, SLOT(test1()));
    connect(_ui.action_test_2, SIGNAL(triggered()), this, SLOT(test2()));

    connect(_ui.action_input, SIGNAL(triggered()), this, SLOT(showInputDialog()));

    _input_timer.setInterval(5);
    connect(&_input_timer, SIGNAL(timeout()), this, SLOT(handleSDLEvents()));
    _input_timer.start();

    this->show();
}

MainWindow::~MainWindow()
{
    delete _emu_thread;
    delete _tineras_nes;
    delete _GLWidget;
    delete _input_handler;
    delete _input_dialog;
    delete _ram_viewer;
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

    // TODO: Add archive support

    readFile(filename);
}

void MainWindow::readFile(QString filename)
{
    if (!filename.isEmpty())
    {
        _tineras_nes->setRunning(false);
        QThread::msleep(50);

        _tineras_nes->openFile(filename);

        emit idle();

        connect(_tineras_nes, SIGNAL(repaintGLWidget()), this, SLOT(repaintGLWidget()));

        _ui.menu_bar->hide();
        _show_menu = false;
    }
}

void MainWindow::showRamViewer()
{
    if (!_ram_viewer)
        _ram_viewer = new RamViewer(_tineras_nes);

    _ram_viewer->show();
}

void MainWindow::showInputDialog()
{
    if (!_input_dialog)
        _input_dialog = new InputDialog(_input_handler);

    _input_dialog->show();
}

void MainWindow::test1()
{
#if 0
    readFile("E:/Emulators/TestRoms/balloonfight.nes");
#else
    readFile("C:/Emu/TestRoms/balloonfight.nes");
#endif
}

void MainWindow::test2()
{
#if 0
    readFile("E:/Emulators/TestRoms/Scrolling Games/Super Mario Bros (PC10) - NTSC.nes");
#else
    readFile("C:/Emu/TestRoms/Scrolling Games/Super Mario Bros (PC10) - NTSC.nes");
#endif
}

void MainWindow::keyPressEvent(QKeyEvent* event)
{
    // Call base class key press event
    QWidget::keyPressEvent(event);

    if (event->isAutoRepeat())
        return;

    switch(event->key())
    {
    case Qt::Key_Enter:
    case Qt::Key_Return:
        if (event->modifiers() == Qt::AltModifier)
        {
            if (!this->isFullScreen())
            {
                _ui.menu_bar->hide();
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
        break;
    default:
        _input_handler->addKeyboardEvent(SDL_KEYDOWN, event->key());
        break;
    }
}

void MainWindow::keyReleaseEvent(QKeyEvent* event)
{
    // Call base class key press event
    QWidget::keyReleaseEvent(event);

    _input_handler->addKeyboardEvent(SDL_KEYUP, event->key());
}

void MainWindow::mousePressEvent(QMouseEvent* event)
{
    QWidget::mousePressEvent(event);

    if (_show_menu)
        _show_menu = false;
    else
        _show_menu = true;

    _show_menu ? _ui.menu_bar->show() : _ui.menu_bar->hide();
}

void MainWindow::quit()
{
    QApplication::quit();
}

void MainWindow::handleSDLEvents()
{
    if (!_input_dialog)
        return;

    if (_tineras_nes->running() && !_input_dialog->isVisible())
    {
        std::vector<unsigned char> button_down_events;
        std::vector<unsigned char> button_up_events;

        _input_handler->handleSDLInputEvents(button_down_events, button_up_events);

        for (auto buttons_it = button_down_events.begin(); buttons_it != button_down_events.end(); ++buttons_it)
        {
            unsigned char button = *buttons_it;
            _tineras_nes->onKeyPressEvent(button);
        }
        for (auto buttons_it = button_up_events.begin(); buttons_it != button_up_events.end(); ++buttons_it)
        {
            unsigned char button = *buttons_it;
            _tineras_nes->onKeyReleaseEvent(button);
        }
    }
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
