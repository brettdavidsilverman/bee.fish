MOD_DATABASE=Apache2/mod_database/mod_database.so
JSON=bee.fish/build/json

all:	$(JSON) $(MOD_DATABASE) Makefile
	
$(JSON):
	cd bee.fish && make $(DEBUG) test
	
$(MOD_DATABASE):
	cd Apache2/mod_database && sudo make $(DEBUG) install reload test
	
test:	all
	cd bee.fish && make test
	cd Apache2/mod_database && sudo make install stop start test
	
clean:
	#cd bee.fish && make clean
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
