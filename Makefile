HTTPS=build/https

all:	https  Makefile start

https:	$(HTTPS)

$(HTTPS):
	cd code && make $(DEBUG) test PORT=$(PORT)

test:
	cd code && make $(DEBUG) test PORT=$(PORT)

clean:
	cd code && sudo make clean

stop:
	cd code/https && ./stop.sh $(PORT)
	
start:	https 
	cd code/https && ./start.sh $(PORT)

install:	start

restart:	stop start

debug:	DEBUG = debug
debug:	CFLAGS += -g -DDEBUG
debug:	https
