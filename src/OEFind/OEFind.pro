TEMPLATE = app
TARGET = OEFind
unix:VERSION = 1.0.0
DESTDIR = ../../bin


QT += core widgets gui
CONFIG += qt warn_on
DEFINES += 

LIBS += -L"../../lib" \
     += -L"../../../bsl/lib"

INCLUDEPATH += ../../../bsl/include \
			+= ../../../3rdparty \
			+= ../../include \


PRECOMPILED_HEADER += stdafx.h
HEADERS += stdafx.h .h oefind.h OEFindDialog.h
SOURCES += stdafx.cpp oefind.cpp OEFindDialog.cpp main.cpp
RESOURCES += oefind.qrc
include(../../../bsl/include/Common.conf)


MOC_DIR += ./GeneratedFiles/release
RCC_DIR += ./GeneratedFiles

win32:RC_FILE = OEFind.rc
