#ifndef BEE_FISH_WEB__URL_H
#define BEE_FISH_WEB__URL_H

#include <map>
#include <vector>
#include <typeinfo>
#include "../parser/parser.h"

namespace BeeFishWeb {

    using namespace BeeFishParser;
    
    class URL : public Capture
    {
    public:
        
        class Protocol : public OrderOfPrecedence {
        public:
            Protocol() : OrderOfPrecedence(
                {
                    {
                        new Item(
                            new CIWord("https")
                        )
                    },
                    {
                        new Item(
                            new CIWord("http")
                        )
                    }
                }
            )
            {
            }
        };
        
        class HostCharacter : public Not {
        public:
            HostCharacter() : Not(
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
        
        class Host : public Capture {
        public:
            Host() : Capture(
                new Repeat<HostCharacter>()
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

        class Search;
            
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
            
            

        class KeyValuePair : public And {
        public:
            friend class Search;

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

        class Search : 
            public Repeat<KeyValuePair>,
            public std::map<BString, BString>
        {
        protected:
            BString _value;
        public:
            Search() : Repeat<KeyValuePair>() { 
                    
            }

            virtual void matchedItem(KeyValuePair* item) {
                BString key = item->_key.decodeURI();
                BString value = item->_value.decodeURI();
                (*this)[key] = value;
                Repeat<KeyValuePair>::matchedItem(item);
            }
            
            virtual bool match(Parser* parser, const Char& character)
            override
            {
                bool matched =
                    Repeat::match(parser, character);
                    
                if (matched)
                    _value += character;
                    
                return matched;
                
            }
            
            virtual const BString& value() const
            override
            {
                return _value;
            }
            
            virtual bool contains(const BString& test) {
                return count(test) > 0;
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
        Capture* _origin = nullptr;
        Protocol* _protocol = nullptr;
        Capture* _host = nullptr;
        Port* _port = nullptr;
        Path* _path = nullptr;
        Search* _search = nullptr;

    public:
            
        URL() : Capture()
        {
              
            _protocol = new Protocol();
            _host = new Capture(new Host());
            _port = new Port();
            _path = new Path();
            _search = new Search();

            _match = new And(
                new Optional(
                    _origin = new Capture(
                        new And(
                            _protocol,
                            new Word("://"),
                            _host,
                            new Optional(
                                new And(
                                    new Character(':'),
                                    _port
                                )
                            )
                        )
                    )
                ),
                _path,
                new Optional(
                    new And(
                        new Character('?'),
                        _search
                        /*
                        new OrderOfPrecedence(
                            {
                                {
                                    _search
                                },
                                {
                                    _statement
                                }
                            }
                        )
                        */
                    )
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
        
        bool operator == (const BString& rhs) const
        {
            return value() == rhs;
        }
        
        bool operator == (const char* rhs) const
        {
            return value() == BString(rhs);
        }
        
        operator const BString& () const
        {
            return value();
        }
        
        
        const BString& origin() const
        {
            return _origin->value();
        }
        
        const BString& path() const
        {
            return _path->value();
        }
        
        const BString& host() const
        {
            return _host->value();
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