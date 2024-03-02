all:
	cd bee.fish && make $(DEBUG)
	cd bee.fish && make test
	cd Apache2/mod_database && make $(DEBUG) install restart test
	
debug:	clean
debug:	DEBUG = debug
debug:	CFLAGS += -g -DDEBUG
debug:	PORT = 8000
debug:	install
