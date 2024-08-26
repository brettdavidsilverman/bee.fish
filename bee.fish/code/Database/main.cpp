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
using namespace std;

int main(int argc, const char* argv[])
{ 
/*
   Database db;
   Path root(db);
   std::string str1 = "Hello 🤗";
   cerr << "path1" << endl;
   Path path1 = root["str"];
   cerr << "setData" << endl;
   path1.setData(str1);
   std::string str2;
   cerr << "path2" << endl;
   Path path2 = root["str"];
   cerr << "getData" << endl;
   path2.getData(str2);

   bool success = testValue(str1,str2);
   BeeFishMisc::outputSuccess(success);
return 0;
   Branch branch = db.getBranch(0);
   cerr << "Lock Branch" << endl;
   db.lockBranch(0);

   cerr << "Sleep" << endl;
   BeeFishMisc::sleep(10);
   cerr << "Unlock branch" << endl;
   db.unlockBranch(0);

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

   test = true;

   if (test)
   {
      if (!BeeFishDatabase::test())
         return 1;
   }
   
   string filename = "test.data";
   Database database(filename);
   cout << database << endl;
   
   bool read =
      (hasArg(argc, argv, "-read") != -1);
   
   if (read)
   {
      cout << "Read" << endl;
   }
   
   bool large =
      (hasArg(argc, argv, "-large") != -1);

   if (large)
   {
      cout << "Input large.json" << endl;
   }

   bool input =
      (hasArg(argc, argv, "-input") != -1);
      
   if (input)
   {
      cout << "Input" << endl;
   }
   
   bool output =
      (hasArg(argc, argv, "-output") != -1);
      
   if (output)
   {
      cout << "Output" << endl;
   }
   
   if (large)
   {
      JSON2Path json(database);
      ifstream file("/home/bee/bee.fish/large.json");
      json.read(file);
   }

   if (input)
   {
      JSON2Path json(database);
      json.read(cin);
   }
   
   if (output)
   {
      Path2JSON path(database);
      cout << path << endl;
   }

   cout << database << endl;
   cout << "large.json 136771810" << endl;

   return 0;

}
