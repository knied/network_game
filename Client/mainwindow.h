// =================================================================
// Author: Kristof Niederholtmeyer
//

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "gameview.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

    GameView* _game_view;
    
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
};

#endif // MAINWINDOW_H
