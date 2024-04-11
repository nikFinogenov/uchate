CJSON_OBJ = cJSON.o
UTILS_OBJ = cJSON_Utils.o
CJSON_LIBNAME = libcjson
UTILS_LIBNAME = libcjson_utils

LDLIBS = -lm

CJSON_SO_LDFLAG=-Wl,-soname=$(CJSON_LIBNAME).so
UTILS_SO_LDFLAG=-Wl,-soname=$(UTILS_LIBNAME).so

DESTDIR = 
INCLUDE_PATH ?= include/cjson
LIBRARY_PATH ?= lib

INSTALL_INCLUDE_PATH = $(DESTDIR)/$(INCLUDE_PATH)
INSTALL_LIBRARY_PATH = $(DESTDIR)/$(LIBRARY_PATH)

INSTALL ?= cp -a

CC = gcc -std=c89

# validate gcc version for use fstack-protector-strong
MIN_GCC_VERSION = "4.9"
GCC_VERSION := "`$(CC) -dumpversion`"
IS_GCC_ABOVE_MIN_VERSION := $(shell expr "$(GCC_VERSION)" ">=" "$(MIN_GCC_VERSION)")
ifeq "$(IS_GCC_ABOVE_MIN_VERSION)" "1"
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
	$(CC) -shared -o $@ $< $(CJSON_SO_LDFLAG) $(LDFLAGS)
#cJSON_Utils
$(UTILS_SHARED): $(UTILS_OBJ)
	$(CC) -shared -o $@ $< $(CJSON_OBJ) $(UTILS_SO_LDFLAG) $(LDFLAGS)

#objects
#cJSON
$(CJSON_OBJ): cJSON.c cJSON.h
#cJSON_Utils
$(UTILS_OBJ): cJSON_Utils.c cJSON_Utils.h cJSON.h

#uninstall
#cJSON
uninstall-cjson: uninstall-utils
	$(RM) $(INSTALL_LIBRARY_PATH)/$(CJSON_SHARED)
	$(RM) $(INSTALL_INCLUDE_PATH)/cJSON.h
	
#cJSON_Utils
uninstall-utils:
	$(RM) $(INSTALL_LIBRARY_PATH)/$(UTILS_SHARED)
	$(RM) $(INSTALL_INCLUDE_PATH)/cJSON_Utils.h

uninstall: uninstall-utils uninstall-cjson

clean:
	$(RM) $(CJSON_OBJ) $(UTILS_OBJ)
	$(RM) $(CJSON_SHARED) $(CJSON_STATIC)
	$(RM) $(UTILS_SHARED) $(UTILS_STATIC)
