OUTPUT = ../../build/JSON
DEPS = Makefile main.cpp Number.hpp String.hpp Test.hpp
CFLAGS = -std=c++17 -Wfatal-errors
TEST = ../../large.json

$(OUTPUT): $(DEPS)
	g++ $(CFLAGS) main.cpp -o $(OUTPUT)

debug:	clean
debug:	DEBUG = debug
debug:	CFLAGS += -g -DDEBUG
debug:	$(OUTPUT)

../../build/Parser:
	cd ../Parser && make $(DEBUG)

time:	clean
time:	CFLAGS += -DTIME -O3
time:	run_time

perf:	clean
perf:	CFLAGS += -DTIME -O3
perf:	run_performance

trace:	clean
trace:	CFLAGS += -DTIME -g -O3
trace:	run_trace

test:	clean
test:	CFLAGS += -g
test:	run_test

run_test:	$(OUTPUT)
	$(OUTPUT) -test

run:	$(OUTPUT)
	$(OUTPUT)

run_time:	$(OUTPUT)
	time cat $(TEST) | $(OUTPUT) -use home

run_trace:	$(OUTPUT)
	rm -f trace.txt
	valgrind  -s --log-file="trace.txt" --track-origins=yes --leak-check=yes ./cat.sh $(TEST)
	cat trace.txt

run_performance:	$(OUTPUT)
	sudo perf record ./cat.sh $(TEST)
	sudo perf report

clean:
	rm -f $(OUTPUT)
	rm -f tmp
	rm -f callgrind.out.*
	rm -f trace.txt
	rm -f test.data
