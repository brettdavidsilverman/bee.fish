BUILD=../../build
OUTPUT=$(BUILD)/Database
CFLAGS=-std=c++17 -Wall -fmax-errors=1
DEPS=$(BUILD)/json main.cpp Database.hpp DatabaseBase.hpp LeastRecentlyUsedCache.hpp Data.hpp Version.hpp Test.hpp File.hpp LockFile.hpp Version.hpp Path.hpp PathBase.hpp Stack.hpp PathBase.hpp Iterable.hpp Makefile JSONPath.hpp JSONPathParser.hpp JSONDatabase.hpp JSONIndex.hpp
DEATHS=$(BUILD)/../deaths.json

$(OUTPUT): $(DEPS)
	g++ $(CFLAGS) main.cpp -o $(OUTPUT)
	

$(BUILD)/json:
		cd ../json && make $(DEBUG) $(TEST)
		
test:	TEST=test
test:	$(OUTPUT)
	$(OUTPUT) -test

debug:	CFLAGS += -g -D DEBUG
debug:	DEBUG = debug
debug:	$(OUTPUT)

time:	CFLAGS += -D TIME -D DEBUG -Ofast
time:	$(OUTPUT)
	time --verbose $(OUTPUT) -deaths
	
run:	$(OUTPUT)
	$(OUTPUT) -input
	
run_debug:	debug
	gdb --args $(OUTPUT) -test
	
	
trace:	$(OUTPUT)
	rm -f trace.txt
	valgrind --tool=memcheck -s --leak-check=full --track-origins=yes --log-file="trace.txt"  $(OUTPUT) -test
	cat trace.txt

clean:
	rm -f $(OUTPUT)
	rm -f *.o
	rm -f trace.txt
