#include <iostream>
#include <filesystem>

#include "../config.h"
#include "../parser/parser.h"
#include "../parser/test.h"
#include "json.h"
#include "test.h"

#include "../Script/Script.hpp"

using namespace std;
using namespace BeeFishParser;
using namespace BeeFishJSON;
using namespace BeeFishMisc;

int main(int argc, const char* argv[]) {
    /*
   BeeFishJSON::JSONParser p;
   cout << "{\"hello\":\"world\"" << endl;
   p.read("{\"hello\":\"world\"");
   if (p.result() == nullopt)
      p.eof();

   cout << "RESULT: " << p.getErrorMessage() << endl;

   return 0;
   */
   cout << "bee.fish.json"
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
   
   BeeFishJSON::JSONParser parser;

   parser.read(cin);

   if (parser.matched())
   {
      cout << "Valid JSON: " << endl;
   }
   else
   {
      cout << "Invalid JSON" << endl;
   }


   cout << "Bye" << endl;
   
   return 0;
}
