OUTPUT=../../build/query
CFLAGS=-std=c++17 -Wall -fmax-errors=1
DEPS=main.cpp query.h test.h version.h Makefile

$(OUTPUT): $(DEPS)
	g++ $(CFLAGS) main.cpp -o $(OUTPUT)
	
test:	$(OUTPUT)
	$(OUTPUT) -test

debug:	CFLAGS += -g -D DEBUG
debug:	DEBUG = debug
debug:	$(OUTPUT)
debug:	test

run:	$(OUTPUT)
	$(OUTPUT)
	
run_debug:	debug
run_debug:	$(OUTPUT)
	gdb --args $(OUTPUT) -test
	
trace:	$(OUTPUT)
	rm -f trace.txt
	valgrind  --log-file="trace.txt" --track-origins=yes --leak-check=yes $(OUTPUT) -test
	cat trace.txt
	
clean:
	rm -f $(OUTPUT)
	rm -f *.o
	rm -f trace.txt
	rm -f main

