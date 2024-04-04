rm -f trace.txt
valgrind --tool=memcheck --leak-check=full --track-origins=yes --log-file="trace.txt"  ../../build/parser -test
cat trace.txt