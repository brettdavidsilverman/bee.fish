OUTPUT = ../../build/Parser
DEPS = Makefile ../../Config.hpp ../Config.hpp Config.hpp ../Miscellaneous/Miscellaneous.hpp Parser.hpp UTF8Character.hpp Character.hpp And.hpp Or.hpp Not.hpp Optional.hpp Word.hpp Range.hpp Repeat.hpp Char.hpp Rules.hpp Capture.hpp Invoke.hpp Test.hpp main.cpp
CFLAGS = -std=c++17 -Wfatal-errors

$(OUTPUT):	$(DEPS)
	g++ $(CFLAGS) main.cpp -o $(OUTPUT) -lboost_system -lboost_thread -lpthread -latomic

test:	clean $(OUTPUT)
	$(OUTPUT) -test

clean:
	rm -f $(OUTPUT)

run:	$(OUTPUT)
	$(OUTPUT)

debug:	clean
debug:	DEBUG = debug
debug:	CFLAGS += -g -DDEBUG
debug:	$(OUTPUT)
