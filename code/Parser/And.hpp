#ifndef BEE_FISH_PARSER__AND_H
#define BEE_FISH_PARSER__AND_H
#include <ostream>
#include <vector>
#include "Parser.hpp"
#include "UTF8Character.hpp"

using namespace std;

namespace BeeFishParser {

   class And : public Character {
   protected:
      Parser& _lhs; // Left hand side
      Parser& _rhs; // Right hand side
 
   public:

      using Parser::read;

      And(
         Parser& lhs,
         Parser& rhs
      ) :
         _lhs(lhs),
         _rhs(rhs)
      {
      }
      
      virtual ~And()
      {
      }

      virtual bool read(const Character& character) {

         bool matched = false;
            
         if ( _lhs._result == NullOpt ) {
            matched = _lhs.read(character);
         }
         else if (
            _lhs._result == true &&
            _rhs._result == NullOpt
         )
         {
            matched = _rhs.read(character);
         }

         if (matched) {
            if (_lhs._result == true &&
                _rhs._result == true )
            {
               setResult(true);
            }
         }
         
         if (_lhs._result == false ||
             _rhs._result == false )
         {
            setResult(false);
         }


         return matched;

         
         
      }

      
   };

};

#endif
