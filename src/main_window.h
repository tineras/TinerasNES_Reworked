#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include "common.h"

#include <QtWidgets/QMainWindow>
#include "ui_main_window.h"
#include "tinerasnes.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    virtual void closeEvent(QCloseEvent* event);
    virtual void keyPressEvent(QKeyEvent* event);
    virtual void keyReleaseEvent(QKeyEvent* event);

signals:
    void onKeyPressEvent(QKeyEvent* event);
    void onKeyReleaseEvent(QKeyEvent* event);

private slots:
    void openFile();
    void quit();

private:
    QThread* _main_thread;
    TinerasNES* _tineras_nes;
    Ui::MenuBar _ui;
};

#endif
// MAIN_WINDOW_H
