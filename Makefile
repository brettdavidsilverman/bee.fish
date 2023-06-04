all:
	- mkdir build
	cd code && make $(DEBUG)

install:	all
	./start.sh $(PORT)

test:
	-  mkdir build
	cd code && make test

clean:
	rm -f -r build

debug:	clean
debug:	DEBUG = debug
debug:	CFLAGS += -g -DDEBUG
debug:	PORT = 8080
debug:	install
