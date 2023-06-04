all:
	- mkdir build
	cd code && make $(DEBUG)

install:	all
	./start.sh $(PORT)

test:	all
	./build/Parser -test
	./build/JSON -test
	./build/PowerEncoding -test
	./build/Database -test
	./build/WebRequest -test
	./build/WebServer -test
	./build/DBServer -test

clean:
	rm -f -r build

debug:	clean
debug:	DEBUG = debug
debug:	CFLAGS += -g -DDEBUG
debug:	PORT = 8080
debug:	install
