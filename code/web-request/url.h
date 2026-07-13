#ifndef BEE_FISH_WEB__URL_H
#define BEE_FISH_WEB__URL_H

#include <map>
#include <vector>
#include <typeinfo>
#include "../parser/parser.h"

namespace BeeFishWeb {

    using namespace BeeFishParser;
    
    class URL : public Match
    {
    public:
        
        
        class Protocol : public Capture {
        public:
            Protocol() : Capture(
                new OrderOfPrecedence(
                {
                    {
                        new OrderOfPrecedence::Item(
                            new CIWord("https")
                        )
                    },
                    {
                        new OrderOfPrecedence::Item(
                            new CIWord("http")
                        )
                    }
                }
                )
            )
            {
            }
        };
        
        class DomainCharacter : public Not {
        public:
            DomainCharacter() : Not(
                new Or(
                    new Character(' '),
                    new Character('/'),
                    new Character('?'),
                    new Character(':')
                )
            )
            {
            }
            
        };
        
        class Domain : public Capture {
        public:
            Domain() : Capture(
                new Repeat<DomainCharacter>()
            )
            {
            }
        };
        
        class PortCharacter : public Range
        {
        public:
            PortCharacter() : Range('0', '9')
            {
            }
        };
        
        class Port : public Capture
        {
        public:
            Port() : Capture(
                new Repeat<PortCharacter>()
            )
            {
            }
        };
        

        class Origin : public Capture
        {
        public:
            Protocol* _protocol = new Protocol();
            Domain*   _domain   = new Domain();
            Port*     _port     = new Port();
            
        public:
            Origin() : Capture(
            )
            {
                _match = new And(
                    new Optional(
                        new And(
                            _protocol,
                            new Word("://")
                        )
                    ),
                    _domain,
                    new Optional(
                        new And(
                            new Character(':'),
                           _port
                        )
                    )
                );
            }
        };
                    
        class PathCharacter : public Not {
        public:
            PathCharacter() : Not(
                new Or(
                    new Character(' '),
                    new Character('?')
                )
            )
            {
            }
            
        };

        class Path : public Capture {
                
        public:
            Path() : Capture(
                new  Repeat<PathCharacter>()
            )

            {
            }
            
        };

        class KeyCharacter : public Not {
        public:
            KeyCharacter() : Not(
                new Or(
                    new Character('='),
                    new Character('&'),
                    new Character(' ')
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
            
        class ValueCharacter : public Not {
        public:
            ValueCharacter() : Not(
                new Or(
                    new Character('&'),
                    new Character(' ')
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
            
        class KeyValues;
        
        class KeyValuePair : public And {
        public:
            friend class KeyValues;

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
                        new Character('=')
                    ),
                    new Optional(
                        new Capture(
                            new Value(),
                            _value
                        )
                    ),
                    new Optional(
                        new Character('&')
                    )
                };

            }
                

        };
        
        class KeyValues :
            public Repeat<KeyValuePair>,
            public std::map<BString, BString>
        {
        public:
            KeyValues() : Repeat<KeyValuePair>()
            {
            }
                
            virtual void matchedItem(KeyValuePair* item) {
                BString key = item->_key.decodeURI();
                BString value = item->_value.decodeURI();
                (*this)[key] = value;
                Repeat<KeyValuePair>::matchedItem(item);
            }
        };

        class Search : 
            public Capture
        {
        protected:
            KeyValues* _keyValues;
                
            
        public:
            Search() : Capture(
                new And(
                    new Character('?'),
                    _keyValues =
                        new KeyValues()
                )
            )
            {
                assert(_keyValues);
            }
            
            virtual void success()
            override
            {
                _value = _value.decodeURI();
                Capture::success();
            }
            
            virtual operator const BString& () const
            {
                return _value;
            }
            
            KeyValues& items() const
            {
                assert(_keyValues);
                return *_keyValues;
            }
            
            virtual bool contains(const BString& key) {
                assert(_keyValues);
//cerr << "CONTAINS KEY " << key << ":" << _keyValues << ":" << typeid(*_keyValues).name() << endl;
                return _keyValues->count(key) > 0;
            }
            
            virtual BString& operator [] (const BString& key)
            {
                assert(_keyValues);
                return (*_keyValues)[key];
            }
        
            
        
        };         

    public:
        class URLException : public std::runtime_error
        {
        public:
            URLException(const BString& message) :
                std::runtime_error(message)
            {
            }
        };
        
    public:
        Origin* _origin = new Origin();
        Path*   _path   = new Path();
        Search* _search = new Search();
        
        const URL* _baseURL = nullptr;

    public:
            
        URL() : Match()
        {
            _match = new And(
                new Optional(
                    _origin
                ),
                new Optional(
                    _path
                ),
                new Optional(
                    _search
                )
            );
        }
        
        
        URL(const BString& input) : URL() {
            Parser parser(*this);
            parser.read(input);
            parser.eof();
            if (!parser.matched())
                throw URLException(parser.getError());
        }
        
        URL(const char* input) :
            URL(BString(input)) 
        {
        }
        
        URL(const BString& input, const BString& baseURL) :
            URL(input)
        {
            _baseURL = new URL(baseURL);
        }
        
        URL(const URL& source) :
            URL(source.toString())
        {
        }
        
        virtual ~URL() {
            if (_baseURL)
                delete _baseURL;
        }
        
        
        bool operator == (const BString& rhs) const
        {
            return toString() == rhs;
        }
        
        bool operator == (const char* rhs) const
        {
            return toString() == BString(rhs);
        }
        
        operator BString () const
        {
            return toString();
        }
        
        friend ostream& operator << (ostream& output, const URL& url)
        {
            output << url.toString();
            return output;
        }
        
        BString toString() const
        {
            BString string = origin();
            
            string += path();
            
            string += search();
                        
            if (string == "")
                string = "/";
                
            return string;
        }
        
        
        const BString origin() const
        {

            if (_origin && 
                _origin->matched() &&
                _origin->_domain &&
                _origin->_domain->matched())
            {
                BString origin = 
                    protocol() + 
                    BString("://") +
                    domain();
                
                BString port = URL::port();
            
                if (port.length())
                    origin += ":" + port;
                    
                return origin;
            }
            
            if (_baseURL)
                return _baseURL->origin();
                
            return "";
        }
        
        const BString& port() const
        {
            static const BString emptyPort = "";
            
            Port* port = _origin->_port;
            if (port->matched()) {
                if (protocol() == "https" && 
                    port->value() != "443")
                    return port->value();
                else if (protocol() == "http" &&
                    port->value() != "80")
                    return port->value();
            }
            else if (_baseURL)
                return _baseURL->port();
                
            return emptyPort;
        }
        
        BString path() const
        {
            const BString& path = _path->value();

            if (path.length() > 0) {
                return path;
            }
            else
                return "/";
                
        }
        
        vector<BString> paths() const
        {
            BString path = URL::path();

            // Remove leading /
            if (path.length())
                path = path.substr(1);
            
            if (path.endsWith("/"))
                path = path.substr(0, path.length() - 1);
            
            if (path == "")
                return vector<BString>();
                
            vector<BString> paths = path.split('/');
            for (auto& path : paths) {
                path = path.decodeURI();
            }
            
            return paths;
        }
        
        const BString& domain() const
        {
            static const BString
                emptyDomain = "";
            
            Domain* domain = _origin->_domain;
            
            if (domain->matched()) {
                return domain->value();
            }
            
            if (_baseURL)
                return _baseURL->domain();
                
            
            return emptyDomain;

        }
        
        const BString& protocol() const
        {
            static const BString
                defaultProtocol = 
                "https";
                
            Protocol* protocol = 
                _origin->_protocol;
            
            if (protocol->matched()) {
                return protocol->value();
            }
            if (_baseURL)
                return _baseURL->protocol();
                
            return defaultProtocol;
        }
            
        const Search& search() const
        {
            return (*_search);
        }
            
        Search& search()
        {
            return (*_search);
        }
            
            
    };
    

}

#endif