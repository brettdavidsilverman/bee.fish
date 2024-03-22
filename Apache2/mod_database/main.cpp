#include <iostream>
#include <filesystem>
#include "Config.hpp"
#include "Miscellaneous/Miscellaneous.hpp"
#include "parser/parser.h"
#include "parser/test.h"
#include "Database/Database.hpp"
#include "Test.hpp"
#include "json/test.h"

using namespace std;
using namespace BeeFishMisc;
using namespace BeeFishParser;
using namespace BeeFishApache2;


std::optional<bool> testParser(Parser* parser);

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
      cout << "Testing json..." << endl << endl;
      if (!BeeFishJSON::test())
         return 1;
         
      cout << "Testing apache2..." << endl << endl;
      if (!BeeFishApache2::test())
         return 1;
            
      return 0;
   }
   


   Database database("test.data");
   
   
   std::optional<bool> result;
   
   JSON match;
   JSON2Path* parser = new JSON2Path(database, match);

   result =
      BeeFishApache2::testParser(parser);
  
   int returnCode = 0;

   if (result == false) {
      string error =
         parser->getErrorMessage();
      cout << error << endl;
      returnCode = 2;
   }
   else if (result == nullopt) {
      cout << "Insufficient data";
      returnCode = 3;
   }
   else {
      cout << "Stored in database" << endl;
      
    //  Database database2("test.data");
      
     // Path2JSON start(database2);
      //cout << start << endl;
   }
   
   delete parser;
   
   return returnCode;

}

