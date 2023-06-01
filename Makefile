build:	clean
	mkdir build
	cd code && make $(DEBUG)

install:	build
	./start.sh $(PORT)

test:	build
	./build/Parser -test
	./build/JSON -test
	./build/WebRequest -test
	- ./build/WebServer -test

clean:
	rm -f -r build

debug:	clean
debug:	DEBUG = debug
debug:	CFLAGS += -g -DDEBUG
debug:	PORT = 8080
debug:	install
