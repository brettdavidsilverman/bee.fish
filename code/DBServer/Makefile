CC = g++
CFLAGS = -std=c++17 -fmax-errors=1
# -Ofast
DEPS=main.cpp DBServer.hpp Test.hpp StreamToDB.hpp StreamFromDB.hpp Makefile HomePage.html NotFound.html ErrorPage.html DBWebRequest.hpp JSONPath.hpp
BUILD = ../../build
OUTPUT = $(BUILD)/DBServer
TEMP = $(BUILD)

$(OUTPUT):	$(DEPS) $(TEMP)/NotFound.o $(TEMP)/HomePage.o $(TEMP)/ErrorPage.o
	g++ $(CFLAGS) $(TEMP)/NotFound.o $(TEMP)/HomePage.o $(TEMP)/ErrorPage.o main.cpp -o $(OUTPUT) -lpthread

$(TEMP)/NotFound.o:	NotFound.html
	ld -r -b binary NotFound.html -o $(TEMP)/NotFound.o

$(TEMP)/HomePage.o:	HomePage.html
	ld -r -b binary HomePage.html -o $(TEMP)/HomePage.o

$(TEMP)/ErrorPage.o:	ErrorPage.html
	ld -r -b binary ErrorPage.html -o $(TEMP)/ErrorPage.o

test:	$(OUTPUT)
	../../stop.sh 8000
	$(BUILD)/DBServer -test

debug:	clean
debug:	DEBUG = debug
debug:	CFLAGS += -g -D DEBUG -rdynamic
debug:	$(OUTPUT)

install:	$(OUTPUT)
	sudo setcap cap_net_bind_service=ep $(OUTPUT)
	cd ../.. && ./start.sh $(PORT)

clean:
	rm -f $(TEMP)/NotFound.o
	rm -f $(TEMP)/HomePage.o
	rm -f $(TEMP)/ErrorPage.o
	rm -f $(TEMP)/DBServer.data
	rm -f $(OUTPUT)
