BUILD=../../build
OUTPUT=$(BUILD)/json
CC=g++
CFLAGS=-std=c++17 -Wall -fmax-errors=1 -Ofast
DEPS=main.cpp json.h json-string.h number.h array.h object.h boolean.h null.h blank-space.h set.h keyed-set.h test.h version.h $(BUILD)/parser json-parser.h Makefile

$(OUTPUT): $(DEPS)
	g++ $(CFLAGS) main.cpp -o $(OUTPUT)
	
test:	$(OUTPUT)
	$(BUILD)/parser -test
	$(OUTPUT) -test
	
$(BUILD)/parser:
	cd ../parser && make $(DEBUG) test

debug:	DEBUG = debug
debug:	CFLAGS += -Og -g -D DEBUG
debug:	$(OUTPUT) $(BUILD)/parser

run_debug:	debug
	gdb --args $(OUTPUT) -test

time:	clean
time:	CFLAGS += -D TIME -Ofast
time:	$(OUTPUT)
time:	run_time

run_time:
	time cat ~/bee.fish/large.json | $(OUTPUT)

trace:	$(OUTPUT)
	rm -f trace.txt
	valgrind  --log-file="trace.txt" --track-origins=yes --leak-check=yes $(OUTPUT) -test
	cat trace.txt

clean:
	rm -f $(OUTPUT)
	rm -f *.o
	rm -f a.out
	rm -f json
	rm -f tmp
	rm -f callgrind.out.*
	rm -f trace.txt
	rm -f test.data
	rm -f main
