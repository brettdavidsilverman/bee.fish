MOD_DATABASE=Apache2/mod_database/mod_database.so
DATABASE=bee.fish/build/Database

all:	$(DATABASE) $(MOD_DATABASE) Makefile
	
$(DATABASE):
	cd bee.fish && make $(DEBUG) test
	
$(MOD_DATABASE):
	cd Apache2/mod_database && sudo make $(DEBUG) install reload test
	
test:
	cd bee.fish && make $(DEBUG) test
	cd Apache2/mod_database && sudo make $(DEBUG) install reload test
	
clean:
	cd bee.fish && sudo make clean
	cd Apache2/mod_database && sudo make clean $(DEBUG)

apache2:
	cd Apache2/mod_database && sudo make clean $(DEBUG) install reload test

stop:
	cd Apache2/mod_database && sudo make stop

start:
	cd Apache2/mod_database && sudo make start

restart:	stop start

debug:	DEBUG = debug
debug:	CFLAGS += -g -DDEBUG
debug:	PORT = 8000
debug:	all
