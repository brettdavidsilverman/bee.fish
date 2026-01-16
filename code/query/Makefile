BUILD=../../build
OUTPUT=$(BUILD)/query
CFLAGS=-std=c++17 -Wall -fmax-errors=1
DEPS=main.cpp query.hpp test.hpp version.h Makefile $(BUILD)/Database

$(OUTPUT): $(DEPS) $(BUILD)
	g++ $(CFLAGS) main.cpp -o $(OUTPUT)


$(BUILD):
		mkdir $(BUILD)
		
$(BUILD)/Database:
	cd ../Database && make $(DEBUG)
	
test:	$(OUTPUT)
	$(OUTPUT) -test

debug:	CFLAGS += -g -D DEBUG
debug:	DEBUG = debug
debug:	$(OUTPUT)


run:	$(OUTPUT)
	$(OUTPUT)
	
run_debug:	debug
	gdb --args $(OUTPUT)
	
trace:	$(OUTPUT)
	rm -f trace.txt
	valgrind  -s --log-file="trace.txt" --track-origins=yes --leak-check=yes $(OUTPUT) -test
	cat trace.txt
	
clean:
	rm -f $(OUTPUT)
	rm -f *.o
	rm -f trace.txt
	rm -f main

