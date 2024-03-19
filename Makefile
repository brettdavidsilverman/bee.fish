all:
	cd bee.fish && make $(DEBUG) test
	#tail /home/bee/debug.txt -f &
	cd Apache2/mod_database && sudo make $(DEBUG) install reload test

test:
	cd bee.fish && make test
	cd Apache2/mod_database && sudo make install reload test
clean:
	cd bee.fish && make clean
	cd Apache2/mod_database && sudo make clean
	
debug:	clean
debug:	DEBUG = debug
debug:	CFLAGS += -g -DDEBUG
debug:	PORT = 8000
debug:	install
