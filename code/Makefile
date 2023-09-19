CC = g++
CFLAGS = -std=c++17 -fmax-errors=1
DEPS=main.cpp DBServer.hpp Test.hpp StreamToDB.hpp StreamFromDB.hpp Makefile HomePage.html 404.html
BUILD = ../../build
TEMP = /var/tmp

all:
	cd Parser && make $(DEBUG)
	cd PowerEncoding && make $(DEBUG)
	cd Database && make $(DEBUG)
	cd JSON && make $(DEBUG)
	cd WebRequest && make $(DEBUG)
	cd WebServer && make $(DEBUG)
	cd DBServer && make $(DEBUG)
	
test:	all
	cd Parser && $(BUILD)/Parser -test
	cd PowerEncoding && $(BUILD)/PowerEncoding -test
	cd Database && $(BUILD)/Database -test
	cd JSON && $(BUILD)/JSON -test
	cd WebRequest && $(BUILD)/WebRequest -test
	cd WebServer && $(BUILD)/WebServer -test
	cd DBServer && $(BUILD)/DBServer -test

debug:	DEBUG = debug
debug:	CFLAGS += -g -D DEBUG
debug:	all

clean:
	cd .. && make clean