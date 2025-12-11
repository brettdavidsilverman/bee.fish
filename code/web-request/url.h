#ifndef BEE_FISH_WEB__URL_H
#define BEE_FISH_WEB__URL_H

#include <map>
#include <vector>
#include <typeinfo>
#include "../query/query.h"
#include "new-line.h"


namespace BeeFishWeb {

    using namespace BeeFishParser;
    
    class URL : public Match
    {
    public:

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

        class Query;
            
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
                            new Character('='),
                            new Capture(
                                new Value(),
                                _value
                            )
                        )
                    ),
                    new Optional(
                        new Character('&')
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
            
            virtual bool matchCharacter(const Char& character)
            override
            {
                bool matched =
                    Repeat::matchCharacter(character);
                    
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
        Path* _path = nullptr;
        Query* _query = nullptr;
    public:
            
        URL() : Match()
        {
              
                
            _path = new Path();
                    
            Match* query =
                new Optional(
                    new And(
                        new Character('?'),
                        _query = new Query()
                    )
                );
                    
            _match = new And(
                _path,
                query
            );

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
    

}

#endif