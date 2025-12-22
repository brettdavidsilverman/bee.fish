#ifndef BEE_FISH_WEB__BODY_H
#define BEE_FISH_WEB__BODY_H

#include <map>
#include <vector>
#include <typeinfo>
#include "../parser/parser.h"
#include "../json/json-parser.h"
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
    JSON* _json = nullptr;
    Headers* _headers = nullptr;
    Size _contentCount = 0;
    Size _contentLength = 0;
public:
    Body(bool parseJSON) {
        _parseJSON = parseJSON;
        if (_parseJSON)
        {
            _json = new BeeFishJSON::JSON();
            _match = _json;
        }
    }

    virtual ~Body() {
    }


    virtual void setup(
        Size contentLength, 
        BStream::OnBuffer ondata
    )
    {
        _contentLength = contentLength;
        BStream::setOnBuffer(ondata);
        if (!_parseJSON)
            _parser->setDataBytes(this, _contentLength);

    }


    virtual bool hasJSON() const {
        return _json && _json->matched();
    }

    BeeFishJSON::JSON* json()
    {
        return _json;
    }

    virtual void eof(Parser* parser)
    override
    {
        if (_parseJSON)
            Match::eof(parser);
        else if (!_parseJSON && 
                  _contentCount != _contentLength)
            Match::fail();
    }

    virtual bool match(Byte b)
    override
    {

        if (_contentCount >= _contentLength)
            return false;
            
        if (++_contentCount == _contentLength) {
            success();
        }

        BStream::push_back(b);
        
        return true;
    }

    virtual bool match(
        Parser* parser,
        const Char& character
    )
    override
    {
        if (Match::match(parser, character))
        {
            for (const char c : character)
            {
                BStream::push_back((Byte)c);
            }
            return true;
        }
        
        return false;
    }



};
}

#endif