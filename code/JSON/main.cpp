#include <iostream>
#include <filesystem>

#include "config.hpp"
#include "../Miscellaneous/Miscellaneous.hpp"
#include "../Parser/Parser.hpp"
#include "../Parser/Test.hpp"
#include "JSON.hpp"
//#include "Test.hpp"

using namespace std;
using namespace BeeFish;
using namespace BeeFishParser;
//using namespace BeeFishJSON;

int main(int argc, const char* argv[]) {

   cerr << "bee.fish.json"
           << endl
        << "C++ run time: "
           << __cplusplus
           << endl
        << "Version: "
           << JSON_VERSION
           << endl
        << "Parser Version: "
           << PARSER_VERSION
           << endl;

   if (hasArg(argc, argv, "-test") >= 0)
   {
      cout << "Testing json..." << endl << endl;
     // if (!BeeFishP::test())
     //    return 1;
            
      return 0;
   }
   
/* BeeFishJSON::JSON json;

   BeeFishJSON::JSONParser parser(json);
 
   parser.read(cin);

   if (json.matched())// || (json->result() == BeeFishMisc::nullopt))
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

      if (json.matched() || (json.result() == BeeFishMisc::nullopt))
      {
         cout << "Valid JSON" << endl;
      }
      else
      {
         cout << "Invalid JSON" << endl;
      }
      
   }
*/
   cout << "Bye" << endl;
   
   return 0;
}
