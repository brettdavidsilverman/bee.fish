BUILD = ../build
CFLAGS = -std=c++17 -fmax-errors=1
DEPS=Makefile
TEMP = /var/tmp

all:
all:	$(BUILD)
	make -C b-string $(DEBUG) test
	make -C power-encoding $(DEBUG) test
	make -C Id $(DEBUG) test
	make -C parser $(DEBUG) test
	make -C json $(DEBUG) test
	make -C Database $(DEBUG) test
	make -C Script $(DEBUG) test
	make -C web-request $(DEBUG) test
	make -C query $(DEBUG) test
	make -C https $(DEBUG) test PORT=$(PORT)

$(BUILD):
	mkdir $(BUILD)
	
install:
	@if [ "$(PORT)" = "" ]; then\
        make -C https $(DEBUG) install PORT=8000;\
	else\
		make -C https $(DEBUG) install PORT=$(PORT);\
    fi
	
debug:	DEBUG = debug
debug:	CFLAGS += -g -D DEBUG
debug:	PORT=8000
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
