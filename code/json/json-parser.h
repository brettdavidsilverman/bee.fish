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
            _json(&match)
        {
            _delete = false;
        }
        
        JSONParser(Match* match) :
            Parser(*match),
            _json(match)
        {
             _delete = false;
        }
        
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

            if (_json->result() == nullopt)
            {
                _json->eof(this);
                
                if (_json->result() == true)
                    success();
                else
                    fail();
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
        
        virtual void onobjectkey(ObjectKey* key)
        {
        }
        
        virtual void onobjectvalue(Object* object, ObjectKey* key, JSON* value)
        {
            if (_onvalues.count(key->value()) > 0) {

                JSONParser::OnValue onvalue =
                    _onvalues[key->value()];
                     
                JSON* json = static_cast<JSON*>(value);
                
                if (!json->_parser)
                    json->setup(this);
                    
                onvalue(key->value(), *json);
            }
        }

        virtual void onendobject(JSON* match) {
        }

        virtual void onbeginarray(JSON* match) {
        }

        virtual void onarrayvalue(JSON* match) {
        }

        virtual void onendarray(JSON* match) {
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
    inline void Object::onbeginset(Match* json) {
        if (_parser->isJSONParser()) {
            jsonParser()->onbeginobject((JSON*)json);
        }
    }

    // Declared in object.h
    inline void Object::onendset(Match* json) {
        if (_parser->isJSONParser()) {
            jsonParser()->onendobject((JSON*)json);
        }
    }
    
    inline void Object::onsetupkeyvalue(ObjectKey* key, ObjectValue* value)
    {

        if (_parser->isJSONParser()) {
            JSONParser* parser = (JSONParser*)_parser;
            if (parser->_onkeys.count(key->value()) > 0) {
                JSONParser::OnKey _onkey = parser->_onkeys[key->value()];
                value->setup(parser);
                if (value->_match)
                {
                     JSON* json = static_cast<JSON*>(value->_match);
                     if (!json->_parser)
                         json->setup(parser);
                    _onkey(key->value(), *json);
                }
            }
        }
    }
        

    // Declared in object.h
    inline void Object::onobjectvalue(ObjectKey* key, ObjectValue* value)
    {
        if (_parser->isJSONParser()) {
            jsonParser()->onobjectvalue(this, key, (JSON*)(value->_match));
        }

    }
    
    
    // Declared in object.h
    inline void Object::onobjectkey(ObjectKey* key) 
    {

        if (_parser->isJSONParser()) {
            jsonParser()->onobjectkey(key);
        }

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