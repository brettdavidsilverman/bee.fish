all: build
	cd code && make

build:
	mkdir build

clean:
	rm -f -r build 