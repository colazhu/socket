#QT += core
#QT -= gui

TARGET = TestSocket
CONFIG += console
CONFIG -= app_bundle gui core
CONFIG += c++11

TEMPLATE = app
DEFINES += OS_ANDROID=0

DESTDIR  = ../out

LIBS += -lglfw -lGLESv2 -lEGL
LIBS += -L$$DESTDIR

SOURCES += main.cpp \
    SocketClient.cpp \
    Log.cpp \
    SocketThread.cpp \
    Condition.cpp \
    SocketServer.cpp

HEADERS += \
    SocketClient.h \
    Log.h \
    Base.h \
    SocketThread.h \
    Condition.h \
    SocketServer.h

DISTFILES += \
    Android.mk

