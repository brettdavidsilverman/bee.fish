#ifndef BEE_FISH_PARSER__NOT_HPP
#define BEE_FISH_PARSER__NOT_HPP

#include <memory>
#include "Character.hpp"

namespace BeeFishParser {

   class Not : public Parser {
   protected:
      Parser* _not;

   public:
      using Parser::read;

      Not(const Parser& value) :
         _not(value.copy())
      {
      }

      Not(const Not& source) :
         _not(source._not->copy())
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

         if (_result != nullopt)
         {
            //return false;
            return _result == true;

         }

         bool matched = true;

         if (_not->_result == nullopt)
         {
            matched = _not->read(character);
         }

         if (_not->_result == false)
         {
            setResult(true);
            return true;
         }
         else if (_not->_result == true) 
         {
            setResult(false);
            return false;
         }
         else if ( _not->_result == nullopt)
         {
            return true;
         }

         

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

      virtual bool isOptional() const
      override
      {
         return _not->_result == false;
      }

      virtual Parser* copy()
      const override
      {
         return new Not(*this);
      }


   };
   
};

#endif
