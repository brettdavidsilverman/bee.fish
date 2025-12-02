rm -f trace.txt
valgrind --log-file="trace.txt" ../../build/web-request -test
