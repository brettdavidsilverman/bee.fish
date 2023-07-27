CC = g++
CFLAGS = -std=gnu++17 -fmax-errors=1
DEPS=main.cpp WebRequest.hpp Version.hpp Test.hpp Makefile
OUTPUT = ../../build/WebRequest

$(OUTPUT):	$(DEPS)
	g++ $(CFLAGS) main.cpp -o $(OUTPUT) -lpthread -lstdc++fs

debug:	clean
debug:	DEBUG = debug
debug:	CFLAGS += -g -D DEBUG
debug:	$(OUTPUT)

test:	debug
	$(OUTPUT) -test

clean:
	rm -f $(OBJECTS)
	rm -f $(OUTPUT)
	rm -f trace.txt
