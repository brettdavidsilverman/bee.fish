OUTPUT = ../../build/Parser
DEPS = Makefile ../../Config.hpp ../Config.hpp Config.hpp ../Miscellaneous/Miscellaneous.hpp Parser.hpp UTF8Character.hpp Character.hpp And.hpp Or.hpp Not.hpp Optional.hpp Word.hpp Range.hpp Repeat.hpp Char.hpp Rules.hpp Capture.hpp Test.hpp main.cpp

$(OUTPUT):	$(DEPS)
	g++ $(CFLAGS) main.cpp -o $(OUTPUT) -lboost_system -lboost_thread -lpthread -latomic

test:	$(OUTPUT)
	$(OUTPUT) -test

clean:
	rm -f $(OUTPUT)

run:	$(OUTPUT)
	$(OUTPUT)

debug:	DEBUG = debug
debug:	CFLAGS += -g -DDEBUG
debug:	$(OUTPUT)