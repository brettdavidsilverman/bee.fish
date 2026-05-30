BUILD  = ../../build
OUTPUT = $(BUILD)/Id
CFLAGS = -std=c++17 -Wall -fmax-errors=1
DEPS= $(BUILD)/power-encoding Id.hpp Test.hpp main.cpp Makefile

$(OUTPUT): $(DEPS)
	g++ $(CFLAGS) main.cpp -o $(OUTPUT) -lboost_locale -licuuc -lcryptopp
	
$(BUILD)/power-encoding:
	cd ../power-encoding && make $(DEBUG)
test:	TEST=test
test:	$(OUTPUT)
	$(OUTPUT) -test
	
debug:	DEBUG = debug
debug:	CFLAGS += -Og -g -D DEBUG
debug:	$(OUTPUT)

run_debug:	debug
	gdb --args $(OUTPUT) -test
	
trace:	$(OUTPUT)
	rm -f trace.txt
	- valgrind --log-file="trace.txt" --track-origins=yes --leak-check=yes -s $(OUTPUT) -test
	cat trace.txt
	
clean:
	rm -f $(OUTPUT)
	rm -f callgrind.out.*
	rm -f trace.txt