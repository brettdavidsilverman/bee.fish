#ifndef BEE_FISH__PARSER__OPTIONAL_HPP
#define BEE_FISH__PARSER__OPTIONAL_HPP
#include <string>
#include <stdexcept>
#include <memory>
#include "Parser.hpp"

namespace BeeFishParser {

   using namespace std;

   class Optional : public Parser {
   protected:
      Parser* _optional;
      bool _matched {false};
   public:
      using Parser::read;

      Optional(const Parser& optional) :
         _optional(optional.copy())
      {
      }

      virtual ~Optional() {
         delete _optional;
      }

      virtual bool read(
         char c
      ) override
      {
         
   
         bool matched =
            _optional->read(c);

        
         if (_optional->_result == true)
         {
         
            setResult(true);
            
         }
         else if(_optional->_result == false)
         {
            return false;
            
         } 

         return matched;
      }
   
      virtual Parser* copy() const override {
         return new Optional(*_optional);
      }

      virtual bool isOptional() const
      override
      {
         return true;
      }
     
   };

   inline bool Parser::isOptional() const {

      const Optional* optional =
         dynamic_cast<const Optional*>(this);
     
      if (optional == nullptr)
         return false;

      return true;
   }


}

#endif
