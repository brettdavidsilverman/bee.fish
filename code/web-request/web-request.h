#ifndef BEE_FISH_WEB__WEB_REQUEST_H
#define BEE_FISH_WEB__WEB_REQUEST_H

#include <map>
#include <vector>
#include <typeinfo>
#include "../parser/parser.h"
#include "../json/json-parser.h"
#include "../json/json.h"
#include "../Config.hpp"

#ifdef SERVER
#include "../Database/Path.hpp"
#endif

#include "url.h"
#include "headers.h"
#include "body.h"


namespace BeeFishWeb {
    
using namespace BeeFishParser;
using namespace BeeFishPowerEncoding;

class WebRequest : public Match 
{
public:



    class FirstLine : public Match
    {
    protected:
        class Method : public BeeFishParser::Or {
        public:
            Method() : BeeFishParser::Or(
                    new Word("GET"),
                    new Word("POST"),
                    new Word("DELETE"),
                    new Word("OPTIONS")
                )
            {

            }

        };

        class Version : public BeeFishParser::And {
        public:
            Version() : And(
                    new Word("HTTP/1."),
                    new Range('0', '9')
                )
            {

            }

        };

    public:
        BString _method;
        URL*     _url = nullptr;
        BString _version;
        NewLine* _newLine;
    private:
        BString _fullURL;
    public:
        FirstLine() : Match()
        {
        }

        virtual ~FirstLine()
        {
        }

        virtual void setup(Parser* parser)
        override
        {
            if (_parser)
                return;

            _match = new BeeFishParser::And(
                new Invoke(
                    new Capture(
                        new Method(),
                        _method
                    ),
            [this](Match* match) {
                _method = _method.toUpper();
                return true;
            }
                ),
            new Blankspaces(1),
            new Capture(
                _url = new URL(),
                _fullURL
            ),
            new Blankspaces(1),
            new Capture(
                new Version(),
                _version
            ),
            new Optional(
                _newLine = new NewLine()
            )
            );

            Match::setup(parser);

        }

        const URL& url() const {
            return *_url;
        }

        URL& url() {
            return *_url;
        }

        const BString fullURL() const {
            return _fullURL;
        }

    };

    FirstLine*           _firstLine = nullptr;
    BeeFishWeb::Headers* _headers   = nullptr;
    Body*                _body      = nullptr;
    Optional*            _optionalBody = nullptr;
    BStream::OnBuffer    _ondata    = nullptr;
    bool                 _parseJSON = true;
public:

    WebRequest(bool parseJSON = true) :
        Match(),
        _parseJSON(parseJSON)

    {

        _firstLine = new FirstLine();
        _headers   = new Headers();
        _body      = new Body(_parseJSON);


        auto onheaders =
            [this](Match* match)
        {

            if (method() == "GET")
                success();
            else {
                Size contentLength = 0;
                
                if (!_parseJSON) {
                    contentLength =
                        getContentLength();

                    if (contentLength == 0) {
                        success();
                        return true;
                    }
                 
                }
                
                _body->setup(
                    contentLength,
                    _ondata
                );
            }

            return true;
        };


        _match = new BeeFishParser::And(
            _firstLine,
            new Optional(
                new NewLine()
            ),
            new Optional(
                new Invoke(
                    _headers,
                    onheaders
                )
            ),
            new Optional(
                new NewLine()
            ),
            _optionalBody = new Optional(
                _body
            )
        );




    }

    virtual ~WebRequest()
    {
    }



    virtual void setOnData(BStream::OnBuffer ondata)
    {
        _ondata = ondata;

    }

    virtual void flush()
    {
        _body->flush();
    }

    virtual Size getContentLength()
    {
        Size contentLength = 0;

        if (_headers->contains("content-length"))
        {

            BString contentLengthString =
                (*_headers)["content-length"];

            contentLength =
                atol(contentLengthString.c_str());
        }

        return contentLength;

    }




    virtual bool hasJSON()
    {
        return _optionalBody->_matched && _body->hasJSON();
    }

    virtual BeeFishJSON::JSON& json()
    {
        return *(_body->json());
    }

    Headers& headers()
    {
        return *_headers;
    }

    const Headers& headers() const
    {
        return *_headers;
    }

    const BString& method() const
    {
        return _firstLine->_method;
    }

    const FirstLine& firstLine() const
    {
        return *_firstLine;
    }

    FirstLine& firstLine()
    {
        return *_firstLine;
    }

    const URL& url() const
    {
        return _firstLine->url();
    }

    URL& url()
    {
        return _firstLine->url();
    }

    BString fullURL() const
    {
        return _firstLine->fullURL();
    }

    BString path() const
    {
        return url().path();
    }

    const URL::Search& searchObject() const
    {
        return url().search();
    }

    URL::Search& searchObject()
    {
        return url().search();
    }

    const BString& search() const {
        return url().search().value();
    }

    const BString& version() const
    {
        return _firstLine->_version;
    }

    BString getCookie(
        const BString& cookieName
    ) const
    {
        const Headers& headers =
            this->headers();

        if (headers.contains("cookie"))
        {
            const BString& cookieHeader = headers["cookie"];
            vector<BString> cookies = cookieHeader.split(';');
            for (BString cookie : cookies)
            {
                vector<BString> pair =
                    cookie.trim().split('=');

                if ( pair.size() == 2 &&
                        pair[0].trim() == cookieName )
                {
                    return (pair[1].trim());

                }
            }
        }

        return "";
    }

};


};

#endif