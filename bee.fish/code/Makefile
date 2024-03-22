BUILD = ../build
CFLAGS = -std=c++17 -fmax-errors=1
DEPS=$(BUILD)/Database Makefile
TEMP = /var/tmp

all:	$(DEPS)
	cd Id && make $(DEBUG) test
	cd power-encoding && make $(DEBUG) test
	cd b-string && make $(DEBUG) test
	cd parser && make $(DEBUG) test
	cd json && make $(DEBUG) test
	cd Database && make $(DEBUG) test
	
	#cd PowerEncoding && make $(DEBUG)
	
	#cd JSON && make $(DEBUG)
	#cd WebRequest && make $(DEBUG)
	#cd WebServer && make $(DEBUG)
#	cd DBServer && make $(DEBUG)
	
	
$(BUILD)/Database:
	cd Database && make $(DEBUG) test
	
test:	all
	../stop.sh 8000
	cd Id && ../$(BUILD)/Id -test
	cd power-encoding && ../$(BUILD)/power-encoding -test
	cd b-string && ../$(BUILD)/b-string -test
	cd parser && ../$(BUILD)/parser -test
	cd json && ../$(BUILD)/json -test
	cd Database && ../$(BUILD)/Database -test

	#cd WebRequest && $(BUILD)/WebRequest -test
	#cd WebServer && $(BUILD)/WebServer -test
#	cd DBServer && $(BUILD)/DBServer -test

debug:	DEBUG = debug
debug:	CFLAGS += -g -D DEBUG
debug:	all

clean:
	cd .. && make clean
