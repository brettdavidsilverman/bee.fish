all:
	cd bee.fish && make $(DEBUG) test
	#tail /home/bee/debug.txt -f &
	cd Apache2/mod_database && sudo make $(DEBUG) install restart test

clean:
	cd bee.fish && make clean
	cd Apache2/mod_database && sudo make clean
	
stop:
	cd Apache2/mod_database && sudo make stop

start:
	cd Apache2/mod_database && sudo make start

restart:	stop start

debug:	clean
debug:	DEBUG = debug
debug:	CFLAGS += -g -DDEBUG
debug:	PORT = 8000
debug:	all
