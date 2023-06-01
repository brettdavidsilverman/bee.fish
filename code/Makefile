all:
	cd Parser && make $(DEBUG)
	cd JSON && make $(DEBUG)
	cd WebRequest && make $(DEBUG)
	cd WebServer && make $(DEBUG)


debug:	DEBUG = debug
debug:	CFLAGS += -g -D DEBUG
debug:	all

clean:
	cd .. && make clean