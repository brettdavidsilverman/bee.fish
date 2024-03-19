CC = g++
CFLAGS = -std=c++17 -fmax-errors=1
OBJECTS=main.o test.o
DEPS=main.cpp test.h test-result.h test-suite.h version.h ../b-string/b-string ../parser/parser ../json/json ../web-request/web-request ../b-script/b-script ../https/https Makefile
%.o: %.cpp $(DEPS)
	$(CC) $< -c $(CFLAGS)
	
test: $(OBJECTS) $(DEPS)
	@echo "Linking test"
	$(CC) -o test $(CFLAGS) -O $(OBJECTS) -lcrypto -lstdc++fs -lpthread

debug:	clean
debug:	DEBUG = debug
debug:	CFLAGS += -g -D DEBUG
debug: test

clean:
	rm -f $(OBJECTS)
	rm -f test