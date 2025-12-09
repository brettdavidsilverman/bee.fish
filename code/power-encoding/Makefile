BUILD=../../build
OUTPUT=$(BUILD)/power-encoding
CFLAGS=-std=c++17 -Wall -fmax-errors=1 -Ofast
DEPS=$(BUILD)/b-string power-encoding.h version.h encoding.h test.h Makefile main.cpp

$(OUTPUT): $(DEPS)
	g++ -o $(OUTPUT) $(CFLAGS) main.cpp
	
$(BUILD)/b-string:
	cd  ../b-string && make $(DEBUG)
	
test:	TEST=test
test:	$(OUTPUT)
	$(OUTPUT) -test
	
debug:	DEBUG = debug
debug:	CFLAGS += -Og -g -D DEBUG
debug:	$(OUTPUT)

clean:
	rm -f $(OUTPUT)
	rm -f callgrind.out.*
	rm -f trace.txt