#include <iostream>
#include <fstream>
#include <iostream>
#include <codecvt>
#include <locale>

#include "../Miscellaneous/Miscellaneous.hpp"

#include "config.hpp"
#include "Parser.hpp"
#include "UTF8Character.hpp"
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
            
   }

   
   class Number : public Character
   {
   
   public:
      Number() {
      }
      
      virtual ~Number()
      {
      }
      
   public:
     
      Character _plus("+");
      Character _minus("-");
      auto _sign = _plus or _minus;
      auto _integerChar = 
         Range("0", "9");
      auto _integer = Repeat(_integerChar);
/*
      
      class IntegerChar : public Range {
      public:
         IntegerChar() : Range('0', '9') {

         }
      };

      Capture* _integer =
         new Capture(
            new Repeat<IntegerChar>(1)
         );
      
      And* _number = new And(
         new Optional(_sign),
         _integer
      );
      
      virtual void write(
         ostream& out,
         size_t tabIndex = 0
      ) const
      {
         if (result() == false)
         {
            out << "<error {" << this->character() << "} >" << endl;
            return;
         }

        // out << *_sign << endl;
         
         if (_plus->matched())
            out << "+";
         else if (_minus->matched())
            out << "-";
         else
            out << " ";
            
         out << _integer->value();
         
     }
*/
   };
   
   string line;
   while (!cin.eof())
   {
      cout << "Number: ";
      
      getline(cin, line);
      
      if (!line.length())
         break;
         /*
      Number number;
     
      Parser parser(number);
      
      parser.read(line);
   
      if (parser.result() == false)
         cout << "Invalid number" << endl;
         
      number.write(cout);
      cout << endl;
      */
   }
  
   cout << "Bye" << endl;
     
   return 0;
}