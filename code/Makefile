BUILD = ../build
CFLAGS = -std=c++17 -fmax-errors=1
DEPS=Makefile
TEMP = /var/tmp

all:
all:	$(BUILD)
	make -C b-string $(DEBUG) $(TEST)
	make -C power-encoding $(DEBUG) $(TEST)
	make -C Id $(DEBUG) $(TEST)
	make -C parser $(DEBUG) $(TEST)
	make -C json $(DEBUG) $(TEST)
	make -C Database $(DEBUG) $(TEST) $(LOAD) 
	make -C query $(DEBUG) $(TEST)
	make -C web-request $(DEBUG) $(TEST)
	make -C Script $(DEBUG) $(TEST)
	make -C https $(DEBUG) $(TIME) $(TEST) PORT=$(PORT)

$(BUILD):
	mkdir $(BUILD)
	
install:
	@if [ "$(PORT)" = "" ]; then\
        make -C https $(DEBUG) install PORT=8000;\
	else\
		make -C https $(DEBUG) install PORT=$(PORT);\
    fi
	
debug:
debug:	DEBUG = debug
debug:	CFLAGS += -g -D DEBUG
debug:	PORT=8000
debug:	all

test:
test:	TEST=test
test:	all

time:	CFLAGS += -Ofast
time:	TIME = time
time:	all

load:
load:	LOAD = load
load:	debug

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
