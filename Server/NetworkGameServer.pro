TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp GameServer.cpp \
    ../World.cpp \
    ../VirtualConnection.cpp \
    ../PlayerView.cpp \
    ../PlayerInput.cpp \
    ../Noise2D.cpp \
    ../Network.cpp

HEADERS += \
    GameServer.h \
    ../World.h \
    ../VirtualConnection.h \
    ../PlayerView.h \
    ../PlayerInput.h \
    ../Noise2D.h \
    ../Network.h \
    ../defines.h

