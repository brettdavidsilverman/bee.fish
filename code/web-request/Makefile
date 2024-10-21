CC = g++
CFLAGS = -std=c++17 -fmax-errors=1
BUILD = ../../build
OUTPUT=$(BUILD)/web-request
DEPS=main.cpp content-length.h body.h new-line.h headers.h web-request.h version.h test.h Makefile

$(OUTPUT): $(DEPS)
	@echo "Linking Web Request"
	$(CC) $(CFLAGS) main.cpp -o $(OUTPUT) -lstdc++fs

test:	$(OUTPUT)
	$(OUTPUT) -test
	
debug:	DEBUG = debug
debug:	CFLAGS += -g -D DEBUG
debug:	$(OUTPUT)

clean:
	rm -f $(OUTPUT)
	rm -f trace.txt
