MOD_DATABASE=Apache2/mod_database/mod_database.so
HTTPS=bee.fish/build/https
PORT=443
all:	$(MOD_DATABASE) $(HTTPS) Makefile

$(MOD_DATABASE):
	cd Apache2/mod_database && sudo make $(DEBUG) install reload test
	
$(HTTPS):
	cd bee.fish && make $(DEBUG) test
	

test:
	cd bee.fish && make $(DEBUG) test
	cd Apache2/mod_database && sudo make $(DEBUG) install reload test
	
clean:
	cd bee.fish && sudo make clean
	cd Apache2/mod_database && sudo make clean $(DEBUG)

stop:
	cd bee.fish/code/https && ./stop.sh
#	cd Apache2/mod_database && sudo make stop

start:	$(HTTPS)
	cd bee.fish/code/https && ./start.sh $(PORT)
#	cd Apache2/mod_database && sudo make start

restart:	stop start

debug:	stop
debug:	DEBUG = debug
debug:	CFLAGS += -g -DDEBUG
debug:	PORT=443
debug:	start
