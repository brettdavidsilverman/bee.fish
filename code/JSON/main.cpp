#include <iostream>
#include <filesystem>
#include <boost/json/src.hpp>
#include "Config.hpp"
#include "../Miscellaneous/Miscellaneous.hpp"
#include "../Parser/Parser.hpp"
#include "../Parser/Test.hpp"
#include "JSON.hpp"
#include "JSONParser.hpp"
#include "Test.hpp"
#include "Config.hpp"

using namespace std;
using namespace BeeFishMisc;
using namespace BeeFishParser;
using namespace BeeFishJSON;
using namespace BeeFishDatabase;

Parser* homeGrownParser();
Parser* boostParser();
std::optional<bool> testParser(Parser* parser);
Database db(TEMP_FILENAME);

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

   if (hasArg(argc, argv, "-test") >= 0)
   {
      cout << "Testing json..." << endl << endl;
      if (!BeeFishJSON::test())
         return 1;
            
      return 0;
   }



   std::optional<bool> result;
   Parser* parser;

   if (useHomeGrown) {
      cout << "Using home grown parser" << endl;
      parser = homeGrownParser();
   }
   else {
      cout << "Using boost parser" << endl;
      parser = boostParser();
   }

   //remove(TEMP_FILENAME);

   result = testParser(parser);

   delete parser;

   remove(TEMP_FILENAME);

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

Parser* homeGrownParser() {
   auto parser = new JSON();
   return parser;
}

Parser* boostParser()
{

   auto parser = new JSONParser(db);

   return parser;
}

std::optional<bool> testParser(Parser* parser) {

   cin >> *parser;
 
   if (parser->result() == nullopt)
      parser->flush();

   cout << endl;

   return parser->result();

}



/*
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
*/