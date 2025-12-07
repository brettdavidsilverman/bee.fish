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

#include "headers.h"
#include "content-length.h"
#include "body.h"


namespace BeeFishWeb {
    using namespace BeeFishParser;
    using namespace BeeFishPowerEncoding;
        
    class WebRequest : public Match {
    public:

        class URL : public Match
        {
        public:
/*
            class Hex : public Capture {
            public:
                Hex() : Capture(
                    new BeeFishParser::Or (
                        new Range('a', 'f'),
                        new Range('A', 'F'),
                        new Range('0', '9')
                    )
                )
                {

                }
                
            };

            class HexCharacter : public BeeFishParser::And {
            protected:
                Hex* _hex1 = nullptr;
                Hex* _hex2 = nullptr;
                Char _character;
            public:
                HexCharacter() : BeeFishParser::And()
                {
                    _inputs = {
                        new BeeFishParser::Character('%'),
                        _hex1 = new Hex(),
                        _hex2 = new Hex()
                    };
                }

                virtual void success() {

                    std::stringstream stream;
                    stream << std::hex;
                    
                    stream << _hex1->value();
                    stream << _hex2->value();

                    uint32_t u32;
                    stream >> u32;
                    _character = Char(u32);
                    
                    Match::success();
                    
                }

                virtual const Char& character() const
                override
                {
                    return _character;
                }

            };
*/
            class PathCharacter : public BeeFishParser::Not {
            public:
                PathCharacter() : Not(
                    new BeeFishParser::Or(
                        new BeeFishParser::Character(' '),
                        new BeeFishParser::Character('?')
                    )
                    /*
                    
                    new Range('0', '9'),
                    new Range('a', 'z'),
                    new Range('A', 'Z'),
                    new BeeFishParser::Character('+'),
                    new BeeFishParser::Character('.'),
                    new BeeFishParser::Character('='),
                  //  new BeeFishParser::Character('?'),
                    new BeeFishParser::Character('&'),
                    new BeeFishParser::Character('-'),
                    new BeeFishParser::Character('_'),
                    new BeeFishParser::Character('['),
                    new BeeFishParser::Character(']'),
                    new BeeFishParser::Character('/'),
                    new HexCharacter()
                    */
                )
                {

                }
                /*
                virtual const Char& character() const
                override
                {
                    return item().character();
                }
*/

            };


            class Path : public Repeat<PathCharacter> {
            protected:
                BString _value;

            public:
                Path() : Repeat() {

                }

                virtual void matchedItem(PathCharacter* item) {
                    _value.push_back(item->character());
                    Repeat<PathCharacter>::matchedItem(item);
                }

                virtual const BString& value() const {
                    return _value;
                }
            };

            class Query;
            
            class KeyCharacter : public BeeFishParser::Not {
            public:
                KeyCharacter() : Not(
                    new BeeFishParser::Or(
                        new BeeFishParser::Character('='),
                        new BeeFishParser::Character('&'),
                        new BeeFishParser::Character(' ')
                    )
                )
                {

                }
            };
            
            class Key : public Repeat<KeyCharacter>
            {
            public:
                Key() : Repeat<KeyCharacter>()
                {
                }
            };
            
            class ValueCharacter : public BeeFishParser::Not {
            public:
                ValueCharacter() : Not(
                    new BeeFishParser::Or(
                        new BeeFishParser::Character('&'),
                        new BeeFishParser::Character(' ')
                    )
                )
                {

                }
            };
            
            class Value : public Repeat<ValueCharacter>
            {
            public:
                Value() : Repeat<ValueCharacter>()
                {
                }
            };
            
            

            class KeyValuePair : public BeeFishParser::And {
            public:
                friend class Query;

            protected:
                BString _key;
                BString _value;

            public:
                KeyValuePair() : And ()
                {
                    _inputs = {
                        new Capture(
                            new Key(),
                            _key
                        ),
                        new Optional(
                            new And(
                                new BeeFishParser::Character('='),
                                new Capture(
                                    new Value(),
                                   _value
                                )
                            )
                        ),
                        new Optional(
                            new BeeFishParser::Character('&')
                        )
                    };

                }
                

            };

            class Query : 
                public Repeat<KeyValuePair>,
                public std::map<BString, BString>
            {
            protected:
                BString _value;
            public:
                Query() : Repeat<KeyValuePair>() { 
                    
                }

                virtual void matchedItem(KeyValuePair* item) {
                    BString key = item->_key.decodeURI();
                    BString value = item->_value.decodeURI();
                    (*this)[key] = value;
                    Repeat<KeyValuePair>::matchedItem(item);
                }
                
                virtual bool  matchCharacter(const Char& character) {
                    if (Repeat<KeyValuePair>::matchCharacter(character)) {
                        _value.push_back(character);
                        return true;
                    }
                    return false;
                }

                virtual const BString& value() const {
                    return _value;
                }

                virtual bool contains(const BString& test) {
                    return count(test) > 0;
                }
                
                virtual void success() 
                override
                {
                    cerr << "{" << _value << "}";
                    Repeat<KeyValuePair>::success();
                }
                
            };         

        public:
            Path* _path = nullptr;
            Query* _query = nullptr;
        public:
            
            URL() : Match()
            {
              
                
                _path = new Path();
                    
                    
                Match* query =
                    new Optional(
                        new BeeFishParser::And(
                            new BeeFishParser::
                                Character('?'),
                            _query = new Query()
                        )
                    );
                    
                _match = new BeeFishParser::And(_path, query);

            }
            
            const BString& path() const
            {
                return _path->value();
            }
            
            const Query& query() const
            {
                return (*_query);
            }
            
            Query& query()
            {
                return (*_query);
            }
            
            
        };
 
        class FirstLine : public Match
        {
        protected:
            class Method : public BeeFishParser::Or{
            public:
                Method() : BeeFishParser::Or(
                    new Word("GET"),
//                    new Word("PUT"),
                    new Word("POST"),
                    new Word("DELETE")
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
            _body      = new Body( _parseJSON);
            
            
            auto onheaders =
            [this](Match* match) 
            {

                if (method() == "POST") {
                    _body->setup(_headers);
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
            _body->setOnBuffer(ondata);
        }

        
        
        virtual void flush()
        {
            if (_body)
               _body->flush();
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
        
        const URL::Query& queryObject() const
        {
            return url().query();
        }

        URL::Query& queryObject()
        {
            return url().query();
        }

        const BString& query() const {
            return url().query().value();
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