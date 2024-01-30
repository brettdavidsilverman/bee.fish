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

      Optional() :
        _optional(nullptr)
      {
      }

      Optional(const Parser& optional) :
         _optional(optional.copy())
      {
      }

      virtual ~Optional() {
         if (_optional)
            delete _optional;
      }

      virtual bool read(
         char c
      ) override
      {
         
         if (_optional == nullptr) {
            setResult(true);
            return false;
         }

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
         if (_optional)
            return new Optional(*_optional);
         return new Optional();
      }

      virtual bool isOptional() const
      override
      {
         return true;
      }
     
   };


}

#endif
