#include <iostream>
#include "../parser/parser.h"
#include "version.h"
#include "number.h"

using namespace std;
using namespace bee::fish::parser;
using namespace bee::fish::json;


int main(int argc, char* argv[]) {
   
   // Set standard input and output
   // to unbufferd
   
   cerr << "bee.fish.server.parser.json"
           << endl
        << "C++ run time: "
           << __cplusplus
           << endl
        << "Version: "
           << BEE_FISH_SERVER_JSON_VERSION
        << endl;
   
   // Request req;
   NumberPtr digits;
   
   digits->_capture = true;
   digits->read("269");
   
   cout << digits->_result
        << ":"
        << digits->_value 
        << endl;
        
   if (digits->isMatched())
      cout << "ok" << endl;
   else
   {
      cout << *digits << endl;
      cout << "FAIL" << endl;
   }
   return 0;
   /*

   cerr << "Reading from stdin." << endl;
   parser.read(cin, true);
   bool ok = parser.isMatched();
   
   if (ok) {
      cerr << endl
           << "ok joe" << endl
           << endl;
   }
   else
      cerr << endl << "Fail" << endl;
 */
}