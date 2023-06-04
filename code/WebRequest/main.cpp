#include <iostream>
#include <filesystem>

#include "Config.hpp"
#include "../Parser/Parser.hpp"
#include "Version.hpp"
#include "WebRequest.hpp"
#include "Test.hpp"

using namespace std;
using namespace BeeFishParser;
using namespace BeeFishJSON;
using namespace BeeFishWeb;

int main(int argc, const char* argv[]) {
 

   cout << "bee.fish.web-request"
           << endl
        << "C++ run time: "
           << __cplusplus
           << endl
        << "Version: "
           << WEB_REQUEST_VERSION
           << endl
        << "Parser Version: "
           << PARSER_VERSION
           << endl;

   if (hasArg(argc, argv, "-test") >= 0)
   {
      cout << "Testing WebRequest..." << endl << endl;
      if (!BeeFishWeb::testWebRequest())
         return 1;
      return 0;
   }

   string line;
   string path;

   auto onpath = [&path](Parser*) {
      cout << path << endl;
      path = "";
   };

   while (true) {

      getline(cin, line);

      if (line == "")
         break;

      auto _url = URL(path, onpath);

      path = "";

      bool read = _url.read(line + "\r\n");

      if (read)
         cout << "Read 😆" << endl;
      else
         cout << "Read 🚫" << endl;

      if (_url.result() == true)
         cout << "Result True" << endl;
      else if (_url.result() == false)
         cout << "Result False" << endl;
      else
         cout << "Result Null" << endl;
   }

   return 0;
}
