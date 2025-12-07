#ifndef BEE_FISH_WEB__CONTENT_LENGTH_H
#define BEE_FISH_WEB__CONTENT_LENGTH_H

#include <map>
#include <vector>
#include <typeinfo>
#include "../parser/parser.h"
#include "../json/json-parser.h"

using namespace BeeFishParser;
        
namespace BeeFishWeb {

    class ContentLength : public Match
    {
    public:
        Size _contentCount = 0;
        Size _contentLength = 0;
    public:

        ContentLength() :
            _contentCount(0)
        {
        }

        virtual bool match(Byte b) {
            
            if (++_contentCount == _contentLength) {
                success();
            }
            
            return true;
        }
        
        virtual void eof(Parser* parser)
        override
        {
             if (_contentCount != _contentLength)
                  fail();
        }
        
        virtual void setup(Headers* headers)
        {
            if (headers->contains("content-length")) {
                     
                std::string contentLengthString = 
                    (*headers)["content-length"].str();
                          
                _contentLength =
                    atol(contentLengthString.c_str());
                    
                _parser->setDataBytes(this, _contentLength);
          
            }
            
            if (_contentLength == 0)
                success();
        }
        
        

    };
    

};

#endif
