BUILD=../../build
OUTPUT=$(BUILD)/query
CFLAGS=-std=c++17 -Wall -fmax-errors=1
DEPS=main.cpp query.h test.h version.h Makefile

all:
all:	$(BUILD)
all:	$(OUTPUT)


$(BUILD):
		mkdir $(BUILD)
		
$(OUTPUT): $(DEPS)
	g++ $(CFLAGS) main.cpp -o $(OUTPUT)
	
test:	all
	$(OUTPUT) -test

debug:	CFLAGS += -g -D DEBUG
debug:	DEBUG = debug
debug:	all


run:	all
	$(OUTPUT)
	
run_debug:	debug
	gdb --args $(OUTPUT) -test
	
trace:	all
	rm -f trace.txt
	valgrind  --log-file="trace.txt" --track-origins=yes --leak-check=yes $(OUTPUT) -test
	cat trace.txt
	
clean:
	rm -f $(OUTPUT)
	rm -f *.o
	rm -f trace.txt
	rm -f main

