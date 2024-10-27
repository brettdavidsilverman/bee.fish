CC = g++
CFLAGS = -std=c++17 -Wall -fmax-errors=1
BUILD = ../../build
OUTPUT=$(BUILD)/https
DEPS=server.h session.h response.h response-headers.h app.h storage-app.h json-app.h file-system-app.h error-app.h test.h authentication.h version.h Makefile ParseURL.hpp main.cpp ../Config.hpp storage.h https.h authentication-app.h Makefile
PORT=443

all:	install

$(OUTPUT): $(DEPS)
	@echo "Linking server"
	$(CC) main.cpp $(CFLAGS) -o $(OUTPUT) -lstdc++fs -lssl -lcrypto -lboost_system -lboost_thread -lpthread -latomic
#	sudo setcap cap_net_bind_service=ep $(OUTPUT)
	

install:	test
	./start.sh $(PORT)
	
test:	$(OUTPUT)
	./stop.sh $(PORT)
	$(OUTPUT) -port $(PORT) -test

start:	$(OUTPUT)
	./start.sh $(PORT)
	
stop:
	./stop.sh $(PORT)
	
	
debug:	DEBUG=debug
debug:	CFLAGS += -g -DDEBUG
debug:	$(OUTPUT)
debug:	PORT=8000
debug:	test
debug:	install


run_debug:	DEBUG=debug
run_debug:	CFLAGS += -g -DDEBUG
run_debug:	$(OUTPUT)
	gdb --args $(OUTPUT) -port 8000 -test

clean:
	rm -f session.log
	rm -f *.o
	rm -f test.data
	rm -f trace.txt
	rm -f $(OUTPUT)

