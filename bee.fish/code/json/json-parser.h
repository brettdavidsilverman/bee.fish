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
       
      class _JSON : public JSON
      {
      public:
          _JSON() {
          }
          
      public:
         virtual void onvalue(Type type, BString& value)
         {
             cerr << endl
                  << "JSONPARSER:onvalue "
                  << type
                  << ":"
                  << value
                  << endl;
         }
      };

      typedef std::function<void(const BString& key, JSON& json)> OnKey;
      typedef std::map<const BString, OnKey> OnKeys;
      typedef std::function<void(const BString& key, JSON& json)> OnValue;
      typedef std::map<const BString, OnValue> OnValues;

      OnKeys _onkeys = {};
      OnValues _onvalues {};

      JSON* _json;
      
   public:
       
      JSONParser(Match& match) :
         Parser(match),
         _json((JSON*)&match)
      {
      }
      /*
      JSONParser(JSON* json) :
         Parser(*json),
         _json(json),
         _delete(true)
      {
      }
      */
      
      JSONParser() :
         Parser(new _JSON())
      {
          _json = (_JSON*)&_match;
      }
      
      virtual ~JSONParser()
      {
      }
      
      virtual void eof() {
         
         if (result() == nullopt) {
            _json->eof(this);
         }
      }
      
      void captureValue(const BString& key, optional<BString>& value) {
         _onvalues[key] = 
            [&value] (const BString& key, JSON& json) {
               if (json.isNull())
                  value = nullopt;
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

      virtual void onbeginobject(JSON* match) {
      }
      
      virtual void onobjectkey(const BString& key) {
      }
      
      virtual void onobjectkey(const BString& key, JSON* json) {
      }
      

      virtual void onendobject(JSON* match) {
      }

      virtual void onbeginarray(JSON* match) {
      }

      virtual void onarrayvalue(JSON* match) {
      }

      virtual void onendarray(JSON* match) {
      }

      virtual void onobjectvalue(const BString& key, const JSON* value) {
      }

      virtual void onvalue(JSON* json) {
      }
      

   };

   // Declared in json.h
   void JSON::success()
   {
      if (_parser->isJSONParser()) {
        // jsonParser()->onvalue(this);
      }
      And::success();
   }

   // Declared in object.h
   inline void Object::onbeginset(Match* match) {
      if (_parser->isJSONParser()) {
         JSON* json =
             (JSON*)(match);
         jsonParser()->onbeginobject(json);
      }
   }

   // Declared in object.h
   inline void Object::onendset(Match* match) {
      if (_parser->isJSONParser()) {
         JSON* json =
             (JSON*)(match);
         jsonParser()->onendobject(json);
      }
   }

   // Declared in object.h
   inline void Object::matchedKey(const BString& key, LoadOnDemand<JSON>& value) {

      if (_parser->isJSONParser()) {
         //JSONParser* parser = (JSONParser*)_parser;
         JSON* json = (JSON*)(value._match);
         assert(json);
         jsonParser()->onobjectkey(key, json);
      }

   }
   
   // Declared in object.h
   inline void Object::matchedKey(const BString& key) {

      if (_parser->isJSONParser()) {
         jsonParser()->onobjectkey(key);
      }

   }

   // Declared in object.h
   inline void Object::matchedSetItem(Object::_KeyValue* item) {

      //JSON* json = (JSON*)(item->_value->_match);
      const BString& key = item->_key->value();

      if (_parser->isJSONParser()) {

         JSONParser* parser = (JSONParser*)(_parser);

         LoadOnDemand<BeeFishJSON::JSON>* value = item->_value;
         const BeeFishJSON::JSON* json = (const BeeFishJSON::JSON*)(value->_match);

         parser->onobjectvalue(key, json);
      }
      
      Set::matchedSetItem(item);

   }

   // Declared in array.h
   inline void Array::matchedSetItem(LoadOnDemand<JSON>* item)
   {
      ++_size;

      if (_parser->isJSONParser()) {
          JSON* json =
             (JSON*)(item->_match);
          jsonParser()->onarrayvalue(json);
      }
      
      Set::matchedSetItem(item);
   }

   // Declared in array.h
   inline void Array::onbeginset(Match* match) {
      if (_parser->isJSONParser()) {
          jsonParser()->onbeginarray((JSON*)match);
      }
      Set::onbeginset(match);
   }

   // Declared in array.h
   inline void Array::onendset(Match* match) {
      if (_parser->isJSONParser()) {
          jsonParser()->onendarray((JSON*)match);
      }
      Set::onendset(match);
   }


}

#endif