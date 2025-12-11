BUILD=../../build
OUTPUT=$(BUILD)/json
DEATHS=$(BUILD)/../deaths.json
CC=g++
OPTIMISER=-Ofast
CFLAGS=-std=c++17 -Wall -fmax-errors=1 $(OPTIMISER)
DEPS=main.cpp json.h string.h number.h array.h object.h boolean.h null.h blank-space.h set.h keyed-set.h test.h version.h json-parser.h Makefile $(BUILD)/parser

$(OUTPUT): $(DEPS)
	g++ $(CFLAGS) main.cpp -o $(OUTPUT)
	
$(BUILD)/parser:
	cd .. && make $(DEBUG) $(TEST)
	
test:	TEST=test
test:	$(OUTPUT)
	$(OUTPUT) -test

debug:	CFLAGS += -g -D DEBUG
debug:	DEBUG = debug
debug:	OPTIMISER =
debug:	$(OUTPUT)

time:	CFLAGS += -D TIME
time:	$(OUTPUT)
	time cat $(DEATHS) | $(OUTPUT)
	
run:	$(OUTPUT)
	$(OUTPUT)
	
run_debug:	debug
	gdb --args $(OUTPUT) -test
	
trace:	$(OUTPUT)
	rm -f trace.txt
	valgrind --tool=memcheck -s --leak-check=full --track-origins=yes --log-file="trace.txt"  $(OUTPUT) -test
	cat trace.txt

clean:
	rm -f $(OUTPUT)
	rm -f *.o
	rm -f trace.txt

