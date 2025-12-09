BUILD  = ../../build
OUTPUT = $(BUILD)/Id
CFLAGS = -std=c++17 -Wall -fmax-errors=1 -Ofast
DEPS= $(BUILD)/power-encoding Id.hpp Test.hpp main.cpp Makefile

$(OUTPUT): $(DEPS)
	g++ $(CFLAGS) main.cpp -o $(OUTPUT)
	
$(BUILD)/power-encoding:
	cd ../power-encoding && make $(DEBUG)
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