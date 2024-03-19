#ifndef BEE_FISH_JSON__OBJECT_H
#define BEE_FISH_JSON__OBJECT_H

#include <map>
#include <memory>

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

#endif