BUILD=../../build
OUTPUT=$(BUILD)/query
CFLAGS=-std=c++17 -Wall -fmax-errors=1
DEPS=main.cpp query.h test.h version.h Makefile

$(OUTPUT): $(DEPS) $(BUILD)
	g++ $(CFLAGS) main.cpp -o $(OUTPUT)


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
	valgrind  -s --log-file="trace.txt" --track-origins=yes --leak-check=yes $(OUTPUT)
	cat trace.txt
	
clean:
	rm -f $(OUTPUT)
	rm -f *.o
	rm -f trace.txt
	rm -f main

