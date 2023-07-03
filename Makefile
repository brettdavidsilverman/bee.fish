all:
	- mkdir build
	cd code && make $(DEBUG)

install:	all
	./start.sh $(PORT)

test:
	-  mkdir build
	cd code && make test
#	curl http://bee.fish/test -H "Content-Type: application/json" -d {}
#	curl http://bee.fish/test -H "Content-Type: text/plain" -H "Content-Length: 11" -d "Hello World" | grep "Hello World"
#	curl http://bee.fish/test -H "Content-Type: application/json" --data-binary "@sample.json"

clean:
	rm -f -r build

debug:	clean
debug:	DEBUG = debug
debug:	CFLAGS += -g -DDEBUG
debug:	PORT = 8080
debug:	install
