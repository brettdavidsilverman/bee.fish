all:
	- mkdir build
	cd code && make $(DEBUG)

install:	all
	./start.sh $(PORT)

test:
	-  mkdir build
	cd code && make test

Parser:
	-  mkdir build
	cd code/Parser && make $(DEBUG)

PowerEncoding:
	-  mkdir build
	cd code/PowerEncoding && make $(DEBUG)

Database:
	-  mkdir build
	cd code/Database && make $(DEBUG)

JSON:
	-  mkdir build
	cd code/JSON && make $(DEBUG)

WebRequest:
	-  mkdir build
	cd code/WebRequest && make $(DEBUG)

WebServer:
	-  mkdir build
	cd code/WebServer && make $(DEBUG)

DBServer:
	-  mkdir build
	cd code/DBServer && make $(DEBUG)

clean:
	rm -f -r build
	rm -f large2a.json
	rm -f large2b.json

debug:	clean
debug:	DEBUG = debug
debug:	CFLAGS += -g -DDEBUG
debug:	PORT = 8080
debug:	install
