CC = g++
CFLAGS = -std=c++17 -fmax-errors=1
BUILD = ../../build
OBJECTS=main.o
DEPS=main.cpp content-length.h body.h new-line.h headers.h web-request.h version.h test.h Makefile
%.o: %.cpp $(DEPS)
	$(CC) $< -c $(CFLAGS)
	

web-request: $(OBJECTS) $(DEPS)
	@echo "Linking Web Request"
	$(CC) $(CFLAGS) -O -o $(BUILD)/web-request $(OBJECTS) -lstdc++fs

test:	web-request
	$(BUILD)/web-request -test
	
debug:	DEBUG = debug
debug:	CFLAGS += -g -D DEBUG
debug:	web-request

clean:
	rm -f $(OBJECTS)
	rm -f web-request
	rm -f trace.txt
	rm -f $(BUILD)/web-request
