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
        BeeFishWeb::ContentLength* _contentLengthBody = nullptr;
        JSON* _json = nullptr;
    public:
        Body(bool parseJSON = true) : Match() {
            _parseJSON = parseJSON;
            
        }

        virtual void setup(Parser* parser, Headers*  headers) {
            
            Match* content = nullptr;
            
            if (!_parseJSON &&
                headers->contains("content-length") )
            {

                std::string contentLengthString = (*headers)["content-length"].str();
                size_t contentLength = atol(contentLengthString.c_str());
                if (contentLength > 0) {
                    _contentLengthBody = new ContentLength(contentLength);
                    parser->setDataBytes(contentLength);
                    content = _contentLengthBody;
                }
            }
            else {
                _json = new BeeFishJSON::JSON();
                
                BeeFishParser::And* _and =
                    new BeeFishParser::And(
                        _json,
                        new Optional(
                            new NewLine()
                        )
                    );
                    
                content = _and;
            }
            
            
            _match = content;
            
            Match::setup(parser);

        }
        
        
        virtual ~Body() {
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