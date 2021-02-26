#include <iostream>
#include "../parser/parser.h"
#include "version.h"
#include "json.h"
#include "test.h"

using namespace std;
using namespace bee::fish::parser;
using namespace bee::fish::json;


int main(int argc, char* argv[]) {
   
   cerr << "bee.fish.server.parser.json"
           << endl
        << "C++ run time: "
           << __cplusplus
           << endl
        << "Version: "
           << BEE_FISH_SERVER_JSON_VERSION
        << endl;
           
   if (!bee::fish::parser::test())
      return 1;
      
   if (!bee::fish::json::test())
      return 2;
      /*
  // Request req;
   JSON* parser = new JSON();

   cerr << "Reading from stdin." << endl;
   optional<bool> ok =
      parser->read(cin, true);
   
   if (ok == true) {
      cerr << endl
           << "ok joe" << endl
           << endl;
   }
   else
      cerr << endl << "Fail" << endl;
 
   delete parser;
   */
   
   return 0;
}