TEMPLATE = app
QT = core network
TARGET = vlcstreamer
TARGET_x.y.z=0.0.1
CONFIG *= QT CONSOLE debug_and_release debug_and_release_target

include(../common.pri)

QMAKE_CLEAN += $${DESTDIR_TARGET}
QMAKE_DISTCLEAN += debug/ release/

target.path = /usr/bin
INSTALLS += target

# Input
HEADERS += Utils.h \
           VlcEncoder.h \
           VlcEncodingSystem.h \
           VlcStreamerApp.h \
           VlcStreamerConnection.h \
		   VlcStreamerFileSystem.h \
           VlcStreamerServer.h \
		   VlcStreamerService.h \

SOURCES += main.cpp \
           Utils.cpp \
           VlcEncoder.cpp \
           VlcEncodingSystem.cpp \
           VlcStreamerApp.cpp \
           VlcStreamerConnection.cpp \
		   VlcStreamerFileSystem.cpp \
           VlcStreamerServer.cpp \
		   VlcStreamerService.cpp \

LIBS += -lqjson

include(../third-party/qtservice-2.6_1-opensource/src/qtservice.pri)

