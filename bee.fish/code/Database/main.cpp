#include <stdio.h>
#include <iostream>
#include <chrono>
#include <sstream>

#include "../Miscellaneous/Miscellaneous.hpp"
#include "../Miscellaneous/SigHandler.hpp"

#include "Database.hpp"
#include "Path.hpp"
#include "Test.hpp"

using namespace BeeFishDatabase;
using namespace BeeFishMisc;

int main(int argc, const char* argv[]) {

   cout << "bee.fish.database"
           << endl
        << "C++ run time: "
           << __cplusplus
           << endl
        << "Version: "
           << DATABASE_VERSION
           << endl;

  // signal(SIGSEGV, sigHandler);
 //  signal(SIGABRT, sigHandler);

   bool test =
      (hasArg(argc, argv, "-test") != -1);
   
   if (test)
   {
      if (BeeFishDatabase::test())
         return 0;
         
      return 1;
   }
   
   bool read =
      (hasArg(argc, argv, "-read") != -1);
   
   if (read)
   {
      cout << "Read" << endl;
   }
   
   bool output =
      (hasArg(argc, argv, "-output") != -1);
      
   if (output)
   {
      cout << "Output" << endl;
   }

   bool input =
      (hasArg(argc, argv, "-input") != -1);
      
   if (input)
   {
      cout << "Input" << endl;
   }
   
   string fileName = "test.data";
   
   Database database(fileName);
   cout << database;
   Path root(database);
   
   if (output)
   {
      Path2JSON path = root;
      cout << path;
      return 0;
   }
   
   if (input)
   {
      JSON match;
      JSON2Path json(root, match);
      json.read(cin);
      return 0;
   }
 
   
}
