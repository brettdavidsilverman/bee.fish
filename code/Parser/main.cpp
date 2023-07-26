#include <iostream>
#include <fstream>
#include <iostream>
#include <codecvt>

#include "../Miscellaneous/Miscellaneous.hpp"

#include "Config.hpp"
#include "Test.hpp"
#include "Number.hpp"

using namespace std;
using namespace BeeFishMisc;
using namespace BeeFishParser;

int main(int argc, const char* argv[]) {

   if (true)
   {

      std::string capture;
/*
      const auto word =
         Capture(
            not Word("hello") and
            Invoke(
               Word("boo"),
               
            capture
         );

      bool success = testPattern(
         word,
         "worldboo",
         nullopt
      );

      cout << endl << "*" << capture << endl;

      return 0;
*/
   }

   cout << "bee.fish.parser"
           << endl
        << "C++ run time: "
           << __cplusplus
           << endl
        << PARSER_VERSION
           << endl;

   if (hasArg(argc, argv, "-test") >= 0)
   {
      if (!BeeFishParser::test())
         return 1;

      return 0;
   }

   std::string line;
   while (!cin.eof())
   {
      cout << "Number: ";
      
      getline(cin, line);
      
      if (!line.length())
         break;

      auto lineEnd =
         Character("\r") and
         Character("\n");

      std::string string;

      auto complete =
         Capture(
            NumberExample(),
            string
         ) and 
         lineEnd;
         
     
      line += "\r\n";

      complete.read(line);

      if (complete.result() == false)
         cout << "Invalid number" << endl;
      else if (complete.result() == true) {
         float number = atof(string.c_str());
         cout << "Valid number: " << number << endl;
      }
      else
         cout << "Insufficient data" << endl;

      cout << endl;

   }
  
   cout << "Bye" << endl;
     
   return 0;
}