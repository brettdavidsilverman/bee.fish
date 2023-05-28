#ifndef BEE_FISH_PARSER__NOT_HPP
#define BEE_FISH_PARSER__NOT_HPP

#include <memory>
#include "Character.hpp"

namespace BeeFishParser {

   class Not : public Parser {
   protected:
      std::shared_ptr<Parser> _not;

   public:
      using Parser::read;

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

         if (_result != nullopt)
            return true;

         bool matched =
            _not->read(c);

         if (_not->_result == true) {
            setResult(false);
         }
         else if (_not->_result == false)
         {
            setResult(true);
         }
         else if (_not->_result == nullopt)
            return true;

         return !matched;
      }

      virtual Parser* copy()
      const override
      {
         return new Not(*_not);
      }


   };
   
};

#endif
