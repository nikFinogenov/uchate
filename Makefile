OS := $(shell uname)

.PHONY: server client libmx

all: install

server:
	@$(MAKE) -C server/

client:
	@$(MAKE) -C client/

libmx:
	@$(MAKE) -C libmx/

install:
	@$(MAKE) -C libmx/
	@$(MAKE) -C client/
	@$(MAKE) -C server/

clean:
	@$(MAKE) -C server/ clean
	@$(MAKE) -C client/ clean
	@$(MAKE) -C libmx/ clean

uninstall:
	@$(MAKE) -C server/ uninstall
	@$(MAKE) -C client/ uninstall
	@$(MAKE) -C libmx/ uninstall

reinstall: uninstall install