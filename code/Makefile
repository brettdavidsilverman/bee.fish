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
	rm -f -r $(BUILD)
