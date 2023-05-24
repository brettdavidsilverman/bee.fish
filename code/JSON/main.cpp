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
   
   string line;
   while (!cin.eof())
   {
      cout << "JSON: ";
      
      getline(cin, line);
      
      if (!line.length())
         break;
        
      auto parser = BeeFishJSON::JSON();
      
      parser.read(line);

      if (parser.result() == true)
      {
         cout << "Valid JSON" << endl;
      }
      else if (parser.result() == false)
      {
         cout << "Invalid JSON" << endl;
      }
      else if (parser.result() == std::nullopt)
      {
         cout << "Insufficient data" << endl;
      }

   }

   cout << "Bye" << endl;
   
   return 0;
}
