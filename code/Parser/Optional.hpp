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
      bool _matched = false;

   public:
      using Parser::read;

      Optional(const Parser& optional) :
         _optional(optional.copy())
      {
      }

      virtual ~Optional() {
      }
      /*
      virtual bool read(
         bool bit
      ) override
      {
         throw std::logic_error("Should not reach here");
      }
*/
      virtual bool read(
         const Character& character
      ) override
      {

         bool matched =
            _optional->read(character);

         bool succeeded = false;
         
         if (_optional->_result == true)
         {
            _matched = true;
            succeeded = true;
            
         } 
         else if (!matched)
         {

            _matched = false;
            succeeded = true;
         }

         if (succeeded) {
            setResult(true);
         }

         return matched;
         

      }
   
      virtual Parser* copy() const override {
         return new Optional(*_optional);
      }
   };


}

#endif
