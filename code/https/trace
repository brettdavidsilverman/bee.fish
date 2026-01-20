rm -f trace.txt
./stop.sh 8000
valgrind --log-file="trace.txt" --track-origins=yes --leak-check=yes ../../build/https -port 8000 -test
