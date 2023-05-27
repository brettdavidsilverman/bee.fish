#include <iostream>
#include <filesystem>

#include "Config.hpp"
#include "../Miscellaneous/Miscellaneous.hpp"
#include "../Parser/Parser.hpp"
#include "../Parser/Test.hpp"
#include "JSON.hpp"
#include "Test.hpp"

using namespace std;
using namespace BeeFishMisc;
using namespace BeeFishParser;
using namespace BeeFishJSON;

int main(int argc, const char* argv[]) {
   //testPattern(Array(), "[1,2,3]", true);
   testPattern(
      Repeat(Word("abcd"))// and Word("cd"))
      and Word("abee")// and BeeFishJSON::Number())
   , "abcdabcdabcdabee", true);
return 0;
   cerr << "bee.fish.json"
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

   if (hasArg(argc, argv, "-test") >= 0)
   {
      cout << "Testing json..." << endl << endl;
      if (!BeeFishJSON::test())
         return 1;
            
      return 0;
   }
   
   static const Parser& parser = BeeFishJSON::JSON();
  
   cin >> parser;
 

   if (parser.result() == true)
   {
      cout << "Valid JSON" << endl;
      return 0;
   }
   else if (parser.result() == false)
   {
      cout << "Invalid JSON" << endl;
      return 1;
   }
   else
   {
      cout << "Insufficient data" << endl;
      return 1;
   }

   return 0;
}
