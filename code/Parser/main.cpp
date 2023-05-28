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


class A : public Repeat {

   public:
      A() : Repeat(
        // Character(","),
         Character("a")
      )
      {
      }

      virtual Parser* copy() const
      {
         return new A();
      }

      virtual bool read(char c)
      override
      {
         cerr << c << flush;
         return Repeat::read(c);
      }


};

int main(int argc, const char* argv[]) {

   if (true)
   {
      bool success = true;
/*
      testPattern(
         Repeat(
            Character(",") and
             Repeat(
               Character("a")
             )
         ),
         ",aaa,aa",
         std::nullopt
      );

*/
      testPattern(
         Repeat(
            Character(",") and
            Repeat(Character("a"))
         ),
         ",aa,aaa,",
         std::nullopt
      );

      //testNumber();
      //return 0;
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