OUTPUT = ../../build/JSON
DEPS = Makefile main.cpp Number.hpp String.hpp Test.hpp
CFLAGS = -Wfatal-errors

$(OUTPUT): $(DEPS)
	g++ $(CFLAGS) main.cpp -o $(OUTPUT) -lboost_system -lboost_thread -lpthread -latomic

debug:	clean
debug:	DEBUG = debug
debug:	CFLAGS += -g -DDEBUG
debug:	$(OUTPUT)

../../build/Parser:
	cd ../Parser && make $(DEBUG)

time:	clean
time:	CFLAGS += -DTIME -Ofast
time:	

test:	$(OUTPUT)
	$(OUTPUT) -test

run:	$(OUTPUT)
	$(OUTPUT)

clean:
	rm -f $(OUTPUT)
	rm -f tmp
	rm -f callgrind.out.*
	rm -f trace.txt
	rm -f test.data
