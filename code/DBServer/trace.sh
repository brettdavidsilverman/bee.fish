rm -f trace.txt
valgrind -s --log-file="trace.txt" --track-origins=yes --leak-check=yes ../../build/DBServer -test
