TEMPLATE = app
TARGET = JMConvert
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
HEADERS += stdafx.h ArticleConvert.h ArticleParse.h
SOURCES += stdafx.cpp ArticleConvert.cpp ArticleParse.cpp main.cpp

include(../../../bsl/include/Common.conf)
