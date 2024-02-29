#include <iostream>
#include <filesystem>
#include "Config.hpp"
#include "Miscellaneous/Miscellaneous.hpp"
#include "Parser/Parser.hpp"
#include "Parser/Test.hpp"
#include "JSON/JSON.hpp"
#include "Test.hpp"


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
           << "1.0"
           << endl
        << "JSON Version: "
           << JSON_VERSION
           << endl
        << "Parser Version: "
           << PARSER_VERSION
           << endl;

   int useArg = -1;

   if (hasArg(argc, argv, "-test") >= 0)
   {
      cout << "Testing apache2..." << endl << endl;
      if (!BeeFishApache2::test())
         return 1;
            
      return 0;
   }



   std::optional<bool> result;
   
   JSON* parser = new JSON();

   result =
      BeeFishApache2::testParser(parser);

   Variable var;
   if (parser->_variable) {
      var = *(parser->_variable);
   }
  
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
   else
      cout << var << endl;
      
   delete parser;
   
   return returnCode;

}

