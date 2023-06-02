CC = g++
CFLAGS = -std=c++17 -fmax-errors=1
DEPS=main.cpp DBServer.hpp Makefile
OUTPUT = ../build/DBServer

$(OUTPUT):	$(DEPS)
	g++ $(CFLAGS) main.cpp -o $(OUTPUT) -lboost_system -lboost_thread -lpthread -latomic -lstdc++fs


all:	$(OUTPUT)
	cd Parser && make $(DEBUG)
	cd JSON && make $(DEBUG)
	cd PowerEncoding && make $(DEBUG)
	cd Database && make $(DEBUG)
	cd WebRequest && make $(DEBUG)
	cd WebServer && make $(DEBUG)


debug:	DEBUG = debug
debug:	CFLAGS += -g -D DEBUG
debug:	all

clean:
	cd .. && make clean