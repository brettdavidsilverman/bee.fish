BUILD = ../../build
CC = g++
CFLAGS = -std=c++17 -fmax-errors=1
DEPS= $(BUILD)/json main.cpp Database.hpp DatabaseBase.hpp Data.hpp Version.hpp Test.hpp File.hpp Version.hpp Path.hpp Stack.hpp Iterator.hpp Makefile JSON2Path.hpp Path2JSON.hpp
OUTPUT = $(BUILD)/Database
LARGE = ../../../large.json

$(OUTPUT):	$(DEPS)
	$(CC) $(CFLAGS) main.cpp -o $(OUTPUT) -lpthread -lstdc++fs

all:	$(OUTPUT)

$(BUILD)/json:
	cd ../json && make $(DEBUG)

time:	CFLAGS += -D TIME -Ofast -DDEBUG
time:	$(OUTPUT)
time:	run_time

run_time:
	time cat $(LARGE) | $(OUTPUT) -input
	
debug:	clean
debug:	DEBUG = debug
debug:	CFLAGS += -g -rdynamic -DDEBUG
debug:	$(OUTPUT)

run_debug:
	gdb --args $(OUTPUT) -test

run:	$(OUTPUT)
	$(OUTPUT) -input -output

process:	$(OUTPUT)
	make run_time &
	make run_time &
	
clean:
	rm -f $(OUTPUT)
	rm -f trace.txt
	rm -f test.data

test:	$(OUTPUT)
	$(OUTPUT) -test
