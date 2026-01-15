BUILD=../../build
OUTPUT=$(BUILD)/parser
CFLAGS=-std=c++17 -Wall -fmax-errors=1
DEPS=main.cpp match.h and.h or.h not.h character.h blankspaces.h word.h ciword.h repeat.h optional.h load-on-demand.h test.h capture.h invoke.h order-of-precedence.h parser.h version.h Makefile $(BUILD)/b-string

$(OUTPUT): $(DEPS)
	g++ $(CFLAGS) main.cpp -o $(OUTPUT)
	
test:	TEST=test
test:	$(OUTPUT)
	$(OUTPUT) -test

debug:	CFLAGS += -g -D DEBUG
debug:	DEBUG = debug
debug:	$(OUTPUT)

$(BUILD)/b-string:
	cd ../b-string && make $(DEBUG) $(TEST)
	
run:	$(OUTPUT)
	$(OUTPUT)
	
run_debug:	debug
	gdb --args $(OUTPUT) -test
	
trace:	$(OUTPUT)
	rm -f trace.txt
	- valgrind --tool=memcheck -s --leak-check=full --track-origins=yes --log-file="trace.txt"  $(OUTPUT) -test
	cat trace.txt

clean:
	rm -f $(OUTPUT)
	rm -f *.o
	rm -f trace.txt
	rm -f main

