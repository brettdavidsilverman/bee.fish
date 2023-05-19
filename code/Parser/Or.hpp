#ifndef BEE_FISH_PARSER__OR_HPP
#define BEE_FISH_PARSER__OR_HPP
#include <vector>
#include <memory>
#include "Character.hpp"


namespace BeeFishParser {

   class Or : public Character {
   protected:
      std::shared_ptr<Parser> _lhs; // Left hand side
      std::shared_ptr<Parser> _rhs; // Right hand side
 
   public:

      using Parser::read;

      Or(
         const Parser& lhs,
         const Parser& rhs
      ) :
         _lhs(lhs.copy()),
         _rhs(rhs.copy())
      {
      }
      
      virtual ~Or()
      {
      }

      virtual bool read(const Character& character) {

         bool matched = false;
            
         if ( _lhs->_result == std::nullopt )
         {
            matched = _lhs->read(character);
         }
         
         if ( _rhs->_result == std::nullopt )
         {
            matched |= _rhs->read(character);
         }

         if (matched) {
            if (_lhs->_result == true ||
                _rhs->_result == true )
            {
               setResult(true);
            }
         }
         
         if (_lhs->_result == false &&
             _rhs->_result == false )
         {
            setResult(false);
         }


         return matched;

         
         
      }

      virtual Parser* copy() const {
         return new Or(*_lhs, *_rhs);
      }

      
   };

};

#endif
