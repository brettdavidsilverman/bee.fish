all:
	- mkdir build
	cd code && make $(DEBUG)
	
test:
	- mkdir build
	cd code && make $(DEBUG) test
	
b-string:
	- mkdir build
	cd code/b-string && make $(DEBUG)
	
power-encoding:
	- mkdir build
	cd code/power-encoding && make $(DEBUG)
	
id:
	- mkdir build
	cd code/Id && make $(DEBUG)
	

parser:
	-  mkdir build
	cd code/parser && make $(DEBUG)


json:
	-  mkdir build
	cd code/json && make $(DEBUG)

Database:
	-  mkdir build
	cd code/Database && make $(DEBUG)

Script:
	-  mkdir build
	cd code/Script && make $(DEBUG)

web-request:
	-  mkdir build
	cd code/web-request && make $(DEBUG)
	
WebRequest:
	-  mkdir build
	cd code/WebRequest && make $(DEBUG)

WebServer:
	-  mkdir build
	cd code/WebServer && make $(DEBUG)
	
#DBServer:
#	#-  mkdir build
#	#cd code/DBServer && make $(DEBUG)

clean:
	rm -f -r build
	rm -f large2a.json
	rm -f large2b.json
	rm -f large2.json

debug:	DEBUG = debug
debug:	CFLAGS += -g -DDEBUG
debug:	PORT = 8000
debug:	all
