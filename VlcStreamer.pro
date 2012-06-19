QT = core network
TEMPLATE = app
TARGET = vlcstreamer
CONFIG += QT CONSOLE

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

include(qtservice-2.6_1-opensource/src/qtservice.pri)

