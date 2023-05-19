#ifndef BEE_FISH_PARSER__PARSER_BASE_HPP
#define BEE_FISH_PARSER__PARSER_BASE_HPP

#include <iostream>
#include <string>
#include <bitset>

#include "../Miscellaneous/Miscellaneous.hpp"

namespace BeeFishParser {

   typedef std::string String;
   typedef BeeFishMisc::optional<bool> Optional;

   class UTF8Character;

   class ParserBase {
   public:
      Optional _result = NullOpt;

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
      
      virtual Optional result() const
      {
         return _result;
      }

      virtual void setResult(Optional result) {

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
      static const String& EmptyString() {
         static const String _EmptyString = {};
         return _EmptyString;
      }

      static unsigned long& parserInstanceCount() {
         static unsigned long _parserInstanceCount = 0;
         return _parserInstanceCount;
      }


      

   };
}

#endif
