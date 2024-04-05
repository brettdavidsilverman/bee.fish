DATABASE=bee.fish/build/Database
JSON=bee.fish/build/json

all:	$(JSON) $(DATABASE)
	
$(JSON):
	cd bee.fish && make $(DEBUG) test
	
$(DATABASE):
	cd Apache2/mod_database && sudo make $(DEBUG) install reload test
	
	
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
