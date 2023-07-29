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
/*
      const string filename = "/var/tmp/DBServer.data";
      remove(filename.c_str());

      Database db(filename);

      Path start(db);
      Path next = start["Hello"];
      std::string str = "My Content Type";
      next.setData(str);
      std::string str2;
      next.getData(str2);
return 0;
*/
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
   
   string fileName = "data";
   
   Database database(fileName);
   cout << database;
   Path<Database::Encoding> root(database);
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
