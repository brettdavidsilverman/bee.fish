HTTPS=bee.fish/build/https

all:	https  Makefile

https:	$(HTTPS)

$(HTTPS):
	cd bee.fish && make $(DEBUG) test

test:
	cd bee.fish && make $(DEBUG) test

clean:
	cd bee.fish && sudo make clean

stop:
	cd bee.fish/code/https && ./stop.sh $(PORT)
	
start:	https 
	cd bee.fish/code/https && ./start.sh $(PORT)

install:	start

restart:	stop start

debug:	DEBUG = debug
debug:	CFLAGS += -g -DDEBUG
debug:	https
