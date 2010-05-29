##
# Copyright (C) 2010 Beat Küng <beat-kueng@gmx.net>
# 
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; version 2 of the License.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#


# Disable make's built-in rules.
MAKE += -RL --no-print-directory
SHELL := $(shell which bash)


# Name for the application to produce.
APP_NAME := template

# Listings of source files for the different executables.
SOURCES := $(wildcard *.cpp) $(wildcard *.c)


# Generic flags for the C/CPP compiler.
CFLAGS := 			-pipe -O2 -Wall -D'APP_NAME="$(APP_NAME)"'
CFLAGS_debug := 		-pipe -g -Wall -D'APP_NAME="$(APP_NAME)"' -D_DEBUG
CXXFLAGS := 			$(CFLAGS)
CXXFLAGS_debug := 		$(CFLAGS_debug)
GCC := 				gcc
GXX := 				g++
LD := 				$(GXX)
LIBS :=				-lm
INCPATH :=			




.PHONY: all clean debug $(APP_NAME)
all: $(APP_NAME)

debug: $(APP_NAME)_dbg
	mv $(APP_NAME)_dbg $(APP_NAME)


# Build targets
build/%.o: %.cpp
	@ mkdir -p $(dir $@)
	$(GXX) -c $(CXXFLAGS) $*.cpp -o $@
build_c/%.o: %.c
	@ mkdir -p $(dir $@)
	$(GCC) -c $(CFLAGS) $*.c -o $@
build_dbg/%.o: %.cpp
	@ mkdir -p $(dir $@)
	$(GXX) -c $(CXXFLAGS_debug) $*.cpp -o $@
build_c_dbg/%.o: %.c
	@ mkdir -p $(dir $@)
	$(GCC) -c $(CFLAGS_debug) $*.c -o $@

# Link targets
$(APP_NAME): $(patsubst %.cpp, build/%.o, $(patsubst %.c, build_c/%.o, $(SOURCES)))
	$(LD) -o $@ $^ $(LIBS)
$(APP_NAME)_dbg: $(patsubst %.cpp, build_dbg/%.o, $(patsubst %.c, build_c_dbg/%.o, $(SOURCES)))
	$(LD) -o $@ $^ $(LIBS)


# Cleans the module.
clean:
	rm -rf build build_dbg build_c build_c_dbg $(APP_NAME)
