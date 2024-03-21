#include <iostream>
#include <filesystem>

#include "../config.h"
#include "../parser/parser.h"
#include "../parser/test.h"
#include "json.h"
#include "test.h"

#include "../Script/Script.hpp"
#include "../Database/Database.hpp"

using namespace std;
using namespace BeeFishParser;
using namespace BeeFishJSON;
using namespace BeeFishMisc;

int main(int argc, const char* argv[]) {

   cerr << "bee.fish.json"
           << endl
        << "C++ run time: "
           << __cplusplus
           << endl
        << "Version: "
           << BEE_FISH_JSON_VERSION
           << endl
        << "Parser Version: "
           << BEE_FISH_PARSER_VERSION
           << endl;

   if (hasArg(argc, argv, "-test") >= 0)
   {
      cout << "Testing json..." << endl << endl;
      if (!BeeFishJSON::test())
         return 1;
            
      return 0;
   }
   
   BeeFishJSON::JSON json;

   BeeFishJSON::JSONParser parser(json);
  /*
   
   JSONParser::OnValue onvalue =
      [](const BString& key, JSON& json) {
         cerr << json.value() << endl;
      };

   parser.invokeValue("fieldName", onvalue);
   
*/
   parser.read(cin);

   if (json.matched())// || (json->result() == nullopt))
   {
      cout << "Valid JSON: " << json.value() << endl;
   }
   else
   {
      cout << "Invalid JSON" << endl;
   }

   return 0;

   string line;
   while (!cin.eof())
   {
      cout << "JSON: ";
      
      getline(cin, line);
      
      if (!line.length())
         break;
        
      BeeFishJSON::JSON json;
      
      BeeFishJSON::JSONParser parser(json);
      
      parser.read(line);

      if (json.matched() || (json.result() == nullopt))
      {
         cout << "Valid JSON" << endl;
      }
      else
      {
         cout << "Invalid JSON" << endl;
      }
      
   }

   cout << "Bye" << endl;
   
   return 0;
}
