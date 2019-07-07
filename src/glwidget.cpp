#include "glwidget.h"

GLWidget::GLWidget(MainWindow* main_window) : _main_window(main_window)
{

}

GLWidget::~GLWidget()
{

}

void GLWidget::initializeGL()
{
    qglClearColor(Qt::black);

    glGenTextures(1, &tex_id);

    glBindTexture(GL_TEXTURE_2D, tex_id);
    glTexEnvi(GL_TEXTURE_2D, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, 1, 1, 0, -1, 1);

    glBindTexture(GL_TEXTURE_2D, tex_id);

    glEnable(GL_TEXTURE_2D);
}

void GLWidget::paintGL()
{
    if (!_main_window->initialized())
        return;

    unsigned char* buffer = _main_window->pixels();

    if (buffer == nullptr)
        return;

    // This just puts the image into memory, it does not control how it's drawn to the screen
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 240, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer);

    glClear(GL_COLOR_BUFFER_BIT);

    glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex2f(0, 0);
    glTexCoord2f(1, 0); glVertex2f(1, 0);
    glTexCoord2f(1, 1); glVertex2f(1, 1);
    glTexCoord2f(0, 1); glVertex2f(0, 1);
    glEnd();
}

void GLWidget::resizeGL(int width, int height)
{
    _current_width = width;
    _current_height = height;

    glViewport(0, 0, width, height);
}
