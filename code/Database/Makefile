BUILD=../../build
OUTPUT_INDEX=$(BUILD)/DatabaseIndex
OUTPUT=$(BUILD)/Database
CFLAGS=-std=c++17 -Wall -fmax-errors=1
DEPS=$(BUILD)/json main.cpp Database.hpp DatabaseBase.hpp LeastRecentlyUsedCache.hpp Data.hpp Version.hpp Test.hpp File.hpp LockFile.hpp Locks.hpp Version.hpp Branch.hpp Path.hpp PathBase.hpp PathBase.hpp Iterable.hpp Iterator.hpp ReverseIterator.hpp Makefile JSONPath.hpp JSONPathParser.hpp JSONDatabase.hpp JSONIndex.hpp NullStream.hpp

$(OUTPUT): $(DEPS)
	g++ $(CFLAGS) main.cpp -o $(OUTPUT) -lboost_locale -lboost_regex -licuuc -lcryptopp

$(OUTPUT_INDEX): $(DEPS)
	g++ $(CFLAGS) main.cpp -o $(OUTPUT_INDEX) -D JSON_INDEX -lboost_locale -lboost_regex -licuuc -lcryptopp
	

$(BUILD)/json:
		cd ../json && make $(DEBUG) $(TEST)

testIndex:	$(OUTPUT_INDEX)
	@echo Testing Index
	$(OUTPUT_INDEX) -test
	
testId:	$(OUTPUT)
	@echo Testing Id
	$(OUTPUT) -test
	
test:	testIndex
test:	testId

debug:	CFLAGS += -g -D DEBUG
debug:	DEBUG = debug
debug:	$(OUTPUT_INDEX)
debug:	$(OUTPUT)

unlock:	$(OUTPUT)
	$(OUTPUT) -unlock

clean:
	rm -f $(OUTPUT)
	rm -f $(OUTPUT_INDEX)
	rm -f *.o
	rm -f trace.txt

deaths:
deaths:	CFLAGS += -D TIME -D DEBUG -Ofast
deaths:	$(OUTPUT)
	$(OUTPUT) -deaths
	
time:
time:	CFLAGS += -D TIME -D DEBUG -Ofast
time:	$(OUTPUT)

run:	$(OUTPUT)
	$(OUTPUT) -input
	
run_debug:	debug
	gdb --args $(OUTPUT) -test
	
	
trace:	$(OUTPUT)
	rm -f trace.txt
	-valgrind --tool=memcheck -s --leak-check=full --track-origins=yes --log-file="trace.txt"  $(OUTPUT) -test
	cat trace.txt

