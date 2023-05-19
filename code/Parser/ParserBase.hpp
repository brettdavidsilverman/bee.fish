#ifndef BEE_FISH_PARSER__PARSER_BASE_HPP
#define BEE_FISH_PARSER__PARSER_BASE_HPP

#include <iostream>
#include <string>
#include <bitset>
#include "../Miscellaneous/optional.h"

namespace BeeFishParser {

   class UTF8Character;

   class ParserBase {
   public:
      std::optional<bool> _result = std::nullopt;

   public:
   
      ParserBase()
      {
         ++parserInstanceCount();
      }

      ParserBase(const ParserBase& parser)
      {
         ++parserInstanceCount();
      }
      
      virtual ~ParserBase()
      {
         --parserInstanceCount();
      }

      bool parsed() const
      {
         return (result() == true);
      }
      
      virtual std::optional<bool> result() const
      {
         return _result;
      }

      virtual void setResult(std::optional<bool> result) {

         if (_result == result) 
            return;

         _result = result;

         if (_result == true)
            success();
         else if (_result == false)
            fail();

      }

      virtual void success()
      {
      }
   
      virtual void fail()
      {
      }

      static const std::string& EmptyString() {
         static const std::string _EmptyString = {};
         return _EmptyString;
      }

      static unsigned long& parserInstanceCount() {
         static unsigned long _parserInstanceCount = 0;
         return _parserInstanceCount;
      }


      

   };
}

#endif
