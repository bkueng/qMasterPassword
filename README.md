### template ###

this serves as a starting point for new projects, for linux/windows, with Qt,
Makefile, whatever, ...

to start:
- change licence in files if not GPL
- change project specific things in config.h and in Makefile
- execute git_hooks/install.sh

the do-tag script is used under linux to automatically change version.h file and
make a git commit


for Qt projects
---------------
run qmake -project
add these lines to the .pro file:

TEMPLATE = app
TARGET = <app name>
Debug:DEFINES += _DEBUG
DEFINES += APP_NAME=\\\"$(TARGET)\\\"
CONFIG += debug_and_release
# for profiling (under linux, using gprof):
#QMAKE_CXXFLAGS_DEBUG += -pg
#QMAKE_LFLAGS_DEBUG += -pg

UI_SOURCES_DIR = src
UI_HEADERS_DIR = include

and change these variables:
HEADERS += include/*.h
SOURCES += src/*.cpp

run make


Copyright 2010-2015 Beat Küng <beat-kueng@gmx.net>
