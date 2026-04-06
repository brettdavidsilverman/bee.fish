BUILD  = ../../build
OUTPUT = $(BUILD)/b-string
CFLAGS = -std=c++17 -Wall -fmax-errors=1
DEPS   = char.h b-string.h bit-stream.h base64.h misc.h b-string-base.h stream.h test.h locale.hpp main.cpp Makefile

all:	$(OUTPUT)

$(OUTPUT): $(DEPS)
	g++ $(CFLAGS) main.cpp -o $(OUTPUT) -lssl -lcrypto -lboost_locale -lboost_regex -licuuc
	
	
trace:	$(OUTPUT)
	-valgrind -s --log-file="trace.txt" --track-origins=yes --leak-check=full --show-leak-kinds=all ../../build/b-string -test
	cat trace.txt
	
test:	TEST = test
test:	all
	$(OUTPUT) -test

debug:	DEBUG = debug
debug:	CFLAGS += -g -D DEBUG
debug:	all

run_debug:	debug
	gdb --args $(OUTPUT) -test
	
clean:
	rm -f $(OUTPUT)
