CC = g++
CFLAGS = -std=c++17 -fmax-errors=1 -Ofast
DEPS=main.cpp DBServer.hpp Test.hpp StreamToDB.hpp StreamFromDB.hpp Makefile HomePage.html 404.html ErrorPage.html JSONParser.hpp DBWebRequest.hpp
BUILD = ../../build
OUTPUT = $(BUILD)/DBServer
TEMP = $(BUILD)

$(OUTPUT):	$(DEPS) $(TEMP)/404.o $(TEMP)/HomePage.o $(TEMP)/ErrorPage.o
	g++ $(CFLAGS) $(TEMP)/404.o $(TEMP)/HomePage.o $(TEMP)/ErrorPage.o main.cpp -o $(OUTPUT) -lpthread -lstdc++fs


$(TEMP)/404.o:	ld -r -b binary 404.html -o 404.o

$(TEMP)/HomePage.o:	ld -r -b binary HomePage.html -o HomePage.o

$(TEMP)/ErrorPage.o:	ld -r -b binary ErrorPage.html -o ErrorPage.o

test:	$(OUTPUT)
	$(BUILD)/DBServer -test

debug:	clean
debug:	DEBUG = debug
debug:	CFLAGS += -g -D DEBUG -rdynamic
debug:	$(OUTPUT)

install:	$(OUTPUT)
	cd ../.. && ./start.sh $(PORT)

clean:
	rm -f $(TEMP)/404.o
	rm -f $(TEMP)/HomePage.o
	rm -f $(TEMP)/ErrorPage.o
	rm -f $(TEMP)/DBServer.data
	rm -f $(OUTPUT)
