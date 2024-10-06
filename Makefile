MOD_DATABASE=Apache2/mod_database/mod_database.so
HTTPS=bee.fish/build/https
PORT=8000
DATA_FILE=~/data/bee.fish.data

all:	data https Apache2 Makefile

data:
	touch $(DATA_FILE)
	sudo chgrp www-data $(DATA_FILE)
	sudo chmod g+w $(DATA_FILE)

https:	data $(HTTPS)

Apache2:	data $(MOD_DATABASE)

$(MOD_DATABASE):
	cd Apache2/mod_database && sudo make $(DEBUG) install reload test
	
$(HTTPS):
	cd bee.fish && make $(DEBUG) test
	

test:	data
	cd bee.fish && make $(DEBUG) test
	cd Apache2/mod_database && sudo make $(DEBUG) install reload test
	
clean:
	cd bee.fish && sudo make clean
	cd Apache2/mod_database && sudo make clean $(DEBUG)

stop:
	cd bee.fish/code/https && ./stop.sh
	cd Apache2/mod_database && sudo make stop

start:	data https Apache2
	cd bee.fish/code/https && ./start.sh $(PORT)
	cd Apache2/mod_database && sudo make start

restart:	stop start

debug:	stop
debug:	DEBUG = debug
debug:	CFLAGS += -g -DDEBUG
debug:	start
