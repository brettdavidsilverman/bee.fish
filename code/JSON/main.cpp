#include <iostream>
#include <filesystem>
#include <boost/json/src.hpp>
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

std::optional<bool> homeGrownParser();
std::optional<bool> boostParser();

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

   if (hasArg(argc, argv, "-test") >= 0)
   {
      cout << "Testing json..." << endl << endl;
      if (!BeeFishJSON::test())
         return 1;
            
      return 0;
   }

   bool useHomeGrown = true;
   int useArg = -1;
   if ((useArg = hasArg(argc, argv, "-use")) >= 0 && argc > (useArg + 1))
   {
      if (argv[useArg + 1] == string("home"))
         useHomeGrown = true;
      else if (argv[useArg + 1] == string("boost"))
         useHomeGrown = false;
      else {
         cout << "Invalid use arg; [home|boost]" << endl;
         useHomeGrown = false;
         return 1;
      }
   }


   std::optional<bool> result;

   if (useHomeGrown) {
      cout << "Using home grown parser" << endl;
      result = homeGrownParser();
   }
   else {
      cout << "Using boost parser" << endl;
      result = boostParser();
   }

   if (result == false) {
      cout << "Invalid JSON" << endl;
      return 2;
   }
   else if (result == nullopt) {
      cout << "Insufficient data";
      return 3;
   }
   

   cout << "Valid JSON" << endl;
   return 0;

}

std::optional<bool> homeGrownParser() {
   auto parser = JSON();

   cin >> parser;
 
   cout << endl;

   return parser.result();

}

std::optional<bool> boostParser()
{

   using namespace boost::json;

   stream_parser p;
   boost::json::error_code ec;

   char buffer[1026];
   while (!cin.eof()) {
      cin.read(buffer, sizeof(buffer));
      size_t read = cin.gcount();
      p.write(
         std::string(buffer, read),
         ec
      );
      if (ec)
         return false;
      if (read < sizeof(buffer))
         break;
   }
   
   if (p.done())
      return true;
   else
      return nullopt;
}
