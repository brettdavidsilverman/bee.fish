CC = g++
CFLAGS = -std=gnu++17 -Wall -fmax-errors=1 -Ofast
OBJECTS=main.o
DEPS=main.cpp match.h and.h or.h not.h character.h blanks.h word.h ciword.h repeat.h optional.h load-on-demand.h test.h capture.h invoke.h parser.h version.h Makefile

%.o: %.cpp $(DEPS)
	$(CC) $< -c $(CFLAGS)


parser: $(OBJECTS) $(DEPS) Makefile
	@echo "Linking parser"
	@$(CC) $(CFLAGS) -O -o parser $(OBJECTS)
#	./parser -test
	
debug:	clean
debug:	CFLAGS += -g -D DEBUG
debug:	DEBUG = debug
debug:	parser

clean:
	rm -f parser
	rm -f *.o
	rm -f trace.txt
	rm -f main

