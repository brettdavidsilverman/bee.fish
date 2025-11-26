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

   //test = false;
   
   if (test)
   {
      if (!BeeFishDatabase::test())
         return 1;
   }
   
   bool unlock =
      (hasArg(argc, argv, "-unlock") != -1);

   
   if (unlock)
   {
      cout << "Unlocking " << DATABASE_FILENAME << endl;
      Database db(DATABASE_FILENAME);
      db.unlockAll();
      return 0;
   }
   
   string filename = DATA_DIR "/test.data";
   JSONDatabase database("Database::main", filename);
   cout << database << endl;
   
   Path root(database.origin());
   JSONPathParser json(root);
   
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
      string largeFilename = WWW_ROOT_DIRECTORY "/large.json";
      cout << "Inputting large " << largeFilename << endl;
      ifstream file(largeFilename);
      json.read(file);
   }
   else if (input)
   {
      json.read(cin);
   }
   
   if (output)
   {
      cout << json << endl;
   }

   cout << database << endl;
   cout << "large.json 127411713" << endl;

   return 0;

}
