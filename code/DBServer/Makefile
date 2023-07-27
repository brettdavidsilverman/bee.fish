CC = g++
CFLAGS = -std=c++17 -fmax-errors=1 -Ofast
DEPS=main.cpp DBServer.hpp Test.hpp StreamToDB.hpp StreamFromDB.hpp Makefile HomePage.html 404.html ErrorPage.html JSONParser.hpp DBWebRequest.hpp
BUILD = ../../build
OUTPUT = $(BUILD)/DBServer
TEMP = $(BUILD)

$(OUTPUT):	$(DEPS) $(TEMP)/404.o $(TEMP)/HomePage.o $(TEMP)/ErrorPage.o
	g++ $(CFLAGS) $(TEMP)/404.o $(TEMP)/HomePage.o $(TEMP)/ErrorPage.o main.cpp -o $(OUTPUT) -lpthread -lstdc++fs


$(TEMP)/404.o: 404.html
	objcopy --input binary \
            --output elf64-x86-64 \
            --binary-architecture i386:x86-64 404.html $(TEMP)/404.o

$(TEMP)/HomePage.o: HomePage.html
	objcopy --input binary \
            --output elf64-x86-64 \
            --binary-architecture i386:x86-64 HomePage.html $(TEMP)/HomePage.o

$(TEMP)/ErrorPage.o: ErrorPage.html
	objcopy --input binary \
            --output elf64-x86-64 \
            --binary-architecture i386:x86-64 ErrorPage.html $(TEMP)/ErrorPage.o

test:	$(OUTPUT)
	$(BUILD)/DBServer -test 2>&1 | c++filt

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
