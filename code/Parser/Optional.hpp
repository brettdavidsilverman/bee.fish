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

      virtual bool read(
         bool bit
      ) override
      {
         throw std::logic_error("Parser::read(bool) Should not reach here 🚫");
      }

      virtual bool read(
         char c
      ) override
      {
         
         bool matched =
            _optional->read(c);
         
         bool succeeded = false;
         
         if (_optional->_result == true)
         {
         
            _matched = true;
            succeeded = true;
            
         } 
         else if (_optional->_result == false)
         {
            _matched = false;
            //succeeded = true;
         }

         if (succeeded)
            setResult(true);

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
