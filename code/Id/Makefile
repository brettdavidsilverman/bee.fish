BUILD  = ../../build
OUTPUT = $(BUILD)/Id
CFLAGS = -std=c++17 -Wall -fmax-errors=1 -Ofast
DEPS= $(BUILD)/b-string Id.hpp Test.hpp main.cpp Makefile

$(OUTPUT): $(DEPS)
	g++ $(CFLAGS) main.cpp -o $(OUTPUT)
	
test:	$(OUTPUT)
	$(OUTPUT) -test

$(BUILD)/b-string:
	cd ../b-string && make $(DEBUG)

debug:	CFLAGS += -g -D DEBUG
debug:	$(OUTPUT)

run:	$(OUTPUT)
	$(OUTPUT)

run_key:	$(OUTPUT)
	$(OUTPUT) -key
	
run_debug:	$(OUTPUT)
	gdb --args $(OUTPUT) -test
	
clean:
	rm -f $(OUTPUT)
	rm -f test.txt
