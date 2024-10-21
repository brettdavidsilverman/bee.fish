#include <iostream>
#include <filesystem>
#include <memory>

#include "../config.h"
#include "../parser/parser.h"
#include "../parser/test.h"
#include "Script.hpp"
#include "Test.hpp"

using namespace std;
using namespace BeeFishParser;
using namespace BeeFishJSON;
using namespace BeeFishScript;

int main(int argc, const char* argv[]) {

   cout << "bee.fish.script"
           << endl
        << "C++ run time: "
           << __cplusplus
           << endl
        << "Version: "
           << BEE_FISH_SCRIPT_VERSION
           << endl
        << "JSON Version: "
           << BEE_FISH_JSON_VERSION
           << endl
        << "Parser Version: "
           << BEE_FISH_PARSER_VERSION
           << endl;

   if (hasArg(argc, argv, "-test") >= 0)
   {
      cout << "Testing b-script..." << endl << endl;
      if (!BeeFishScript::test())
         return 1;
            
      return 0;
   }
   
   BeeFishJSON::JSON json;

   ScriptParser parser(json);

   parser.read(cin);
   parser.eof();
 
   if (parser.matched())
   {
      cout << "Valid JSON: " << endl;
      cout << "Parser stack size: " << parser._stack.size() << endl;
      cout << parser.json() << endl;
   }
   else
   {
      cout << "Invalid JSON" << endl;
   }

   return 0;
/*
   string line;
   cout << "JSON" << endl;
      
   while (!cin.eof())

   {
      getline(cin, line);
      
      if (!line.length())
         break;
        
      parser.read(line);

      if (parser.result() == true)
      {
         cout << "Valid JSON" << endl;
         break;
      }
      else if (parser.result() == false)
      {
         cout << "Invalid JSON" << endl;
         break;
      }
      
   }
*/
   cout << "Bye" << endl;
   
   return 0;
}
