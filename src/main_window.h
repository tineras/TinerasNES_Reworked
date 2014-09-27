#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include "common.h"

#include <QtWidgets/QMainWindow>
#include "ui_main_window.h"
#include "tinerasnes.h"
#include "glwidget.h"
#include "ram_viewer.h"
#include "input_dialog.h"

class GLWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

    unsigned char* pixels();

protected:
    virtual void closeEvent(QCloseEvent* event);
    virtual void mousePressEvent(QMouseEvent* event);
    virtual void keyPressEvent(QKeyEvent* event);
    virtual void keyReleaseEvent(QKeyEvent* event);

signals:
    void onKeyPressEvent(QKeyEvent* event);
    void onKeyReleaseEvent(QKeyEvent* event);
    void idle();

private slots:
    void openFile();
    void quit();
    void handleSDLEvents();
    void repaintGLWidget();
    void showRamViewer();
    void showInputDialog();

    // DEBUG
    void test1();
    void test2();

private:
    void readFile(QString filename);

    QThread* _emu_thread;
    TinerasNES* _tineras_nes;
    GLWidget* _GLWidget;
    Ui::MainWindow _ui;
    InputHandler* _input_handler;
    InputDialog* _input_dialog;
    RamViewer* _ram_viewer;

    QTimer _input_timer;

    bool _show_menu;
};

#endif
// MAIN_WINDOW_H
