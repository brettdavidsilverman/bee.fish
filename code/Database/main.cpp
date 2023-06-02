#include <stdio.h>
#include <iostream>
#include <chrono>
#include <sstream>

#include "../Miscellaneous/Miscellaneous.hpp"

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
           << BEE_FISH__DATABASE_VERSION
           << endl;
 
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
      cerr << "Read" << endl;
   }
   
   bool output =
      (hasArg(argc, argv, "-output") != -1);
      
   if (output)
   {
      cerr << "Output" << endl;
   }

   bool input =
      (hasArg(argc, argv, "-input") != -1);
      
   if (input)
   {
      cerr << "Input" << endl;
   }
   
   string fileName = "data";
   
   Database database(fileName);
   cerr << database;
   Path<PowerEncoding> root(database);
   Path path(root);
   
   if (output)
   {
      cout << path;
      return 0;
   }
   
   if (input)
   {
      cin >> path;
      return 0;
   }
 
   
}
