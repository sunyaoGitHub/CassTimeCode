TEMPLATE = app
TARGET = VendorConvert
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
HEADERS += stdafx.h VendorConvert.h VendorParse.h
SOURCES += stdafx.cpp VendorConvert.cpp VendorParse.cpp main.cpp

include(../../../bsl/include/Common.conf)
