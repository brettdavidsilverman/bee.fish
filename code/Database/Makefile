BUILD=../../build
CC=g++
DEPS=$(BUILD)/json main.cpp Database.hpp DatabaseBase.hpp Data.hpp Version.hpp Test.hpp File.hpp LockFile.hpp Version.hpp Path.hpp Stack.hpp MinMaxPath.hpp Iterable.hpp Makefile AndPath.hpp OrPath.hpp JSONPath.hpp JSONPathParser.hpp JSONDatabase.hpp
OUTPUT=$(BUILD)/Database
LARGE=$(BUILD)/../large.json

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


run:	$(OUTPUT)
	$(OUTPUT) -input -output
	
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
