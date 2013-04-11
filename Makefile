##
# Copyright (C) 2010-2011 Beat Küng <beat-kueng@gmx.net>
# 
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; version 3 of the License.
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
SOURCES_cpp := $(wildcard src/*.cpp)
SOURCES_c:= $(wildcard src/*.c)

# Generic flags for the C/CPP compiler.
CFLAGS := 			-pipe -O2 -Wall -D'APP_NAME="$(APP_NAME)"'
CFLAGS_debug := 	-pipe -g -Wall -D'APP_NAME="$(APP_NAME)"' -D_DEBUG
CXXFLAGS := 		$(CFLAGS)
CXXFLAGS_debug := 	$(CFLAGS_debug)
GCC := 				gcc
GXX := 				g++
LD := 				$(GXX)
LIBS :=				-lm
INCPATH :=			-Iinclude

# whether or not to generate & use include dependency files
USE_DEP_FILES :=	1



.PHONY: all clean debug $(APP_NAME)
all: $(APP_NAME)

debug: $(APP_NAME)_dbg
	mv $(APP_NAME)_dbg $(APP_NAME)


# Dependency targets & includes
DEP_cpp := $(patsubst %.cpp, build/%.d,$(SOURCES_cpp));
DEP_c:= $(patsubst %.c, build/%.d,$(SOURCES_c));
DEP_cpp_dbg := $(patsubst %.cpp, build_dbg/%.d,$(SOURCES_cpp));
DEP_c_dbg:= $(patsubst %.c, build_dbg/%.d,$(SOURCES_c));

ifeq ($(strip $(USE_DEP_FILES)),1)
-include $(DEP_cpp) $(DEP_c)
-include $(DEP_cpp_dbg) $(DEP_c_dbg)
build/%.d: %.c
	@ mkdir -p $(dir $@)
	@$(GCC) -MM -MG $(INCPATH) $(CFLAGS) $< | \
		sed -e "s@^\(.*\)\.o:@$(dir $@)\1.d $(dir $@)\1.o:@" > $@
build/%.d: %.cpp
	@ mkdir -p $(dir $@)
	@$(GXX) -MM -MG $(INCPATH) $(CXXFLAGS) $< | \
		sed -e "s@^\(.*\)\.o:@$(dir $@)\1.d $(dir $@)\1.o:@" > $@
build_dbg/%.d: %.c
	@ mkdir -p $(dir $@)
	@$(GCC) -MM -MG $(INCPATH) $(CFLAGS_debug) $< | \
		sed -e "s@^\(.*\)\.o:@$(dir $@)\1.d $(dir $@)\1.o:@" > $@
build_dbg/%.d: %.cpp
	@ mkdir -p $(dir $@)
	@$(GXX) -MM -MG $(INCPATH) $(CXXFLAGS_debug) $< | \
		sed -e "s@^\(.*\)\.o:@$(dir $@)\1.d $(dir $@)\1.o:@" > $@
endif # ($(USE_DEP_FILES),1)


# Build targets
build/%.o: %.cpp
	@ mkdir -p $(dir $@)
	$(GXX) -c $(CXXFLAGS) $(INCPATH) $*.cpp -o $@
build/%.o: %.c
	@ mkdir -p $(dir $@)
	$(GCC) -c $(CFLAGS) $(INCPATH) $*.c -o $@
build_dbg/%.o: %.cpp
	@ mkdir -p $(dir $@)
	$(GXX) -c $(CXXFLAGS_debug) $(INCPATH) $*.cpp -o $@
build_dbg/%.o: %.c
	@ mkdir -p $(dir $@)
	$(GCC) -c $(CFLAGS_debug) $(INCPATH) $*.c -o $@

# Link targets
$(APP_NAME): $(patsubst %.cpp, build/%.o, $(SOURCES_cpp)) \
	$(patsubst %.c, build/%.o, $(SOURCES_c))
	$(LD) -o $@ $^ $(LIBS)
$(APP_NAME)_dbg: $(patsubst %.cpp, build_dbg/%.o, $(SOURCES_cpp)) \
	$(patsubst %.c, build_dbg/%.o, $(SOURCES_c))
	$(LD) -o $@ $^ $(LIBS)


# Cleans the module.
clean:
	rm -rf build build_dbg $(APP_NAME)
