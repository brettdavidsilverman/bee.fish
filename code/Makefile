CC = g++
CFLAGS = -std=c++17 -fmax-errors=1
DEPS=main.cpp DBServer.hpp Makefile HomePage.html 404.html
BUILD = ../build
OUTPUT = $(BUILD)/DBServer

all: $(OUTPUT)
	cd Parser && make $(DEBUG)
	cd JSON && make $(DEBUG)
	cd PowerEncoding && make $(DEBUG)
	cd Database && make $(DEBUG)
	cd WebRequest && make $(DEBUG)
	cd WebServer && make $(DEBUG)
	
$(OUTPUT):	$(DEPS) $(BUILD)/404.o $(BUILD)/HomePage.o
	g++ $(CFLAGS) $(BUILD)/404.o $(BUILD)/HomePage.o main.cpp -o $(OUTPUT) -lboost_system -lboost_thread -lpthread -latomic -lstdc++fs


$(BUILD)/404.o: 404.html
	objcopy --input binary \
            --output elf64-x86-64 \
            --binary-architecture i386:x86-64 404.html $(BUILD)/404.o

$(BUILD)/HomePage.o: HomePage.html
	objcopy --input binary \
            --output elf64-x86-64 \
            --binary-architecture i386:x86-64 HomePage.html $(BUILD)/HomePage.o

test:	all
	$(BUILD)/Parser -test
	$(BUILD)/JSON -test
	$(BUILD)/PowerEncoding -test
	$(BUILD)/Database -test
	$(BUILD)/WebRequest -test
	$(BUILD)/WebServer -test
	$(BUILD)/DBServer -test

debug:	DEBUG = debug
debug:	CFLAGS += -g -D DEBUG
debug:	all

clean:
	rm -f 404.o
	cd .. && make clean