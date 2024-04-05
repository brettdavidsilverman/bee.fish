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

      Match* _json;
      bool _delete;
   public:
       
      JSONParser(Match& match) :
         Parser(match),
         _json((JSON*)&match)
      {
         _delete = false;
      }
      
      JSONParser(Match* match) :
         Parser(*match),
         _json((JSON*)match)
      {
          _delete = false;
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
         Parser(_json = new JSON())
      {
         _delete = true;
      }
      
      virtual ~JSONParser()
      {
         if (_delete)
            delete _json;
      }
      
      virtual void eof()
      override
      {
         if (_json->result() == nullopt) {
            _json->eof(this);
            _result = _json->result();
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
      /*
      void streamValue(const BString& key, BeeFishBString::BStream::OnBuffer onbuffer) {
         OnKey onkey =
            [onbuffer] (const BString& key, JSON& json) {
               json._string->_onbuffer = onbuffer;
            };
         _onkeys[key] = onkey; 

      }
*/
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
      
      virtual void onobjectkey(BString& key) {
      }

      virtual void onendobject(JSON* match) {
      }

      virtual void onbeginarray(JSON* match) {
      }

      virtual void onarrayvalue(JSON* match) {
      }

      virtual void onendarray(JSON* match) {
      }

      virtual void onobjectvalue(BString& key, const JSON* value) {
      }

      virtual void onvalue(JSON* json) {
         
      }
      
      virtual bool matched() const {
         return _json->result() == true;
      }
      

   };

   // Declared in json.h
   void JSON::success()
   {
      if (_parser->isJSONParser()) {
         jsonParser()->onvalue(this);
      }
      
      Match::success();
      
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
   /*
   inline void Object::matchedKey(const BString& key, LoadOnDemand<JSON>& value) {

      if (_parser->isJSONParser()) {
         //JSONParser* parser = (JSONParser*)_parser;
         JSON* json = (JSON*)(value._match);
         assert(json);
         jsonParser()->onobjectkey(key, json);
      }

   }
   */
   
   // Declared in object.h
   inline void Object::onobjectkey(ObjectKey* match) {

      if (_parser->isJSONParser()) {
         BString& key = match->value();
         jsonParser()->onobjectkey(key);
      }

   }

   // Declared in object.h
   inline void Object::onobjectvalue(ObjectKey* key, JSON* item) {

      //JSON* json = (JSON*)(item->_value->_match);
      //const BString& key = item->_key->value();

      if (_parser->isJSONParser()) {

         JSONParser* parser = (JSONParser*)(_parser);
         parser->onobjectvalue(key->value(), item);
      }
      
      //Set::matchedSetItem(item);

   }

   // Declared in array.h
   inline void Array::onarrayvalue(JSON* json)
   {
      ++_size;

      if (_parser->isJSONParser()) {
          jsonParser()->onarrayvalue(json);
      }
      
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
   
   istream& operator >> (istream& in, JSON& json)
   {
      JSONParser parser(json);
      
      parser.read(in);
      
      return in;
   }


}

#endif