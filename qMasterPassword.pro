TEMPLATE = app
TARGET = qMasterPassword
INCLUDEPATH += include .


CONFIG(debug, debug|release):DEFINES += _DEBUG
DEFINES += APP_NAME=\\\"$(TARGET)\\\"
CONFIG += debug_and_release \
	qt \
	c++11
QT += widgets
UI_DIR = ui

# enable unit tests
CONFIG(debug, debug|release) {
	DEFINES += TESTING_SUPPORT
	QT += testlib
	SOURCES += test/test.cpp
	HEADERS   += test/test.h
}

win32 {
	LIBS += -lws2_32 -lscrypt -llibeay32
} else {
	LIBS += -lscrypt -lcrypto
}
ICON = data/icons/app_icon.png

target.path = /usr/bin
INSTALLS += target

# Input
HEADERS += \
		   include/app_settings.h \
		   include/command_line.h \
		   include/config.h \
		   include/crypto.h \
		   include/crypto_functions.h \
		   include/edit_site_widget.h \
		   include/exception.h \
		   include/global.h \
		   include/import_export.h \
		   include/logging.h \
		   include/main_class.h \
		   include/main_window.h \
		   include/pushbutton_delegate.h \
		   include/settings_widget.h \
		   include/ui_helpers.h \
		   include/user.h \
		   include/version.h \
		   include/user_widget.h
FORMS += \
		 ui/edit_site_widget.ui \
		 ui/main_window.ui \
		 ui/settings_widget.ui \
		 ui/user_widget.ui
SOURCES += \
		   src/command_line.cpp \
		   src/crypto.cpp \
		   src/edit_site_widget.cpp \
		   src/exception.cpp \
		   src/global.cpp \
		   src/import_export.cpp \
		   src/logging.cpp \
		   src/main.cpp \
		   src/main_class.cpp \
		   src/main_window.cpp \
		   src/pushbutton_delegate.cpp \
		   src/settings_widget.cpp \
		   src/user.cpp \
		   src/user_widget.cpp
RESOURCES += ui/resources.qrc
win32:RC_FILE = data/icons/app_icon.rc
