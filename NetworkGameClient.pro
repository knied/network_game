#-------------------------------------------------
#
# Project created by QtCreator 2013-05-14T11:32:58
#
#-------------------------------------------------

QT       += core gui
QT       += widgets
QT       += opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = NetworkGameClient
TEMPLATE = app

SOURCES += main.cpp\
           mainwindow.cpp \
           gameview.cpp \
           game.cpp \
           World.cpp \
           Noise2D.cpp \
           Network.cpp

HEADERS += mainwindow.h \
           gameview.h \
           game.h \
           World.h \
           Noise2D.h \
           defines.h \
           Network.h

RESOURCES = resources.qrc
