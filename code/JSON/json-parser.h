#ifndef BEE_FISH__JSON_PARSER_H
#define BEE_FISH__JSON_PARSER_H

#include "../parser/parser.h"
#include "json.h"

namespace BeeFishJSON
{
   
   using namespace BeeFishBString;

   class JSONParser : public Parser
   {
   public:

      typedef std::function<void(const BString& key, JSON& json)> OnKey;
      typedef std::map<const BString, OnKey> OnKeys;
      typedef std::function<void(const BString& key, JSON& json)> OnValue;
      typedef std::map<const BString, OnValue> OnValues;

      OnKeys _onkeys = {};
      OnValues _onvalues {};

      public:
      JSONParser(Match& match) :
         Parser(match)
      {
      }
      
      virtual ~JSONParser()
      {
      }
      
      void captureValue(const BString& key, BeeFishMisc::optional<BString>& value) {
         _onvalues[key] = 
            [&value] (const BString& key, JSON& json) {
               if (json.isNull())
                  value = BeeFishMisc::nullopt;
               else
                  value = json.value();
            };
      }
      
      void streamValue(const BString& key, BeeFishBString::BStream::OnBuffer onbuffer) {
         OnKey onkey =
            [onbuffer] (const BString& key, JSON& json) {
               json._string->_onbuffer = onbuffer;
            };
         _onkeys[key] = onkey; 

      }

      void invokeValue(const BString& key, BeeFishJSON::JSONParser::OnValue onvalue) {
         _onvalues[key] = onvalue;
      }

      void clear() {
         _onkeys.clear();;
         _onvalues.clear();
      }

      virtual bool isJSONParser() {
         return true;
      }

      virtual void onbeginobject(Match* match) {
      }

      virtual void onendobject(Match* match) {
      }

      virtual void onbeginarray(Match* match) {
      }

      virtual void onarrayvalue(Match* match) {
      }

      virtual void onendarray(Match* match) {
      }

      virtual void onobjectvalue(const BString& key, const JSON* value) {
      }

      virtual void onvalue(JSON* json) {
      }

   };

   // Declared in json.h
   inline void JSON::success() {
      if (_parser->isJSONParser()) {
         jsonParser()->onvalue(this);
      }
      And::success();
   }

   // Declared in object.h
   inline void Object::onbeginset(Match* match) {
      if (_parser->isJSONParser()) {
         jsonParser()->onbeginobject(match);
      }
   }

   // Declared in object.h
   inline void Object::onendset(Match* match) {
      if (_parser->isJSONParser()) {
         jsonParser()->onendobject(match);
      }
   }

   // Declared in object.h
   inline void Object::matchedKey(String& key, LoadOnDemand<JSON>& value) {

      if (_parser->isJSONParser()) {
         JSONParser* parser = (JSONParser*)_parser;
         if (parser->_onkeys.count(key.value()) > 0) {
            JSONParser::OnKey onkey = parser->_onkeys[key.value()];
            JSON* json = static_cast<JSON*>(value._match);
            if (!json->_setup)
               json->setup(parser);
            onkey(key.value(), *json);
         }
      }

   } 

   // Declared in object.h
   inline void Object::matchedSetItem(Object::_KeyValue* item) {
      
      JSON* json = static_cast<JSON*>(item->_value->_match);
      const BString& key = item->_key->value();

      if (_parser->isJSONParser()) {

         JSONParser* parser = (JSONParser*)(_parser);

         if (parser->_onvalues.count(key) > 0) {
            JSONParser::OnValue onvalue = parser->_onvalues[key];
            if (!item->_value->_setup)
               item->_value->setup(parser);
            onvalue(key, *json);
         }

         LoadOnDemand<BeeFishJSON::JSON>* value = item->_value;
         const BeeFishJSON::JSON* json = (const BeeFishJSON::JSON*)(value->_match);

         parser->onobjectvalue(key, json);
      }
      
      if (_onkeyvalue) {
         _onkeyvalue(key, *json);
      }

      Set::matchedSetItem(item);

   }

   // Declared in array.h
   inline void Array::matchedSetItem(LoadOnDemand<JSON>* item)
   {
      ++_size;

      if (_parser->isJSONParser()) {
          jsonParser()->onarrayvalue(item->_match);
      }
      
      Set::matchedSetItem(item);
   }

   // Declared in array.h
   inline void Array::onbeginset(Match* match) {
      if (_parser->isJSONParser()) {
          jsonParser()->onbeginarray(match);
      }
      Set::onbeginset(match);
   }

   // Declared in array.h
   inline void Array::onendset(Match* match) {
      if (_parser->isJSONParser()) {
          jsonParser()->onendarray(match);
      }
      Set::onendset(match);
   }


}

#endif