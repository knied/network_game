// =================================================================
// Author: Kristof Niederholtmeyer
//

#include "gameview.h"

#include <QtGui>
#include <iostream>
#include <vector>

struct Vertex {
    float x, y, z;
    float u, v;
    Color color0;
    Color color1;
};

GameView::GameView(const QGLFormat& format, QWidget *parent) :
    QGLWidget(format, parent), _tile_size(24), _timer_id(0) 
{
    setFocusPolicy(Qt::StrongFocus);
}

int GameView::prefered_width() const {
    return GameClient::ViewWidth * _tile_size ;
}

int GameView::prefered_height() const {
    return GameClient::ViewHeight * _tile_size;
}

void GameView::compile_shader(GLuint shader_identifier, const std::string& shader_source) {
    GLint shader_ok = 0;

    // compile the vertex shader
    const char* c_source = shader_source.c_str();
    glShaderSource(shader_identifier, 1, &c_source, 0);
    glCompileShader(shader_identifier);
    glGetShaderiv(shader_identifier, GL_COMPILE_STATUS, &shader_ok);
    if (!shader_ok) {
        GLint log_length;
        glGetShaderiv(shader_identifier, GL_INFO_LOG_LENGTH, &log_length);
        char *log = new char[log_length];
        glGetShaderInfoLog(shader_identifier, log_length, 0, log);
        std::cout << shader_source << std::endl;
        std::cout << "ERROR: Unable to compile shader:" << std::endl;
        std::cout << log << std::endl;
        delete [] log;
    }
}

void GameView::link_program() {
    glLinkProgram(_program_identifier);
    GLint program_ok;
    glGetProgramiv(_program_identifier, GL_LINK_STATUS, &program_ok);
    if (!program_ok) {
        GLint log_length;
        glGetProgramiv(_program_identifier, GL_INFO_LOG_LENGTH, &log_length);
        char* log = new char[log_length];
        glGetProgramInfoLog(_program_identifier, log_length, 0, log);
        std::cout << "ERROR: Unable to link program" << std::endl;
        std::cout << log << std::endl;
        delete [] log;
    }
}

void GameView::update_vertex_buffer() {
    std::vector<Vertex> vertex_data(GameClient::ViewWidth * GameClient::ViewHeight * 6);
    for(int y = 0; y < GameClient::ViewHeight; ++y) {
        for(int x = 0; x < GameClient::ViewWidth; ++x) {
            float tmp = 1.0f / 16.0f;

            int tile_x = _game.player_view().symbol(x, y) % 16;
            int tile_y = _game.player_view().symbol(x, y) / 16;

            float u0 = 0.0f;
            float u1 = 0.0f;
            float v0 = 0.0f;
            float v1 = 0.0f;
            if (_game.player_view().flip_x(x, y)) {
                u1 = tile_x * tmp;
                u0 = (tile_x + 1) * tmp;
            } else {
                u0 = tile_x * tmp;
                u1 = (tile_x + 1) * tmp;
            }
            if (_game.player_view().flip_y(x, y)) {
                v1 = 1.0f - (tile_y) * tmp;
                v0 = 1.0f - (tile_y + 1) * tmp;
            } else {
                v0 = 1.0f - (tile_y) * tmp;
                v1 = 1.0f - (tile_y + 1) * tmp;
            }
            
            Color color0 = _game.color(_game.player_view().symbol_color(x, y));
            Color color1 = _game.color(_game.player_view().background_color(x, y));

            float x_offset = -(float)GameClient::ViewWidth / 2.0f * (float)_tile_size;
            float y_offset = -(float)GameClient::ViewHeight / 2.0f * (float)_tile_size;

            Vertex vertex0 = {(float)(x) * (float)_tile_size + x_offset, (float)(y+1) * (float)_tile_size + y_offset, 0.0f, u0, v0, color0, color1};
            Vertex vertex1 = {(float)(x+1) * (float)_tile_size + x_offset, (float)(y+1) * (float)_tile_size + y_offset, 0.0f, u1, v0, color0, color1};
            Vertex vertex2 = {(float)(x+1) * (float)_tile_size + x_offset, (float)(y) * (float)_tile_size + y_offset, 0.0f, u1, v1, color0, color1};
            Vertex vertex3 = {(float)(x) * (float)_tile_size + x_offset, (float)(y) * (float)_tile_size + y_offset, 0.0f, u0, v1, color0, color1};

            vertex_data[6 * GameClient::ViewWidth * y + 6 * x + 0] = vertex0;
            vertex_data[6 * GameClient::ViewWidth * y + 6 * x + 1] = vertex1;
            vertex_data[6 * GameClient::ViewWidth * y + 6 * x + 2] = vertex2;

            vertex_data[6 * GameClient::ViewWidth * y + 6 * x + 3] = vertex0;
            vertex_data[6 * GameClient::ViewWidth * y + 6 * x + 4] = vertex2;
            vertex_data[6 * GameClient::ViewWidth * y + 6 * x + 5] = vertex3;
        }
    }

    glBindVertexArray(_vao);
    glBindBuffer(GL_ARRAY_BUFFER, _vertex_buffer_identifier);
    glBufferData(GL_ARRAY_BUFFER,
                 vertex_data.size() * sizeof(Vertex),
                 &vertex_data[0],
                 GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,
                          3,
                          GL_FLOAT,
                          GL_TRUE,
                          sizeof(Vertex),
                          0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,
                          2,
                          GL_FLOAT,
                          GL_TRUE,
                          sizeof(Vertex),
                          &(((Vertex*)0)->u));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2,
                          4,
                          GL_UNSIGNED_BYTE,
                          GL_TRUE,
                          sizeof(Vertex),
                          &(((Vertex*)0)->color0));
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3,
                          4,
                          GL_UNSIGNED_BYTE,
                          GL_TRUE,
                          sizeof(Vertex),
                          &(((Vertex*)0)->color1));
}

void GameView::initializeGL()
{
    glClearColor(0,0,0,0);

    glGenVertexArrays(1, &_vao);
    glGenBuffers(1, &_vertex_buffer_identifier);
    _program_identifier = glCreateProgram();

    QImage img;
    if (!img.load(":/8x8tiles.png")) {
        std::cout << "image not found!" << std::endl;
    }

    QImage GL_formatted_image;
    GL_formatted_image = QGLWidget::convertToGLFormat(img);

    glGenTextures(1, &_texture_identifier);
    glBindTexture(GL_TEXTURE_2D, _texture_identifier);
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, GL_formatted_image.width(),
                  GL_formatted_image.height(),
                  0, GL_RGBA, GL_UNSIGNED_BYTE, GL_formatted_image.bits() );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    update_vertex_buffer();

#if defined(__APPLE__)
    // Use GLSL 1.5
    std::string vertex_shader;
    vertex_shader += "#version 150\n";

    vertex_shader += "in vec4 vertex;\n";
    vertex_shader += "in vec2 texcoord;\n";
    vertex_shader += "in vec4 color0;\n";
    vertex_shader += "in vec4 color1;\n";

    vertex_shader += "uniform mat4 mvp;\n";

    vertex_shader += "out vec2 v_texcoord;\n";
    vertex_shader += "out vec4 v_color0;\n";
    vertex_shader += "out vec4 v_color1;\n";

    vertex_shader += "void main() {\n";
    vertex_shader += "  gl_Position = mvp * vertex;\n";
    vertex_shader += "  v_texcoord = texcoord;\n";
    vertex_shader += "  v_color0 = color0;\n";
    vertex_shader += "  v_color1 = color1;\n";
    vertex_shader += "}\n";

    std::string fragment_shader;
    fragment_shader += "#version 150\n";

    fragment_shader += "in vec2 v_texcoord;\n";
    fragment_shader += "in vec4 v_color0;\n";
    fragment_shader += "in vec4 v_color1;\n";

    fragment_shader += "uniform sampler2D texture0;\n";

    fragment_shader += "out vec4 frag_color;\n";

    fragment_shader += "void main() {\n";
    fragment_shader += "  float r = texture(texture0, v_texcoord).r;\n";
    //fragment_shader += "  float r = v_texcoord.x;\n";
    fragment_shader += "  frag_color = v_color0 * r + v_color1 * (1.0 - r);\n";
    //fragment_shader += "  frag_color = r * vec4(1.0,0.0,0.0,1.0) + (1.0 - r) * vec4(0.0,0.0,1.0,1.0);\n";
    fragment_shader += "}\n";
#elif
    // Use GLSL 1.2
    std::string vertex_shader;
    vertex_shader += "#version 120\n";

    vertex_shader += "attribute vec4 vertex;\n";
    vertex_shader += "attribute vec2 texcoord;\n";
    vertex_shader += "attribute vec4 color0;\n";
    vertex_shader += "attribute vec4 color1;\n";

    vertex_shader += "uniform mat4 mvp;\n";

    vertex_shader += "varying vec2 v_texcoord;\n";
    vertex_shader += "varying vec4 v_color0;\n";
    vertex_shader += "varying vec4 v_color1;\n";

    vertex_shader += "void main() {\n";
    vertex_shader += "  gl_Position = mvp * vertex;\n";
    vertex_shader += "  v_texcoord = texcoord;\n";
    vertex_shader += "  v_color0 = color0;\n";
    vertex_shader += "  v_color1 = color1;\n";
    vertex_shader += "}\n";

    std::string fragment_shader;
    fragment_shader += "#version 120\n";

    fragment_shader += "varying vec2 v_texcoord;\n";
    fragment_shader += "varying vec4 v_color0;\n";
    fragment_shader += "varying vec4 v_color1;\n";

    fragment_shader += "uniform sampler2D texture0;\n";

    //fragment_shader += "out vec4 frag_color;\n";

    fragment_shader += "void main() {\n";
    fragment_shader += "  float r = texture(texture0, v_texcoord).r;\n";
    //fragment_shader += "  float r = v_texcoord.x;\n";
    fragment_shader += "  gl_FragColor = v_color0 * r + v_color1 * (1.0 - r);\n";
    //fragment_shader += "  frag_color = r * vec4(1.0,0.0,0.0,1.0) + (1.0 - r) * vec4(0.0,0.0,1.0,1.0);\n";
    fragment_shader += "}\n";
#endif

    GLuint vertex_shader_identifier = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragment_shader_identifier = glCreateShader(GL_FRAGMENT_SHADER);

    compile_shader(vertex_shader_identifier, vertex_shader);
    compile_shader(fragment_shader_identifier, fragment_shader);

    glAttachShader(_program_identifier, vertex_shader_identifier);
    glAttachShader(_program_identifier, fragment_shader_identifier);

    glBindAttribLocation(_program_identifier, 0, "vertex");
    glBindAttribLocation(_program_identifier, 1, "texcoord");
    glBindAttribLocation(_program_identifier, 2, "color0");
    glBindAttribLocation(_program_identifier, 3, "color1");

    link_program();

    glDetachShader(_program_identifier, vertex_shader_identifier);
    glDetachShader(_program_identifier, fragment_shader_identifier);

    glDeleteShader(fragment_shader_identifier);
    glDeleteShader(vertex_shader_identifier);

    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cout << "Error: " << error << std::endl;
    }

    gettimeofday(&_lastTime, 0);
}

void GameView::resizeGL(int width, int height)
{
    glViewport(0,0,width,height);

    glBindTexture(GL_TEXTURE_2D, _texture_identifier);
    glUseProgram(_program_identifier);
    GLuint mvp_position = glGetUniformLocation(_program_identifier, "mvp");
    float znear = -10.0f;
    float zfar = 10.0f;
    float left = -(float)width / 2.0f;
    float right = -left;
    float top = (float)height / 2.0f;
    float bottom = -top;
    float mvp[16] = {
        2.0f / (right -left),    0.0f,                   0.0f,                   -(right+left)/(right-left),
        0.0f,                    2.0f / (top-bottom),    0.0f,                   -(top+bottom)/(top-bottom),
        0.0f,                    0.0f,                   -2.0f / (zfar-znear),   -(zfar+znear)/(zfar-znear),
        0.0f,                    0.0f,                   0.0f,                   1.0f
    };
    glUniformMatrix4fv(mvp_position, 1, GL_FALSE, mvp);
}

void GameView::paintGL()
{
    timeval tmpTime = _lastTime;
    gettimeofday(&_lastTime, 0);
    float dt = (float)(_lastTime.tv_sec - tmpTime.tv_sec) + (float)(_lastTime.tv_usec - tmpTime.tv_usec) * 0.000001f;

    _game.update(dt);
    update_vertex_buffer();

    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(_program_identifier);
    glBindVertexArray(_vao);
    glDrawArrays(GL_TRIANGLES, 0, GameClient::ViewWidth * GameClient::ViewHeight * 6);

    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cout << "Error: " << error << std::endl;
    }
}

void GameView::showEvent(QShowEvent * /* event */)
{
    _timer_id = startTimer(1);
}

void GameView::timerEvent(QTimerEvent *event)
{
    if(event->timerId() == _timer_id)
    {
        update();
    }
    else {
        QGLWidget::timerEvent(event);
    }
}

void GameView::keyPressEvent(QKeyEvent *event)
{
    std::string text = event->text().toStdString();
    //std::cout << text << std::endl;
    _game.text_input(text);
    switch (event->key()) {
        case Qt::Key_Left: _game.key_down(KEY_LEFT); break;
        case Qt::Key_Right: _game.key_down(KEY_RIGHT); break;
        case Qt::Key_Up: _game.key_down(KEY_UP); break;
        case Qt::Key_Down: _game.key_down(KEY_DOWN); break;
        case Qt::Key_Comma: _game.key_down(KEY_COMMA); break;
        case Qt::Key_Period: _game.key_down(KEY_PERIOD); break;
        case Qt::Key_W: _game.key_down(KEY_W); break;
        case Qt::Key_A: _game.key_down(KEY_A); break;
        case Qt::Key_S: _game.key_down(KEY_S); break;
        case Qt::Key_D: _game.key_down(KEY_D); break;
        case Qt::Key_I: _game.key_down(KEY_I); break;
        case Qt::Key_Space: _game.key_down(KEY_SPACE); break;
        case Qt::Key_Backspace: _game.text_delete(); break;
        case Qt::Key_Enter: case Qt::Key_Return: _game.text_done(); break;
        default: QGLWidget::keyPressEvent(event);
    }
}

void GameView::keyReleaseEvent(QKeyEvent *event) {
    switch (event->key()) {
        case Qt::Key_Left: _game.key_up(KEY_LEFT); break;
        case Qt::Key_Right: _game.key_up(KEY_RIGHT); break;
        case Qt::Key_Up: _game.key_up(KEY_UP); break;
        case Qt::Key_Down: _game.key_up(KEY_DOWN); break;
        case Qt::Key_Comma: _game.key_up(KEY_COMMA); break;
        case Qt::Key_Period: _game.key_up(KEY_PERIOD); break;
        case Qt::Key_W: _game.key_up(KEY_W); break;
        case Qt::Key_A: _game.key_up(KEY_A); break;
        case Qt::Key_S: _game.key_up(KEY_S); break;
        case Qt::Key_D: _game.key_up(KEY_D); break;
        case Qt::Key_I: _game.key_up(KEY_I); break;
        case Qt::Key_Space: _game.key_up(KEY_SPACE); break;
        default: QGLWidget::keyPressEvent(event);
    }
}

