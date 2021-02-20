CC = g++
CFLAGS = -std=c++2a -Wall -fmax-errors=1 -Ofast
OBJECTS=main.o
BUILD_NUMBER_FILE=../../build-number.txt
DEPS=json.h string.h number.h array.h object.h blank-space.h value.h version.h ../match.h Makefile

json: $(OBJECTS)
	rm -f json
	@echo "Linking json"
	@$(CC) $(CFLAGS) -o json $(OBJECTS)

%.o: %.cpp $(DEPS)
	$(CC) $< -c $(CFLAGS)

debug:	clean
debug:	DEBUG = debug
debug:	CFLAGS += -Og -g -D DEBUG
debug:	json

clean:
	rm -f *.o
	rm -f json
	rm -f tmp
	rm -f callgrind.out.*
	rm -f trace.txt