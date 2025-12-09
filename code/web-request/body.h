#ifndef BEE_FISH_WEB__BODY_H
#define BEE_FISH_WEB__BODY_H

#include <map>
#include <vector>
#include <typeinfo>
#include "../parser/parser.h"
#include "../json/json-parser.h"
#include "content-length.h"
#include "headers.h"


namespace BeeFishWeb {

    using namespace BeeFishJSON;
    using namespace BeeFishParser;
      
    class Body :
        public Match,
        public BeeFishBString::BStream
    {
    protected:
        bool _parseJSON;
        Size _contentLength = 0;
        BeeFishWeb::ContentLength* _contentLengthBody = nullptr;
        JSON* _json = nullptr;
        Headers* _headers = nullptr;
        
    public:
        Body(bool parseJSON) {
            _parseJSON = parseJSON;
            if (_parseJSON)
            {
                _json = new BeeFishJSON::JSON();
                _match = _json;
            }
            else
            {
                _contentLengthBody =
                    new ContentLength();
                _match = _contentLengthBody;
            }
        }

        virtual ~Body() {
        }
        
        virtual void setup(Headers* headers)
        {
    
            if (_contentLengthBody)
            {
                _contentLengthBody->setup(
                    headers
                );
            }
            
        }
        
        virtual bool hasJSON() const {
            return _json && _json->matched();            
        }
        
        BeeFishJSON::JSON* json()
        {
            return _json;
        }
        
        virtual bool matchCharacter(const Char& character) {

            if (Match::matchCharacter(character)) {
                for (auto c : character)
                   BeeFishBString::BStream::push_back((Byte)c);
                return true;
            }

            return false;
        }

        

    };  
}

#endif