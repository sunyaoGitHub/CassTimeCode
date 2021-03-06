TEMPLATE = app
TARGET = OEUnify
unix:VERSION = 1.0.0
DESTDIR = ../../bin

QT =
CONFIG -= qt
CONFIG += console
DEFINES += 

LIBS += -L../../../bsl/lib
SHAREDLIBS = DDE
QMAKE_RPATHDIR += ../../../bsl/lib

INCLUDEPATH += ../../../bsl/include/ \
            += ../../../3rdparty/   \
            += ../../include/


PRECOMPILED_HEADER += stdafx.h
HEADERS += stdafx.h OEUnify.h
SOURCES += stdafx.cpp OEUnify.cpp main.cpp

include(../../../bsl/include/Common.conf)
