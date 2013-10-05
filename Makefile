##
# Copyright (C) 2010-2013 Beat Küng <beat-kueng@gmx.net>
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
HEADERS := $(wildcard include/*.h)

# Generic flags for the C/CPP compiler.
CFLAGS := 			-pipe -O2 -Wall -D'APP_NAME="$(APP_NAME)"'
CFLAGS_debug := 	-pipe -g -Wall -D'APP_NAME="$(APP_NAME)"' -D_DEBUG
CXXFLAGS := 		$(CFLAGS)
CXXFLAGS_debug := 	$(CFLAGS_debug)
CCt := 				gcc
CXXt := 			g++
LIBS :=				-lm
INCPATH :=			-Iinclude

# whether or not to generate & use include dependency files
USE_DEP_FILES :=	1


# we want to be able to override CC & CXX. But we cannot simply check whether CC
# is set, because make sets it implicitly and -R option from inside Makefile
# seems not to work
ifeq ($(CC_override),)
CC := $(CCt)
CXX := $(CXXt)
endif
LD := 				$(CXX)


.PHONY: all clean debug $(APP_NAME) analyze format format_clean build_clean
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
	@$(CC) -MM -MG $(INCPATH) $(CFLAGS) $< | \
		sed -e "s@^\(.*\)\.o:@$(dir $@)\1.d $(dir $@)\1.o:@" > $@
build/%.d: %.cpp
	@ mkdir -p $(dir $@)
	@$(CXX) -MM -MG $(INCPATH) $(CXXFLAGS) $< | \
		sed -e "s@^\(.*\)\.o:@$(dir $@)\1.d $(dir $@)\1.o:@" > $@
build_dbg/%.d: %.c
	@ mkdir -p $(dir $@)
	@$(CC) -MM -MG $(INCPATH) $(CFLAGS_debug) $< | \
		sed -e "s@^\(.*\)\.o:@$(dir $@)\1.d $(dir $@)\1.o:@" > $@
build_dbg/%.d: %.cpp
	@ mkdir -p $(dir $@)
	@$(CXX) -MM -MG $(INCPATH) $(CXXFLAGS_debug) $< | \
		sed -e "s@^\(.*\)\.o:@$(dir $@)\1.d $(dir $@)\1.o:@" > $@
endif # ($(USE_DEP_FILES),1)


# Build targets
build/%.o: %.cpp
	@ mkdir -p $(dir $@)
	$(CXX) -c $(CXXFLAGS) $(INCPATH) $*.cpp -o $@
build/%.o: %.c
	@ mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) $(INCPATH) $*.c -o $@
build_dbg/%.o: %.cpp
	@ mkdir -p $(dir $@)
	$(CXX) -c $(CXXFLAGS_debug) $(INCPATH) $*.cpp -o $@
build_dbg/%.o: %.c
	@ mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS_debug) $(INCPATH) $*.c -o $@

# Link targets
$(APP_NAME): $(patsubst %.cpp, build/%.o, $(SOURCES_cpp)) \
	$(patsubst %.c, build/%.o, $(SOURCES_c))
	$(LD) -o $@ $^ $(LIBS)
$(APP_NAME)_dbg: $(patsubst %.cpp, build_dbg/%.o, $(SOURCES_cpp)) \
	$(patsubst %.c, build_dbg/%.o, $(SOURCES_c))
	$(LD) -o $@ $^ $(LIBS)

# static analyzer (using clang static analyzer)
# another option would be cppcheck: cppcheck -Iinclude src
analyze: build_clean
	[ ! -d analysis ] && mkdir analysis; \
		scan-build --use-analyzer=`which clang` -o analysis $(MAKE) CC_override=1

# code formatting
# if you change the formatting, change it also in the git_hooks
format:
	astyle \
		--lineend=linux --indent=force-tab=4 --indent-col1-comments \
		--pad-header --pad-oper --unpad-paren \
		--fill-empty-lines --align-pointer=type \
		--align-reference=type --max-code-length=80 --break-after-logical \
		--keep-one-line-blocks --keep-one-line-statements --style=kr \
		--suffix=.astyle.orig \
		$(HEADERS) $(SOURCES_cpp) $(SOURCES_c)

format_clean:
	find src include -name '*.astyle.orig' -exec rm -f {} \;

build_clean:
	rm -rf build build_dbg $(APP_NAME)

clean: build_clean format_clean

