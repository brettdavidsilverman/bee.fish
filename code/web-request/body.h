#ifndef BEE_FISH_WEB__BODY_H
#define BEE_FISH_WEB__BODY_H

#include <map>
#include <vector>
#include <typeinfo>
#include "../parser/parser.h"
#include "../json/json-parser.h"
#include "content-length.h"
#include "headers.h"

using namespace BeeFishParser;
      
namespace BeeFishWeb {

    class Body :
        public Match,
        public BeeFishBString::BStream
    {
    protected:
        BeeFishWeb::ContentLength* _contentLengthBody = nullptr;
        BeeFishJSON::JSON* _json = nullptr;
        bool _parseJSON;
    public:
        Body(bool parseJSON = true) : Match() {
            _parseJSON = parseJSON;
        }

        virtual void setup(Parser* parser, Headers*  headers) {
            if (_parser)
               return;
              
             if (!_parseJSON &&
                 headers->contains("content-length") ) {
                std::string contentLengthString = (*headers)["content-length"].str();
                size_t contentLength = atol(contentLengthString.c_str());
                if (contentLength > 0) {
                    _contentLengthBody = new ContentLength(contentLength);
                    parser->setDataBytes(contentLength);
                    _match = _contentLengthBody;
                }
            }
            else {
                _json = new BeeFishJSON::JSON();
                _match = _json;
            }

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
        
        virtual void eof(Parser* parser)
        override
        {
           Match::setup(parser);
           
           if (_match->result() == nullopt)
              _match->eof(parser);
        }


    };  
}

#endif