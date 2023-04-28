all: build Makefile
	cd code && make $(DEBUG)

build:
	mkdir build

install: all
	sudo setcap cap_net_bind_service=ep ./build/WebServer
	./start.sh

test: all ./code/WebServer/test.hpp Makefile
	./build/WebServer -test

clean:
	rm -f -r build

debug:	clean
debug:	DEBUG = debug
debug:	CFLAGS += -g -DDEBUG
debug:	all