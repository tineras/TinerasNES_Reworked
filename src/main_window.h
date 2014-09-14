#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include "common.h"

#include <QtWidgets/QMainWindow>
#include "ui_main_window.h"
#include "tinerasnes.h"
#include "glwidget.h"

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
    void repaintGLWidget();

private:
    QThread* _emu_thread;
    TinerasNES* _tineras_nes;
    GLWidget* _GLWidget;
    Ui::MenuBar _ui;

    bool _show_menu;

    QTimer _timer;
};

#endif
// MAIN_WINDOW_H
