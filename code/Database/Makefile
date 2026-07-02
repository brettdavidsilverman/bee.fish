BUILD=../../build
OUTPUT=$(BUILD)/Database
CFLAGS=-std=c++17 -Wall -fmax-errors=1
OPTIMISATION=-Ofast
DEPS=$(BUILD)/json main.cpp Database.hpp DatabaseBase.hpp LeastRecentlyUsedCache.hpp Version.hpp Test.hpp File.hpp LockFile.hpp Locks.hpp Version.hpp Branch.hpp Path.hpp PathBase.hpp PathBase.hpp Iterable.hpp Makefile JSONPath.hpp JSONPathParser.hpp JSONDatabase.hpp JSONIndex.hpp NullStream.hpp LoadFiles.hpp ../Size.hpp Makefile

$(OUTPUT): $(DEPS)
	g++ $(CFLAGS) $(OPTIMISATION) main.cpp $(JSON_INDEX) -o $(OUTPUT) -lcryptopp


$(BUILD)/json:
		cd ../json && make $(DEBUG) $(TEST)
		

test:	$(OUTPUT)
	@echo Testing Database
	$(OUTPUT) -test
	

debug:	CFLAGS += -g -D DEBUG
debug:	DEBUG = debug
debug:	OPTIMISATION=
debug:	JSON_INDEX =

debug:	$(OUTPUT)

unlock:	$(OUTPUT)
	$(OUTPUT) -unlock

clean:
	rm -f $(OUTPUT)
	rm -f *.o
	rm -f trace.txt

deaths:	time
	$(OUTPUT) -deaths
	
time:	CFLAGS += -D TIME -Ofast
time:	$(OUTPUT)

run:	$(OUTPUT)
	$(OUTPUT) -input
	
run_debug:	debug
	gdb --args $(OUTPUT) -test
	
	
trace:	$(OUTPUT)
	rm -f trace.txt
	-valgrind --tool=memcheck -s --leak-check=full --track-origins=yes --log-file="trace.txt"  $(OUTPUT) -test
	cat trace.txt

