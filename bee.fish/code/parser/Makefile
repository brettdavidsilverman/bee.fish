OUTPUT=../../build/parser
CFLAGS=-std=gnu++17 -Wall -fmax-errors=1 -Ofast
DEPS=main.cpp match.h and.h or.h not.h character.h blanks.h word.h ciword.h repeat.h optional.h load-on-demand.h test.h capture.h invoke.h parser.h version.h Makefile

$(OUTPUT): $(DEPS)
	g++ $(CFLAGS) main.cpp -o $(OUTPUT)
	
test:	$(OUTPUT)
	$(OUTPUT) -test
	
debug:	clean
debug:	CFLAGS += -g -D DEBUG
debug:	DEBUG = debug
debug:	parser

clean:
	rm -f $(OUTPUT)
	rm -f *.o
	rm -f trace.txt
	rm -f main

