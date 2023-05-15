#ifndef BEE_FISH_PARSER__NOT_HPP
#define BEE_FISH_PARSER__NOT_HPP

#include "Character.hpp"

namespace BeeFishParser {

   class Not : public Character {

   public:

      Not(const Character& value)
         : Character(value)
      {
      }
      
      virtual ~Not() {
      }

      virtual void setResult(
         Optional result
      ) override
      {
         if (result == true)
            result = false;
         else if (result == false)
            result = true;

         Parser::setResult(result);
      }
   
   };
   
};

#endif
