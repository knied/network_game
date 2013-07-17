// =================================================================
// Author: Kristof Niederholtmeyer
//

#include "mainwindow.h"
#include <iostream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QGLFormat format;
    format.setVersion(3,2);
    format.setProfile(QGLFormat::CoreProfile);
    _game_view = 0;
    _game_view = new GameView(format);
    if (_game_view)
        setCentralWidget(_game_view);
    setFixedSize(_game_view->prefered_width(),
                 _game_view->prefered_height());
}



MainWindow::~MainWindow()
{
    delete _game_view;
}
