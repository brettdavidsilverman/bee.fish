#ifndef BEE_FISH_PARSER__NOT_HPP
#define BEE_FISH_PARSER__NOT_HPP

#include <memory>
#include "Character.hpp"

namespace BeeFishParser {

   class Not : public UTF8Character {
   protected:
      Parser* _not;

   public:
      using Parser::read;

      Not(const Parser& value) :
         _not(value.copy())
      {
      }

      virtual ~Not() {
         delete _not;
      }

      virtual bool read(
         char character
      ) 
      override
      {
         if (_not->_result != nullopt ||
              _result != nullopt)
            return true;

         bool matched =
            _not->read(character);
         
         if (_not->_result == false) {
            setResult(true);
         }
         else if (_not->_result == true) {
            setResult(false);
         }
         else if (_not->_result == nullopt)
            return true;

         return !matched;
/*
         if (_result != nullopt)
            return false;

         bool matched =
            _not->read(c);

         if (_not->_result == true) {
            setResult(false);
         }
         else if (_not->_result == false)
         {
            setResult(true);
         }
         else if (!matched || _not->_result == nullopt)
            return true;

         return !matched;
*/
      }

      virtual Parser* copy()
      const override
      {
         return new Not(*_not);
      }


   };
   
};

#endif
