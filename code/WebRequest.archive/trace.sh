rm -f trace.txt
valgrind --log-file="trace.txt" ../../build/WebRequest -test
