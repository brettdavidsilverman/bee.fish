all: build
	cd code && make

build:
	mkdir build

install:
	sudo setcap cap_net_bind_service=ep ./build/WebServer
	./build/WebServer

clean:
	rm -f -r build