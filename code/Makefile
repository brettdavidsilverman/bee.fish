CC = g++
CFLAGS = -std=c++17 -fmax-errors=1
DEPS=main.cpp DBServer.hpp Makefile HomePage.html 404.html
OUTPUT = ../build/DBServer

all: $(OUTPUT)
	cd Parser && make $(DEBUG)
	cd JSON && make $(DEBUG)
	cd PowerEncoding && make $(DEBUG)
	cd Database && make $(DEBUG)
	cd WebRequest && make $(DEBUG)
	cd WebServer && make $(DEBUG)
	
$(OUTPUT):	$(DEPS) ../build/404.o ../build/HomePage.o
	g++ $(CFLAGS) ../build/404.o ../build/HomePage.o main.cpp -o $(OUTPUT) -lboost_system -lboost_thread -lpthread -latomic -lstdc++fs


../build/404.o: 404.html
	objcopy --input binary \
            --output elf64-x86-64 \
            --binary-architecture i386:x86-64 404.html ../build/404.o

../build/HomePage.o: HomePage.html
	objcopy --input binary \
            --output elf64-x86-64 \
            --binary-architecture i386:x86-64 HomePage.html ../build/HomePage.o

test:	all
	../build/Parser -test
	../build/JSON -test
	../build/PowerEncoding -test
	../build/Database -test
	../build/WebRequest -test
	../build/WebServer -test
	../build/DBServer -test

debug:	DEBUG = debug
debug:	CFLAGS += -g -D DEBUG
debug:	all

clean:
	rm -f 404.o
	cd .. && make clean