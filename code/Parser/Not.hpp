#ifndef BEE_FISH_PARSER__NOT_HPP
#define BEE_FISH_PARSER__NOT_HPP

#include <memory>
#include "Character.hpp"

namespace BeeFishParser {

   class Not : public Character {
   protected:
      std::shared_ptr<Parser> _not;

   public:
      using Character::read;

      Not(const Parser& value) :
         _not(value.copy())
      {
      }

      virtual ~Not() {
      }

      virtual bool read(
         const UTF8Character& character
      ) 
      override
      {
         bool matched =
            _not->read(character);

         if (matched) {
            if (_not->_result == true)
               setResult(false);
         }
         
         if (_not->_result == false) {
            setResult(true);
            return true;
         }

         return matched;
      }

     virtual Parser* copy() const {
        return new Not(*_not);
     }

   };
   
};

#endif
