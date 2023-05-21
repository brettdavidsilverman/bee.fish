OUTPUT = ../../build/JSON
DEPS = Makefile main.cpp
$(OUTPUT): $(DEPS)
	g++ $(CFLAGS) main.cpp -o $(OUTPUT) -lboost_system -lboost_thread -lpthread -latomic

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

clean:
	rm -f *.o
	rm -f a.out
	rm -f json
	rm -f tmp
	rm -f callgrind.out.*
	rm -f trace.txt
	rm -f test.data
	rm -f main
