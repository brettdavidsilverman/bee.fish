CC = g++
CFLAGS = -std=c++17 -Wall -fmax-errors=1
BUILD = ../../build
OUTPUT=$(BUILD)/https
DEPS=server.h session.h response.h response-headers.h app.h storage-app.h json-app.h file-system-app.h error-app.h test.h authentication.h version.h Makefile ParseURL.hpp main.cpp ../Config.hpp storage.h https.h authentication-app.h $(BUILD)/parser $(BUILD)/json $(BUILD)/web-request $(BUILD)/Database $(BUILD)/power-encoding $(BUILD)/script Makefile
#PORT=443

$(OUTPUT): $(DEPS)
	@echo "Linking server"
	$(CC) main.cpp $(CFLAGS) -o $(OUTPUT) -lstdc++fs -lssl -lcrypto -lboost_system -lboost_thread -lpthread -latomic
	sudo setcap cap_net_bind_service=ep $(OUTPUT)
	
install:	$(OUTPUT)
	./start.sh $(PORT)
	
test:	$(OUTPUT)
	./stop.sh $(PORT)
	$(OUTPUT) -test -port $(PORT)

start:	$(OUTPUT)
	./start.sh $(PORT)
	
stop:
	./stop.sh $(PORT)
	
$(BUILD)/parser:
	cd ../parser && make $(DEBUG)
	
$(BUILD)/json:
	cd ../json && make $(DEBUG)
	
$(BUILD)/power-encoding:
	cd ../power-encoding && make $(DEBUG)

$(BUILD)/Database:
	cd ../database && make $(DEBUG)
	
$(BUILD)/web-request:
	cd ../web-request && make $(DEBUG)

$(BUILD)/script:
	cd ../Script && make $(DEBUG)
	
debug:	$(OUTPUT)
debug:	DEBUG = debug
debug:	CFLAGS += -g -DDEBUG
#debug:	PORT = 8000

run_debug:	debug
	gdb --args $(OUTPUT) -port 8000 -test

clean:
	rm -f session.log
	rm -f *.o
	rm -f test.data
	rm -f trace.txt
	rm -f $(OUTPUT)

