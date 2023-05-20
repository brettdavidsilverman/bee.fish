OUTPUT = ../../build/Parser

$(OUTPUT): Makefile ../../config.hpp ../config.hpp config.hpp ../Miscellaneous/Miscellaneous.hpp Parser.hpp UTF8Character.hpp Character.hpp And.hpp Or.hpp Not.hpp Optional.hpp Word.hpp Range.hpp Repeat.hpp Char.hpp Rules.hpp Capture.hpp Test.hpp main.cpp
	g++ $(CFLAGS) main.cpp -o $(OUTPUT) -lboost_system -lboost_thread -lpthread -latomic

debug:	DEBUG = debug
debug:	CFLAGS += -g -DDEBUG
debug:	$(OUTPUT)