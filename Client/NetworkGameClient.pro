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
           ../World.cpp \
           ../Noise2D.cpp \
           ../Network.cpp \
           ../VirtualConnection.cpp \
           ../PlayerView.cpp \
           ../PlayerInput.cpp \
           GameClient.cpp \
    #../Server/GameServer.cpp \
    NetworkClient.cpp \
    ../Entity.cpp \
    ../PlayerController.cpp \
    ../AIController.cpp

HEADERS += mainwindow.h \
           gameview.h \
           ../World.h \
           ../Noise2D.h \
           ../defines.h \
           ../Network.h \
           ../VirtualConnection.h \
           ../PlayerView.h \
           ../PlayerInput.h \
           GameClient.h \
    #../Server/GameServer.h \
    NetworkClient.h \
    ../Entity.h \
    ../PlayerController.h \
    ../AIController.h

RESOURCES = resources.qrc
