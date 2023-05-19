#ifndef BEE_FISH_PARSER__OPTIONAL_HPP
#define BEE_FISH_PARSER__OPTIONAL_HPP
#include <string>
#include <stdexcept>
#include <memory>
#include "Parser.hpp"

namespace BeeFishParser {

   using namespace std;

   class Optional : public Character {
   protected:
      std::shared_ptr<Parser> _optional;

   public:
      using Parser::read;

      Optional(const Parser& optional) :
         _optional(optional.copy())
      {
      }

      virtual ~Optional() {
      }

      virtual bool read(
         const Character& character
      ) override
      {

         bool matched =
            _optional->read(character);

         if (_optional->_result == true)
         {
            setResult(true);
            return true;
         } 
         else if (_optional->_result == false)
         {
            return false;
  
         }

         return matched;

      }
   
      virtual Parser* copy() const override {
         return new Optional(*_optional);
      }
   };


}

#endif
