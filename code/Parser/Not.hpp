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
         char c
      ) 
      override
      {
         bool matched =
            _not->read(c);

         //std::cerr << c << ":" << _not->_result << std::flush;
  
         if (_not->_result == false)
            setResult(true);
         else if (_not->_result == true)
            setResult(false);

         return !matched;
      }

     virtual Parser* copy() const {
        return new Not(*_not);
     }

   };
   
};

#endif
