#include <iostream>
#include <filesystem>
#include "Config.hpp"
#include "../Miscellaneous/Miscellaneous.hpp"
#include "../Parser/Parser.hpp"
#include "../Parser/Test.hpp"
#include "JSON.hpp"
#include "Test.hpp"
#include "Config.hpp"

using namespace std;
using namespace BeeFishMisc;
using namespace BeeFishParser;
using namespace BeeFishJSON;


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

   bool useHomeGrown = true;
   int useArg = -1;

   if (hasArg(argc, argv, "-test") >= 0)
   {
      cout << "Testing json..." << endl << endl;
      if (!BeeFishJSON::test())
         return 1;
            
      return 0;
   }



   std::optional<bool> result;
   
   JSON* parser = new JSON();

   result = testParser(parser);

   Variable var;
   if (parser->_variable) {
      cerr << "one" << endl;
      cerr << *(parser->_variable) << endl;
      var = *(parser->_variable);
   }
   
   delete parser;

   if (result == false) {
      cout << "Invalid JSON" << endl;
      return 2;
   }
   else if (result == nullopt) {
      cout << "Insufficient data";
      return 3;
   }
   

   cerr << "two" << endl;
   cout << var
        << " Valid JSON"
        << endl;
        
   return 0;

}

std::optional<bool> testParser(Parser* parser) {

   cin >> *parser;
 
   if (parser->result() == nullopt)
      parser->flush();

   cout << endl;

   return parser->result();

}