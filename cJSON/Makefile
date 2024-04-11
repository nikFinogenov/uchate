CJSON_OBJ = cJSON.o
UTILS_OBJ = cJSON_Utils.o
CJSON_LIBNAME = libcjson
UTILS_LIBNAME = libcjson_utils

LDLIBS = -lm

CJSON_SO_LDFLAG=-install_name=$(CJSON_LIBNAME).dylib
UTILS_SO_LDFLAG=-install_name=$(UTILS_LIBNAME).dylib

DESTDIR = 
INCLUDE_PATH ?= include/cjson
LIBRARY_PATH ?= lib

INSTALL_INCLUDE_PATH = $(DESTDIR)/$(INCLUDE_PATH)
INSTALL_LIBRARY_PATH = $(DESTDIR)/$(LIBRARY_PATH)

INSTALL ?= cp -a

CC = clang -std=c89

# validate clang version for use fstack-protector-strong
MIN_CLANG_VERSION = "3.4"
CLANG_VERSION := "`$(CC) --version | grep -o '[0-9]*\.[0-9]*' | head -n 1`"
IS_CLANG_ABOVE_MIN_VERSION := $(shell expr "$(CLANG_VERSION)" ">=" "$(MIN_CLANG_VERSION)")
ifeq "$(IS_CLANG_ABOVE_MIN_VERSION)" "1"
    CFLAGS += -fstack-protector-strong
else
    CFLAGS += -fstack-protector
endif

PIC_FLAGS = -fPIC
R_CFLAGS = $(PIC_FLAGS) -pedantic -Wall -Werror -Wstrict-prototypes -Wwrite-strings -Wshadow -Winit-self -Wcast-align -Wformat=2 -Wmissing-prototypes -Wstrict-overflow=2 -Wcast-qual -Wc++-compat -Wundef -Wswitch-default -Wconversion $(CFLAGS)

uname := $(shell sh -c 'uname -s 2>/dev/null || echo false')

#library file extensions
SHARED = so
STATIC = a

## create dynamic (shared) library on Darwin (base OS for MacOSX and IOS)
ifeq (Darwin, $(uname))
	SHARED = dylib
endif

#cJSON library names
CJSON_SHARED = $(CJSON_LIBNAME).$(SHARED)
CJSON_STATIC = $(CJSON_LIBNAME).$(STATIC)

#cJSON_Utils library names
UTILS_SHARED = $(UTILS_LIBNAME).$(SHARED)
UTILS_STATIC = $(UTILS_LIBNAME).$(STATIC)

SHARED_CMD = $(CC) -shared -o

.PHONY: all shared static clean install

all: shared static

shared: $(CJSON_SHARED) $(UTILS_SHARED)

static: $(CJSON_STATIC) $(UTILS_STATIC)

.c.o:
	$(CC) -c $(R_CFLAGS) $<

#static libraries
#cJSON
$(CJSON_STATIC): $(CJSON_OBJ)
	$(AR) rcs $@ $<
#cJSON_Utils
$(UTILS_STATIC): $(UTILS_OBJ)
	$(AR) rcs $@ $<

#shared libraries
#cJSON
$(CJSON_SHARED): $(CJSON_OBJ)
	$(CC) -shared -o $@ $<
	install_name_tool -id $(CJSON_LIBNAME).dylib $@
#cJSON_Utils
$(UTILS_SHARED): $(UTILS_OBJ) $(CJSON_OBJ)
	$(CC) -shared -o $@ $< -L. -lcjson
	install_name_tool -id $(UTILS_LIBNAME).dylib $@

#objects
#cJSON
$(CJSON_OBJ): cJSON.c cJSON.h
#cJSON_Utils
$(UTILS_OBJ): cJSON_Utils.c cJSON_Utils.h