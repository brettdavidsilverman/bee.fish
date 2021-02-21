CC = g++
INCLUDE = -I./
CFLAGS = -std=c++2a $(INCLUDE) -Wall -fmax-errors=1

OBJECTS=main.o
DEPS=match.h and.h or.h not.h character.h utf-8.h bstring.h range.h word.h ciword.h repeat.h optional.h load-on-demand.h parser.h version.h test.h ../id/id.h Makefile

%.o: %.cpp $(DEPS)
	$(CC) $< -c $(CFLAGS)


parser: $(OBJECTS) $(DEPS) Makefile
	@echo "Linking parser"
	@$(CC) $(CFLAGS) -O -o parser -lcrypto -lssl $(OBJECTS)
	
debug:	clean
debug:	CFLAGS += -g -D DEBUG
debug: DEBUG = debug
debug: parser

clean:
	rm -f parser
	rm -f *.o
	rm -f trace.txt

