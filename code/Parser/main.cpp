#include <iostream>
#include <fstream>
#include <iostream>
#include <codecvt>
#include <locale>

#include "../Miscellaneous/Miscellaneous.hpp"

#include "config.hpp"
#include "Test.hpp"

using namespace std;
using namespace BeeFishParser;

int main(int argc, const char* argv[]) {


   cerr << "bee.fish.parser"
           << endl
        << "C++ run time: "
           << __cplusplus
           << endl
        << PARSER
           << endl;

   if (BeeFish::hasArg(argc, argv, "-test") >= 0)
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
         
      auto number = 
         Optional(
            Character("+") or
            Character("-")
         ) and
         Repeat(Range("0", "9"));
     
      number.read(line);
      if (number.result() == std::nullopt)
         number.read(".");

      if (number.result() == false)
         cout << "Invalid number" << endl;
      else if (number.result() == true)
         cout << "Valid number" << endl;
      else
         cout << "Insufficient data" << endl;

      cout << endl;

   }
  
   cout << "Bye" << endl;
     
   return 0;
}