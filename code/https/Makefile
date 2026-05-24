CFLAGS = -std=c++17 -Wall -fmax-errors=1
OPTIMISATION = -Ofast
BUILD = ../../build
OUTPUT=$(BUILD)/https
DEPS=server.h session.h response.h response-headers.h response-stream.h app.h json-app.h file-system-app.h storage-app.h not-found-app.h options-app.h test.h authentication.h ../Authentication/authentication.h ../Authentication/server.h version.h Makefile main.cpp ../Config.hpp https.h authentication-app.h $(BUILD)/web-request $(BUILD)/query Makefile

ifeq ("$(PORT)","")
	PORT=8000
endif
	
$(OUTPUT):	$(DEPS)
	@echo "Linking Web Server"
	g++ main.cpp $(CFLAGS) -o $(OUTPUT) -lstdc++fs -lssl -lcrypto -lboost_system -lboost_thread -lboost_locale -lboost_regex -licuuc -lcryptopp
	@if [ "$(PORT)" = "443" ]; then \
		echo "Allowing port $(PORT)"; \
		sudo setcap cap_net_bind_service=ep $(OUTPUT); \
	fi


all:	CFLAGS += $(OPTIMISATION)
all:	$(OUTPUT)

$(BUILD):
	mkdir $(BUILD)

$(BUILD)/web-request:
	make -C ../web-request $(DEBUG) $(TEST)
	
$(BUILD)/query:
	make -C ../query $(DEBUG) $(TEST)
	
install:	$(OUTPUT)
install:	start

time:	CFLAGS += -Ofast
time:	install

test:	TEST=test
test:	$(OUTPUT)
test:	stop
	@echo "Testing on port $(PORT)"
	$(OUTPUT) -port $(PORT) -test
	$(OUTPUT) -port $(PORT) &

trace:	$(OUTPUT)
trace:	stop
	rm -f trace.txt
	- valgrind --log-file="trace.txt" --track-origins=yes -s --leak-check=yes $(OUTPUT) -port $(PORT) -test
	$(OUTPUT) -port $(PORT) &
	cat trace.txt

start:	CFLAGS += -Ofast
start:	$(OUTPUT)
start:	stop
	$(OUTPUT) -port $(PORT) &

stop:
	@echo Stopping $(PORT)...
	@if [ "$$(sudo lsof -t -i :$(PORT))" != "" ]; then \
		sudo kill -9 $$(sudo lsof -t -i :$(PORT)); \
	fi
	@echo Stopped


debug:	DEBUG=debug
debug:	OPTIMISATION=
debug:	CFLAGS += -g -D DEBUG
debug:	$(OUTPUT)



run_debug:	debug
run_debug:	stop
	gdb --args $(OUTPUT) -port $(PORT) -unlock

clean:
	rm -f session.log
	rm -f *.o
	rm -f test.data
	rm -f trace.txt
	rm -f cookies
	rm -f $(OUTPUT)

