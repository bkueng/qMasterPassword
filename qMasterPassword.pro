TEMPLATE = app
TARGET = qMasterPassword
INCLUDEPATH += include


Debug:DEFINES += _DEBUG
DEFINES += APP_NAME=\\\"$(TARGET)\\\"
CONFIG += debug_and_release \
	qt \
	c++11
QT += widgets
UI_DIR = ui

LIBS += -lscrypt -lcrypto
ICON = icons/app_icon.png

# Input
HEADERS   += include/*.h
SOURCES   += src/*.cpp
FORMS     += ui/*.ui
RESOURCES += ui/resources.qrc

