OUTPUT = ../../build/WebServer
DEPS = Makefile ../../Config.hpp ../Config.hpp Config.hpp ../Miscellaneous/Miscellaneous.hpp WebServer.hpp main.cpp
CFLAGS = -std=c++17 -Wfatal-errors
DEPS = main.cpp WebServer.hpp Version.hpp Test.hpp Config.hpp

$(OUTPUT):	$(DEPS)
	g++ $(CFLAGS) main.cpp -o $(OUTPUT) -lpthread -latomic
	
all:	$(OUTPUT)

test:	$(OUTPUT)
	$(OUTPUT) -test

run:	$(OUTPUT)
	$(OUTPUT)

debug:	DEBUG=debug
debug:	CFLAGS+=-g -DDEBUG
debug:	$(OUTPUT)

clean:
	rm -f $(OUTPUT)