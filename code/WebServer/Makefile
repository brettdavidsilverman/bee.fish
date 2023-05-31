OUTPUT = ../../build/WebServer
DEPS = Makefile ../../Config.hpp ../Config.hpp Config.hpp ../Miscellaneous/Miscellaneous.hpp WebServer.hpp main.cpp

$(OUTPUT):	$(DEPS)
	g++ $(CFLAGS) main.cpp -o $(OUTPUT) -lboost_system -lboost_thread -lpthread -latomic

all:	$(OUTPUT)

test:	$(OUTPUT)
	$(OUTPUT) -test

run:	$(OUTPUT)
	$(OUTPUT)

debug:	DEBUG=debug
debug:	CFLAGS+=-g -DDEBUG
debug:	$(OUTPUT)