QT = core network
TEMPLATE = app
TARGET = vlcstreamer
CONFIG += QT CONSOLE

# Input
HEADERS += VlcEncoder.h \
           VlcEncodingSystem.h \
           VlcStreamerApp.h \
           VlcStreamerConnection.h \
		   VlcStreamerFileSystem.h \
           VlcStreamerServer.h \

SOURCES += main.cpp \
           VlcEncoder.cpp \
           VlcEncodingSystem.cpp \
           VlcStreamerApp.cpp \
           VlcStreamerConnection.cpp \
		   VlcStreamerFileSystem.cpp \
           VlcStreamerServer.cpp \

LIBS += -lqjson

