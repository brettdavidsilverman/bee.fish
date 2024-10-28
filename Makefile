HTTPS=build/https
ifeq ("$(PORT)","")
	PORT=443
endif

all:
all:	https
all:	start

https:
	cd code/https && make $(DEBUG) test PORT=$(PORT)

test:
	cd code/https && make $(DEBUG) test PORT=$(PORT)

clean:
	cd code/https && make clean

stop:
	cd code/https && ./stop.sh $(PORT)
	
start:
start:	https
	cd code/https && ./start.sh $(PORT)

install:	start

restart:	stop start

debug:	DEBUG = debug
debug:	CFLAGS += -g -DDEBUG
debug:	PORT=8000
debug:	https
