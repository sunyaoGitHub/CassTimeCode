TEMPLATE = app
TARGET = E2DB
unix:VERSION = 1.0.0
DESTDIR = ../../bin

QT =
CONFIG -= qt
CONFIG += console
DEFINES += 

LIBS += -L../../../bsl/lib
SHAREDLIBS = DDE
QMAKE_RPATHDIR += ../../../bsl/lib

INCLUDEPATH += ../../../bsl/include



PRECOMPILED_HEADER += stdafx.h
HEADERS += stdafx.h E2DB.h
SOURCES += stdafx.cpp E2DB.cpp main.cpp

include(../../../bsl/include/Common.conf)
