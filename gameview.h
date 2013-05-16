#ifndef GAMEVIEW_H
#define GAMEVIEW_H

#include <QGLWidget>
#include <vector>
#include <sys/time.h>

#include "game.h"

class GameView : public QGLWidget
{
    Q_OBJECT
public:
    explicit GameView(const QGLFormat& format, QWidget *parent = 0);

    int prefered_width() const;
    int prefered_height() const;
protected:
    void initializeGL();
    void resizeGL(int width, int height);
    void paintGL();

    void showEvent(QShowEvent *event);
    void timerEvent(QTimerEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
private:
    void compile_shader(GLuint shader_identifier, const std::string& shader_source);
    void link_program();
    void update_vertex_buffer();

    int _tile_size;

    Game _game;
    timeval _lastTime;

    int _timer_id;
    GLuint _vao;
    GLuint _vertex_buffer_identifier;
    GLuint _program_identifier;
    GLuint _texture_identifier;
    
signals:
    
public slots:
    
};

#endif // GAMEVIEW_H
