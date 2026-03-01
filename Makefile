ifeq ("$(PORT)","")
	PORT=443
endif

all:
all:	https
all:	start

build:
	- mkdir build

https:
https:	build
	cd code && make $(DEBUG) $(TEST) PORT=$(PORT)

test:
test:	TEST=test
test:	build https

clean:
	cd code && make clean

stop:
	cd code/https && ./stop.sh $(PORT)
	
start:
start:	https
	cd code/https && ./start.sh $(PORT)

restart:	stop start

debug:
debug:	DEBUG = debug
debug:	CFLAGS += -g -DDEBUG
debug:	PORT=8000
debug:	all

load:
		make -C code load
