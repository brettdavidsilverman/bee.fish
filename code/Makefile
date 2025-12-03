BUILD = ../build
CFLAGS = -std=c++17 -fmax-errors=1
DEPS=Makefile
TEMP = /var/tmp

all:	$(BUILD)
	cd b-string && make $(DEBUG) test
	cd power-encoding && make $(DEBUG) test
	cd Id && make $(DEBUG) test
	cd parser && make $(DEBUG) test
	cd json && make $(DEBUG) test
	cd Database && make $(DEBUG) test
	cd Script && make $(DEBUG) test
	cd web-request && make $(DEBUG) test
	cd query && make $(DEBUG) test
	cd https && make $(DEBUG) test
	

$(BUILD):
		mkdir $(BUILD)

test:	all

debug:	DEBUG = debug
debug:	CFLAGS += -g -D DEBUG
debug:	all

clean:
	cd b-string && make clean
	cd power-encoding && make clean
	cd Id && make clean
	cd parser && make clean
	cd json && make clean
	cd Database && make clean
	cd Script && make clean
	cd web-request && make clean
	cd query && make clean
	cd https && make clean
	rm -f -r $(BUILD)
