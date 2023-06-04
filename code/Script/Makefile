CC=g++
CFLAGS=-std=c++17 -Wall -fmax-errors=1 -Ofast
OBJECTS=main.o
DEPS=main.cpp object.h version.h b-script.h b-script-parser.h test.h ../json/json ../parser/parser Makefile

%.o: %.cpp $(DEPS)
	$(CC) $< -c $(CFLAGS)

b-script: $(OBJECTS) $(DEPS) Makefile
	@echo "Linking b-script"
	@$(CC) $(CFLAGS) -O -o b-script $(OBJECTS) -lstdc++fs
#	./b-script -test

../parser/parser:
	cd ../parser && make $(DEBUG)

../json/json:
	cd ../json && make $(DEBUG)

time:	clean
time:	CFLAGS += -D TIME -Ofast
time:	b-script

debug:	clean
debug:	DEBUG = debug
debug:	CFLAGS += -Og -g -D DEBUG
debug:	b-script

clean:
	rm -f *.o
	rm -f a.out
	rm -f b-script
	rm -f tmp
	rm -f callgrind.out.*
	rm -f trace.txt
	rm -f test.data
	rm -f json-in-stream.db
