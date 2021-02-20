#include <iostream>
#include "../parser.h"
#include "version.h"
#include "json.h"

using namespace std;
using namespace bee::fish::parser;
using namespace bee::fish::parser::json;


int main(int argc, char* argv[]) {
   
   // Set standard input and output
   // to unbufferd
   
   cerr << "bee.fish.server.parser.json"
           << endl
        << "C++ run time: "
           << __cplusplus
           << endl
        << "Version: "
           << BEE_FISH_SERVER_PARSER_JSON_VERSION
        << endl;
           
  // Request req;
   JSON* parser = new JSON();

   cerr << "Reading from stdin." << endl;
   optional<bool> ok =
      parser->read(cin, true);
   
   if (ok) {
      cerr << endl
           << "ok joe" << endl
           << endl;
   }
   else
      cerr << endl << "Fail" << endl;
 
   delete parser;
}