CC = g++
CFLAGS = -std=c++17 -fmax-errors=1
DEPS=main.cpp DBServer.hpp Test.hpp StreamToDB.hpp StreamFromDB.hpp Makefile HomePage.html 404.html
BUILD = ../build
TEMP = /var/tmp

all:
	cd Parser && make $(DEBUG)
	cd JSON && make $(DEBUG)
	cd PowerEncoding && make $(DEBUG)
	cd Database && make $(DEBUG)
	cd WebRequest && make $(DEBUG)
	cd WebServer && make $(DEBUG)
	cd DBServer && make $(DEBUG)
	
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
	cd .. && make clean