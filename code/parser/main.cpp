#include <iostream>
#include <fstream>
#include "../Miscellaneous/Miscellaneous.hpp"
#include "parser.h"
#include "test.h"

using namespace std;
using namespace BeeFishMisc;
using namespace BeeFishParser;
    
namespace Test {
    
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
      
        Match* _plus =
            new Character('+');
            
        Match* _minus = 
            new Character('-');
            
        Match* _sign = new Capture(
            new Or(
                _plus,
                _minus
            )
        );
        
        class IntegerCharacter :
            public Range
        {
        public:
            IntegerCharacter() : Range('0', '9') {

            }
        };

        Capture* _integer =
            new Capture(
                new Repeat<IntegerCharacter>(1)
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
                out << "{\"error\": \"Failed on; character " << this->character() << "\"}" << endl;
                return;
            }

            if (_sign->matched())
                out << _sign->value();
                
            out << _integer->value();
            
      }
      

    };
    
    std::ostream& operator <<
    (
        std::ostream& output,
        const Number& number
    )
    {
        number.write(output);
        
        return output;
    }
    
         
}
    
    
int main(int argc, const char* argv[]) {
    
    using namespace Test;
    
    cout << "bee.fish.parser"
              << endl
          << "C++ run time: "
              << __cplusplus
              << endl
          << "Version: "
              << BEE_FISH_PARSER_VERSION
              << endl
          << "Match size: "
              << sizeof(Match)
              << endl;

    if (hasArg(argc, argv, "-test") >= 0)
    {
        cout << "Testing parser..." << endl << endl;
        if (!BeeFishParser::test())
            return 1;
                
        return 0;
    }
    
    
    string line;
    do 
    {
        cout << "Number: ";
        
        Test::Number number;
        
        cin >> number;
        
        if (number.matched())
            cout << number << endl;
        else
            break;
            
    }
    while (!cin.eof());
    
    
  
    cout << "Bye" << endl;
      
    return 0;
}