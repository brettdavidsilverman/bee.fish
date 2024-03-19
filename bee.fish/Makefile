all:
	- mkdir build
	cd code && make $(DEBUG)

install:
	- mkdir build
	cd code/DBServer && make install

test:
	-  mkdir build
	cd code && make test

power-encoding:
	- mkdir build
	cd code/power-encoding && make $(DEBUG)
	
b-string:
	- mkdir build
	cd code/b-string && make $(DEBUG)
	
parser:
	-  mkdir build
	cd code/parser && make $(DEBUG)

PowerEncoding:
	-  mkdir build
	cd code/PowerEncoding && make $(DEBUG)

Database:
	-  mkdir build
	cd code/Database && make $(DEBUG)

json:
	-  mkdir build
	cd code/json && make $(DEBUG)

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
	rm -f large2.json

debug:	clean
debug:	DEBUG = debug
debug:	CFLAGS += -g -DDEBUG
debug:	PORT = 8000
debug:	install
