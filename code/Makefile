CC = g++
CFLAGS = -std=c++17 -fmax-errors=1
DEPS=main.cpp DBServer.hpp Test.hpp StreamToDB.hpp StreamFromDB.hpp Makefile HomePage.html 404.html
BUILD = ../build
OUTPUT = $(BUILD)/DBServer
TEMP = /var/tmp

all: $(OUTPUT)
	cd Parser && make $(DEBUG)
	cd JSON && make $(DEBUG)
	cd PowerEncoding && make $(DEBUG)
	cd Database && make $(DEBUG)
	cd WebRequest && make $(DEBUG)
	cd WebServer && make $(DEBUG)
	
$(OUTPUT):	$(DEPS) $(TEMP)/404.o $(TEMP)/HomePage.o
	g++ $(CFLAGS) $(TEMP)/404.o $(TEMP)/HomePage.o main.cpp -o $(OUTPUT) -lboost_system -lboost_thread -lpthread -latomic -lstdc++fs


$(TEMP)/404.o: 404.html
	objcopy --input binary \
            --output elf64-x86-64 \
            --binary-architecture i386:x86-64 404.html $(TEMP)/404.o

$(TEMP)/HomePage.o: HomePage.html
	objcopy --input binary \
            --output elf64-x86-64 \
            --binary-architecture i386:x86-64 HomePage.html $(TEMP)/HomePage.o

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
	rm -f $(TEMP)/404.o
	rm -f $(TEMP)/HomePage.o
	cd .. && make clean