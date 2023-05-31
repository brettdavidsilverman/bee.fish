CC = g++
CFLAGS = -std=c++2a -fmax-errors=1
OBJECTS=main.o
DEPS=main.cpp WebRequest.hpp Version.hpp Test.hpp Makefile
OUTPUT = ../../build/WebRequest

%.o: %.cpp $(DEPS)
	$(CC) $< -c $(CFLAGS)
	
$(OUTPUT): $(OBJECTS) $(DEPS)
	@echo "Linking Web Request"
	$(CC) $(CFLAGS) -O -o $(OUTPUT) $(OBJECTS) -lstdc++fs
	
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
