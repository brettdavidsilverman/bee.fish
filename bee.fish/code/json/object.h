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
         new BlankSpace(0),
         new BeeFishParser::Character('{'),
         new BlankSpace(0)
      )
      {

      }

   };

   class ObjectCloseBrace : public And {
   public:
      ObjectCloseBrace() : And(
         new BlankSpace(0),
         new Character('}') 
      )
      {

      }
   };

   class ObjectFieldSeperator: public And
   {
   public:
      ObjectFieldSeperator() : And(
         new BlankSpace(0),
         new Character(','),
         new BlankSpace(0)
      )
      {

      }
   };

   class ObjectKeyValueSeperator : public And {

   public:
      ObjectKeyValueSeperator() : And(
         new BlankSpace(0),
         new Character(':'),
         new BlankSpace(0)
      )
      {

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
      virtual void onobjectkey(ObjectKey* key);
      
      // Defined in json-parser.h
      virtual void onobjectvalue(ObjectKey* key, JSON* value);
      
      virtual void onkey(ObjectKey* key)
      override
      {
          onobjectkey(key);
      }
      
      virtual void onkeyvalue(ObjectKey* key, ObjectValue* value)
      override
      {
          JSON* json = (JSON*)(value->_match);
          
          onobjectvalue(key, json);
      }

   };

}

#endif