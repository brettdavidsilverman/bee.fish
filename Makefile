ifeq ("$(PORT)","")
	PORT=8000
endif

all:	https

build:
	- mkdir build

https:	build
	make -C code $(DEBUG) $(TEST) PORT=$(PORT)

test:	TEST=test
test:	build https

clean:
	make -C code clean

stop:
	make -C code/https $(DEBUG) stop PORT=$(PORT)
	
start:
	make -C code/https $(DEBUG) start PORT=$(PORT)

restart:	stop start

debug:	DEBUG = debug
debug:	CFLAGS += -g -DDEBUG
debug:	PORT=8000
debug:	all

load:
		make -C code load
