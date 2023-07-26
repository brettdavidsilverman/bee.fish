CC = g++
CFLAGS = -std=c++17 -fmax-errors=1
DEPS=main.cpp PowerEncoding.hpp Version.hpp Test.hpp Makefile
OUTPUT = ../../build/PowerEncoding

$(OUTPUT):	$(DEPS)
	g++ $(CFLAGS) main.cpp -o $(OUTPUT) -lpthread -lstdc++fs

debug:	clean
debug:	DEBUG = debug
debug:	CFLAGS += -g -D DEBUG
debug:	$(OUTPUT)

test:	debug
	$(OUTPUT) -test

run:	debug
	$(OUTPUT)

clean:
	rm -f $(OBJECTS)
	rm -f $(OUTPUT)
	rm -f trace.txt
