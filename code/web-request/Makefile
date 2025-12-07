CC = g++
CFLAGS = -std=c++17 -fmax-errors=1
BUILD = ../../build
OUTPUT=$(BUILD)/web-request
DEPS=$(BUILD) main.cpp content-length.h body.h new-line.h headers.h web-request.h version.h test.h Makefile

$(OUTPUT): $(DEPS)
	@echo "Linking Web Request"
	$(CC) $(CFLAGS) main.cpp -o $(OUTPUT) -lstdc++fs

$(BUILD):
	mkdir $(BUILD)
	
test:	$(OUTPUT)
	$(OUTPUT) -test
	
debug:	CFLAGS += -g -D DEBUG
debug:	DEBUG = debug
debug:	$(OUTPUT)


run:	$(OUTPUT)
	$(OUTPUT)
	
run_debug:	debug
	gdb --args $(OUTPUT) -test
	
trace:	$(OUTPUT)
	rm -f trace.txt
	- valgrind -s --tool=memcheck --leak-check=full --track-origins=yes --log-file="trace.txt"  $(OUTPUT) -test
	cat trace.txt
	

clean:
	rm -f $(OUTPUT)
	rm -f trace.txt
