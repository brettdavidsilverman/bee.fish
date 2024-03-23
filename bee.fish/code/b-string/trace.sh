rm -f trace.txt
valgrind --log-file="trace.txt" ../../build/b-string -test
