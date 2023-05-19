#ifndef BEE_FISH_PARSER__AND_H
#define BEE_FISH_PARSER__AND_H
#include <ostream>
#include <vector>
#include <memory>
#include "Parser.hpp"
#include "UTF8Character.hpp"

using namespace std;

namespace BeeFishParser {

   class And : public Character {
   protected:
      std::shared_ptr<Parser> _lhs; // Left hand side
      std::shared_ptr<Parser> _rhs; // Right hand side
 
   public:

      using Character::read;

      And(
         const Parser& lhs,
         const Parser& rhs
      ) :
         _lhs(lhs.copy()),
         _rhs(rhs.copy())
      {
      }
      
      virtual ~And()
      {
      }

      virtual bool read(
         const Character& character
      ) override
      {

         bool matched = false;

         if ( _lhs->_result == NullOpt ) {
            matched = _lhs->read(character);
         }
         
         if (!matched &&
            _lhs->_result == true &&
            _rhs->_result == NullOpt)
         {
            matched = _rhs->read(character);
         }

         if (matched) {
            if (_lhs->_result == true &&
                _rhs->_result == true )
            {
               setResult(true);
            }
         }
         
         if (_lhs->_result == false ||
             _rhs->_result == false )
         {
            setResult(false);
         }


         return matched;

         
         
      }

      virtual Parser* copy() const {
         return new And(*_lhs, *_rhs);
      }
      
   };

};

#endif
