TEMPLATE = app

CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt
CONFIG+=thread
LIBS+=-pthread

INCLUDEPATH += ../

SOURCES += \
        Server.cpp \
        ../Socket.cpp
