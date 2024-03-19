OUTPUT = ../../build/power-encoding
CFLAGS = -std=c++17 -Wall -fmax-errors=1 -Ofast
DEPS=power-encoding.h version.h encoding.h test.h Makefile main.cpp

$(OUTPUT): $(DEPS)
	g++ $(CFLAGS) main.cpp -o $(OUTPUT)
	
test:	$(OUTPUT)
	$(OUTPUT) -test
	
debug:	clean
debug:	DEBUG = debug
debug:	CFLAGS += -Og -g -D DEBUG
debug:	$(OUTPUT)

clean:
	rm -f *.o
	rm -f $(OUTPUT)
	rm -f tmp
	rm -f callgrind.out.*
	rm -f trace.txt