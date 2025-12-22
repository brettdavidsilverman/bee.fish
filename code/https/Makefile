CC = g++
CFLAGS = -std=c++17 -Wall -fmax-errors=1
BUILD = ../../build
OUTPUT=$(BUILD)/https
DEPS=server.h session.h response.h response-headers.h app.h json-app.h file-system-app.h error-app.h storage-app.h test.h authentication.h version.h Makefile ParseURL.hpp main.cpp ../Config.hpp https.h authentication-app.h Makefile
PORT=443

$(OUTPUT):	$(DEPS)
	@echo "Linking server"
	$(CC) main.cpp $(CFLAGS) -o $(OUTPUT) -lstdc++fs -lssl -lcrypto -lboost_system -lboost_thread -lpthread -latomic
#	sudo setcap cap_net_bind_service=ep $(OUTPUT)
	

$(BUILD):
		mkdir $(BUILD)
		

install:	$(OUTPUT)
	./start.sh $(PORT)
	
test:	$(OUTPUT)
	./stop.sh $(PORT)
	$(OUTPUT) -port $(PORT) -test

start:	$(OUTPUT)
	./start.sh $(PORT)
	
stop:
	./stop.sh $(PORT)
	
	
debug:	DEBUG=debug
debug:	CFLAGS += -g -D DEBUG
debug:	PORT=8000
debug:	$(OUTPUT)


run_debug:	debug
	gdb --args $(OUTPUT) -port $(PORT) -test

clean:
	rm -f session.log
	rm -f *.o
	rm -f test.data
	rm -f trace.txt
	rm -f $(OUTPUT)

