#include <iostream>
#include <filesystem>
#include "../Config.hpp"
#include "Database/Database.hpp"
#include "Database/Test.hpp"
#include "Miscellaneous/Miscellaneous.hpp"
#include "Test.hpp"
#include "ParseURI.hpp"

using namespace std;
using namespace BeeFishMisc;
using namespace BeeFishApache2;


int main(int argc, const char* argv[]) {

   cout << "apache2.mod_database"
           << endl
        << "C++ run time: "
           << __cplusplus
           << endl
        << "mod_database Version: "
           << "1.2"
           << endl
        << "Database Version: "
           << DATABASE_VERSION
           << endl;

   int useArg = -1;

   if (hasArg(argc, argv, "-test") >= 0)
   {
      /*
      cout << "Testing database..." << endl << endl;
      if (!BeeFishDatabase::test())
         return 1;
      */
      cout << "Testing apache2..." << endl << endl;
      if (!BeeFishApache2::test())
         return 1;
            
      return 0;
   }
   


   Database database("test.data");
   
   
   std::optional<bool> result;
   
   JSONPathParser parser(database);
  
   parser.read(cin);
   parser.eof();
   
      
   int returnCode = 0;

   if (parser.result() == false) {
      string error =
         parser.getError();
      cout << error << endl;
      returnCode = 2;
   }
   else if (parser.result() == nullopt) {
      cout << "Insufficient data";
      returnCode = 3;
   }
   else {
      cout << "Stored in database" << endl;
      returnCode = 0;
   }
   

   
   return returnCode;

}

