rm -f trace.txt
valgrind  --log-file="trace.txt" --tool=memcheck --track-origins=yes --leak-check=yes ../../build/json -test
cat trace.txt