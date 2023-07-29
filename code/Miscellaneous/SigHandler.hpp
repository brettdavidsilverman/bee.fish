#ifndef BEEFISH__MISC__SIGHANDLER_HPP
#define BEEFISH__MISC__SIGHANDLER_HPP

#include <stdio.h>
#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>


inline void sigHandler(int sig) {
   void *array[10];
   size_t size;

   // get void*'s for all entries on the stack
   size = backtrace(array, 20);

   // print out all the frames to stderr
   fprintf(stderr, "Error: signal %d:\n", sig);
   backtrace_symbols_fd(array, size, STDERR_FILENO);
}

#endif