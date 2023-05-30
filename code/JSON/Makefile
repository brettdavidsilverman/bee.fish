OUTPUT = ../../build/JSON
DEPS = Makefile main.cpp Number.hpp String.hpp Test.hpp
CFLAGS = -std=c++11 -Wfatal-errors
TEST = tests/time-zones.json

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
time:	run_time

perf:	clean
perf:	CFLAGS += -DTIME -Ofast
perf:	run_performance

test:	$(OUTPUT)
	$(OUTPUT) -test

run:	$(OUTPUT)
	$(OUTPUT)

run_time:	$(OUTPUT)
	time cat $(TEST) | $(OUTPUT)

run_performance:	$(OUTPUT)
	sudo perf record ./perform.sh
	sudo perf report

clean:
	rm -f $(OUTPUT)
	rm -f tmp
	rm -f callgrind.out.*
	rm -f trace.txt
	rm -f test.data
