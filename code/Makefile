CC = g++
CFLAGS = -std=c++17 -fmax-errors=1
DEPS=main.cpp DBServer.hpp Makefile
OUTPUT = ../build/DBServer

all:
	cd Parser && make $(DEBUG)
	cd JSON && make $(DEBUG)
	cd PowerEncoding && make $(DEBUG)
	cd Database && make $(DEBUG)
	cd WebRequest && make $(DEBUG)
	cd WebServer && make $(DEBUG)
	objcopy --input binary \
            --output elf64-x86-64 \
            --binary-architecture i386:x86-64 404.html 404.o
	g++ $(CFLAGS) 404.o main.cpp -o $(OUTPUT) -lboost_system -lboost_thread -lpthread -latomic -lstdc++fs

test:
	cd .. && make test
	$(OUTPUT) -test

debug:	DEBUG = debug
debug:	CFLAGS += -g -D DEBUG
debug:	all

clean:
	rm -f 404.o
	cd .. && make clean