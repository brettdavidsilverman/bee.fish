#include <iostream>
#include <filesystem>
#include <fstream>
#include "Config.hpp"
#include "../Miscellaneous/Miscellaneous.hpp"
#include "../Parser/Parser.hpp"
#include "../Parser/Test.hpp"
#include "JSON2Variable.hpp"
#include "JSON2Path.hpp"
#include "Test.hpp"
#include "Config.hpp"

using namespace std;
using namespace BeeFishMisc;
using namespace BeeFishParser;
using namespace BeeFishJSON;
using namespace BeeFishDatabase;

std::optional<bool> testParser(Parser* parser);

int main(int argc, const char* argv[]) {
   
   cout << "bee.fish.json"
           << endl
        << "C++ run time: "
           << __cplusplus
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
      cout << "Testing json..." << endl << endl;
      if (!BeeFishJSON::test())
         return 1;
            
      return 0;
   }



   std::optional<bool> result;
   
   Database database;
   
   JSON* parser = new JSON2Path(database);

   result = testParser(parser);
  
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
      cout << "Valid JSON" << endl;
   }
   
   delete parser;
   
   return returnCode;

}

std::optional<bool> testParser(Parser* parser) {

   ifstream in("/home/bee/bee.fish/large.json");
   in >> *parser;
   
   //cin >> *parser;
 
   if (parser->result() == nullopt)
      parser->eof();

   cout << endl;

   return parser->result();

}