#ifndef GLWIDGET_H
#define GLWIDGET_H

#include "common.h"
#include "main_window.h"

static GLuint tex_id;

class GLWidget : public QGLWidget
{
    Q_OBJECT

public:
    GLWidget(MainWindow* main_window);
    ~GLWidget();

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);

private:
    int _current_width;
    int _current_height;

    MainWindow* _main_window;
};

#endif // GLWIDGET_H
