TEMPLATE = app
TARGET = qMasterPassword
INCLUDEPATH += include


Debug:DEFINES += _DEBUG
DEFINES += APP_NAME=\\\"$(TARGET)\\\"
CONFIG += debug_and_release

# Input
HEADERS += include/*.h
SOURCES += src/*.cpp

