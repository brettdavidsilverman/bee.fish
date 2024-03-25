rm trace.txt
valgrind --log-file="trace.txt" --track-origins=yes --leak-check=yes ../../build/power-encoding -test
