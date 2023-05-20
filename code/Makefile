all:
	cd Parser && make $(DEBUG)
	cd WebServer && make $(DEBUG)

debug:	DEBUG = debug
debug:	CFLAGS += -g -D DEBUG
debug:	all