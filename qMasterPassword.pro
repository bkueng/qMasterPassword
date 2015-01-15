TEMPLATE = app
TARGET = qMasterPassword
INCLUDEPATH += include


Debug:DEFINES += _DEBUG
DEFINES += APP_NAME=\\\"$(TARGET)\\\"
CONFIG += debug_and_release \
	c++11

LIBS += -lscrypt -lcrypto

# Input
HEADERS += include/*.h
SOURCES += src/*.cpp

