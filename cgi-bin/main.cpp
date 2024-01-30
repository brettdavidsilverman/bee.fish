#include <stdio.h>
#include <iostream>
#include <chrono>
#include <sstream>

using namespace std;

int main(int argc, const char* argv[]) {

   cout << "Content-type: text/plain; charset=utf-8" << "\r\n"
        << "\r\n";
        
   cout << "database.bee.fish"
           << "\r\n"
        << "C++ run time: "
           << __cplusplus
           << "\r\n"
        << "Version: "
           << "0.1"
           << "\r\n";

   return 0;
}
