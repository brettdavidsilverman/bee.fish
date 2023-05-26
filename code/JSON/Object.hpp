#ifndef BEE_FISH__JSON__OBJECT_HPP
#define BEE_FISH__JSON__OBJECT_HPP

#include <map>
#include <memory>

#include "../Parser/Test.hpp"

#include "Number.hpp"
#include "String.hpp"
#include "BlankSpace.hpp"
#include "Null.hpp"

namespace BeeFishJSON {

   const BeeFishParser::And Object() {

      using namespace BeeFishParser;

      auto _Object =
         []() {
            return
               std::shared_ptr<Parser>(
                  Object().copy()
               );
         };

      const auto openBrace =
         Character("{");

      const auto closeBrace =
         Character("}");


      const auto key =
         String();

      const auto colon =
         Character(":");

      const auto value =
         Number() or
         String() or
         Null() or
         LoadOnDemand(_Object);

      const auto seperator =
          Character(",");

      const auto blankSpace =
         BlankSpace();

      const auto line =
         blankSpace and
         key and blankSpace and
         colon and blankSpace and 
         value and blankSpace;

      const auto object = 
         blankSpace and
         openBrace and blankSpace and
         Optional(
            Optional(
               line
            ) and blankSpace and
            Repeat(
               seperator and line,
               0
            )
         ) and blankSpace and
         closeBrace;

      return object;

   }
}

/*
#include "../power-encoding/power-encoding.h"
#include "../parser/parser.h"
#include "blank-space.h"
#include "string.h"
#include "keyed-set.h"

using namespace BeeFishParser;
using namespace BeeFishPowerEncoding;

namespace BeeFishJSON {
   class JSONParser;
}

namespace BeeFishJSON {
   
   class JSON;
   class Object; 

   class ObjectOpenBrace : public And {
   public:
      ObjectOpenBrace() : And(
         new Optional(new BlankSpace()),
         new BeeFishParser::Character('{'),
         new Optional(new BlankSpace())
      )
      {

      }

   };

   class ObjectCloseBrace : public And {
   public:
      ObjectCloseBrace() : And(
         new Optional(new BlankSpace()),
         new BeeFishParser::Character('}') 
      )
      {

      }
   };

   class ObjectFieldSeperator: public BeeFishParser::Character {
   public:
      ObjectFieldSeperator() : Character(',') {

      }
   };

   class ObjectKeyValueSeperator : public BeeFishParser::Character {

   public:
      ObjectKeyValueSeperator() : Character(':') {

      }
   };

   typedef String ObjectKey;
   typedef LoadOnDemand<JSON> ObjectValue;

   class Object:
      public KeyedSet<
         ObjectOpenBrace, 
         ObjectKey,
         ObjectKeyValueSeperator,
         ObjectValue,
         ObjectFieldSeperator,
         ObjectCloseBrace
      >
   {
   public:
      typedef std::function<void(const BString& key, JSON& value)> OnKeyValue;
      OnKeyValue _onkeyvalue = nullptr;
   public:

      Object() : KeyedSet()
      {
      }

      BeeFishJSON::JSONParser* jsonParser() {
         return (BeeFishJSON::JSONParser*)_parser;
      }

      // Defined in json-parser.h
      virtual void onbeginset(Match* match);

      // Defined in json-parser.h
      virtual void onendset(Match* match);

      // Defined in json-parser.h
      virtual void matchedKey(String& key, LoadOnDemand<JSON>& value);
      // Defined in json-parser.h
      virtual void matchedSetItem(_KeyValue* item);

      virtual void setOnKeyValue(OnKeyValue onkeyvalue) {
         _onkeyvalue = onkeyvalue;
      }
   };

}
*/
#endif