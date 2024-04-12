OS := $(shell uname)

ifeq ($(OS),Darwin)
    # If MacOS, use clang
    cJSON_MAKEFILE := Makefile-macos.mk
else
    # Otherwise, assume Ubuntu and use GCC
    cJSON_MAKEFILE := Makefile-ubuntu.mk
endif


.PHONY: server client libmx

all: install

server:
	@$(MAKE) -C server/

client:
	@$(MAKE) -C client/

libmx:
	@$(MAKE) -C libmx/

install:
	@cp -r cJSON/${cJSON_MAKEFILE} cJSON/Makefile
	@$(MAKE) -C libmx/
	@$(MAKE) -C cJSON/
	@$(MAKE) -C client/
	@$(MAKE) -C server/

uninstall:
	@$(MAKE) -C server/ uninstall
	@$(MAKE) -C client/ uninstall
	@$(MAKE) -C libmx/ uninstall
	@$(MAKE) -C cJSON/ uninstall

reinstall: uninstall install