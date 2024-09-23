OUTPUT=../../build/script
CFLAGS=-std=c++17 -Wall -fmax-errors=1 -Ofast
DEPS=main.cpp Version.hpp Variable.hpp Type.hpp Script.hpp ScriptParser.hpp Test.hpp ../../build/json Makefile

$(OUTPUT): $(DEPS)
	g++ $(CFLAGS) main.cpp -o $(OUTPUT)
	
test:	$(OUTPUT)
	$(OUTPUT) -test
	
../../build/json:
	cd ../json && make $(DEBUG)
	
debug:	CFLAGS += -g -D DEBUG
debug:	DEBUG = debug
debug:	$(OUTPUT)

run_debug:	debug
	gdb --args $(OUTPUT) -test

clean:
	rm -f $(OUTPUT)
	rm -f *.o
	rm -f trace.txt
	rm -f main
	rm -f test.data