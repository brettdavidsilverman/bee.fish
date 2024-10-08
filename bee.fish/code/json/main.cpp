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
   JSON integer;
   bool ok = testMatch("integer", &integer, "10", true, "10");
   ok &= testResult("integer type", integer.type() == Type::INTEGER);
   cerr << "INT TYPE: " << integer.type() << endl;
   assert(ok);
      
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
