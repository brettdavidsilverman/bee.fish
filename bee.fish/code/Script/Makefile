OUTPUT=../../build/parser
CFLAGS=-std=c++17 -Wall -fmax-errors=1 -Ofast
DEPS=main.cpp version.h Variable.hpp ../../build/json Makefile

$(OUTPUT): $(DEPS)
	g++ $(CFLAGS) main.cpp -o $(OUTPUT)
	
test:	$(OUTPUT)
	$(OUTPUT) -test
	
../../build/json:
	cd ../json && make $(DEBUG)
	
debug:	clean
debug:	CFLAGS += -g -D DEBUG
debug:	DEBUG = debug
debug:	parser

clean:
	rm -f $(OUTPUT)
	rm -f *.o
	rm -f trace.txt
	rm -f main
	rm -f test.data