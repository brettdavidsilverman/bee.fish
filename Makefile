all:
	cd bee.fish && make clean $(DEBUG) test
	#tail /home/bee/debug.txt -f &
	cd Apache2/mod_database && sudo make $(DEBUG) clean install restart test
	
debug:	clean
debug:	DEBUG = debug
debug:	CFLAGS += -g -DDEBUG
debug:	PORT = 8000
debug:	install
