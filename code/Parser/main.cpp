#include <iostream>
#include <fstream>
#include <iostream>
#include <codecvt>
#include <locale>

#include "config.hpp"
#include "Parser.hpp"
#include "UTF8Character.hpp"
#include "Test.hpp"

using namespace std;
using namespace BeeFishParser;

int main(int argc, const char* argv[]) {

   using namespace BeeFish;

   cerr << "bee.fish.parser"
           << endl
        << "C++ run time: "
           << __cplusplus
           << endl
        << PARSER
           << endl;

   if (hasArg(argc, argv, "-test") >= 0)
   {
      if (!BeeFishParser::test())
         return 1;
            
      return 0;
   }

   
   /*
   class Number : public Match
   {
   
   public:
      Number() : Match()
      {
         _match = _number;
      }
      
      virtual ~Number()
      {
      }
      
   public:
     
      BeeFishParser::Character* _plus =
         new BeeFishParser::Character('+');
         
      BeeFishParser::Character* _minus = 
         new BeeFishParser::Character('-');
         
      Or* _sign = new Or(
         _plus,
         _minus
      );
      
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

   };
   */
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