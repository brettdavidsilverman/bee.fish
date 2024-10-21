#include <iostream>
#include <filesystem>

#include "../config.h"
#include "../parser/parser.h"
#include "../parser/test.h"
#include "version.h"
#include "web-request.h"
#include "test.h"

using namespace std;
using namespace BeeFishParser;
using namespace BeeFishJSON;

int main(int argc, const char* argv[]) {

   cout << "bee.fish.web-request"
           << endl
        << "C++ run time: "
           << __cplusplus
           << endl
        << "Version: "
           << BEE_FISH_WEB_REQUEST_VERSION
           << endl
        << "Parser Version: "
           << BEE_FISH_PARSER_VERSION
           << endl;

   if (hasArg(argc, argv, "-test") >= 0)
   {
      cout << "Testing web-request..." << endl << endl;
      if (!BeeFishWeb::testRequest())
         return 1;
   }

   return 0;
}
