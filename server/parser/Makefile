CC = g++
INCLUDE = -I./
CFLAGS = -std=c++2a $(INCLUDE) -Wall -fmax-errors=1

OBJECTS=main.o
DEPS=match.h and.h or.h not.h character.h word.h ciword.h repeat.h optional.h load-on-demand.h parser.h version.h utf-8.h b-string.h test.h ../id/id.h ../https/request.h ../https/basic-authorization.h ../https/base64.h Makefile

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

