CC = g++
CFLAGS = -std=c++17 -fmax-errors=1
DEPS=main.cpp Database.hpp DatabaseBase.hpp Data.hpp Version.hpp Test.hpp File.hpp Version.hpp Path.hpp Stack.hpp Makefile
OUTPUT = ../../build/Database

$(OUTPUT):	$(DEPS)
	g++ $(CFLAGS) main.cpp -o $(OUTPUT) -lpthread -lstdc++fs

all:	$(OUTPUT)

debug:	clean
debug:	DEBUG = debug
debug:	CFLAGS += -g -rdynamic -DDEBUG
debug:	$(OUTPUT)

clean:
	rm -f $(OUTPUT)
	rm -f trace.txt

test:	$(OUTPUT)
	$(OUTPUT) -test 2>&1 | c++filt