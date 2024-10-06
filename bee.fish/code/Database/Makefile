BUILD = ../../build
CC = g++
CFLAGS = -std=c++17 -fmax-errors=1
DEPS= $(BUILD)/json main.cpp Database.hpp DatabaseBase.hpp Data.hpp Version.hpp Test.hpp File.hpp LockFile.hpp Version.hpp Path.hpp Stack.hpp Iterator.hpp Makefile JSONPath.hpp
OUTPUT = $(BUILD)/Database
LARGE = ../../../large.json

$(OUTPUT):	$(DEPS)
	$(CC) $(CFLAGS) main.cpp -o $(OUTPUT) -lpthread -lstdc++fs -lrt

all:	$(OUTPUT)

$(BUILD)/json:
	cd ../json && make $(DEBUG)

time:	CFLAGS += -DTIME -Ofast
time:	$(OUTPUT)

run_time:	$(OUTPUT)
	time $(OUTPUT) -large
	
trace:	$(OUTPUT)
	strace -c $(OUTPUT) -large
	
debug:	DEBUG = debug
debug:	CFLAGS += -g -rdynamic -DTIME
debug:	$(OUTPUT)

run_debug:	debug
	gdb --args $(OUTPUT) -test -large

run:	$(OUTPUT)
	$(OUTPUT) -input -output


large:	CFLAGS += -DTIME -g
large:	$(OUTPUT)
	$(OUTPUT) -large
	
process:	CFLAGS += -DTIME -Ofast
process:	$(OUTPUT)
process:
	make run_time &
	make run_time &
	
clean_output:
	rm -f $(OUTPUT)
	
clean:
	rm -f $(OUTPUT)
	rm -f *.lock
	rm -f trace.txt
	rm -f test.data
	rm -f lock

test:	$(OUTPUT)
	$(OUTPUT) -test
