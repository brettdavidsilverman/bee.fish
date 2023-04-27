OUTPUT = ../../build/WebServer

$(OUTPUT): Makefile ../../config.hpp ../config.hpp config.hpp ../Miscellaneous/Miscellaneous.hpp WebServer.hpp main.cpp
	g++ main.cpp -o $(OUTPUT) -lboost_system -lboost_thread -lpthread -latomic
