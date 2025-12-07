#ifndef BEE_FISH_WEB__HEADERS_H
#define BEE_FISH_WEB__HEADERS_H

#include <map>
#include <vector>
#include <typeinfo>
#include "../query/query.h"
#include "../parser/parser.h"
#include "../json/json-parser.h"
#include "new-line.h"


namespace BeeFishWeb {

    using namespace BeeFishParser;
    using namespace BeeFishQuery;

    class Header : public Match
    {
    protected:

        class HeaderNameCharacter : public BeeFishParser::Not {
        public:
            HeaderNameCharacter() : Not(
                new BeeFishParser::Or(
                    new BeeFishParser::Character(":"),
                    new BeeFishParser::Character("\r"),
                    new BeeFishParser::Character("\n")
                )
            )
            {

            }
        };
        
        
        class HeaderValueCharacter : public BeeFishParser::Not {
        public:
            HeaderValueCharacter() : Not(
                new BeeFishParser::Or(
                    new BeeFishParser::Character("\r"),
                    new BeeFishParser::Character("\n")
                )
            )
            {

            }
        };


    public:
        BString _name;
        BString _value;
         
    public:
        Header() : Match()
        {
            Match* colon =
                new BeeFishParser::And(
                    new Blankspaces(),
                    new BeeFishParser::
                        Character(':'),
                    new Blankspaces()
               );

            Match*
               headerName =
                  new Repeat<HeaderNameCharacter>(1);
            
            Match*
               headerValue =
                  new Repeat<HeaderValueCharacter>();

            _match = new BeeFishParser::And(
                new Capture(
                    headerName,
                    this->_name
                ),
                
                colon,
                
                new Capture(
                    headerValue,
                    this->_value
                ),
                
                new Optional(
                    new NewLine()
                )
                
            );
            
            
        }
        
        
         
        
    };

    class Headers :
        public Repeat<Header>,
        public std::map<BString, BString>
    {
    public:
        
        Headers() : Repeat<Header>()
        {
        }

        virtual ~Headers()
        {
        }
         
        
        
        virtual void matchedItem(Header* header)
        override
        {

            BString lowerName =
                header->_name.toLower();
         
            emplace(
                lowerName,
                header->_value
            );
            
            Repeat::matchedItem(header);
        }
   
        bool contains(const BString& name) const
        {
            return count(name) > 0;
        }
         
        bool contains(const char* name) const
        {
            BString bstrName(name);
            return contains(bstrName);
        }

        BString operator[] (
            const BString& name
        ) const
        {
            const BString emptyHeader;

            if (contains(name))
                return map<BString, BString>::at(name);
            else
                return emptyHeader;
        }
   
        friend ostream& operator <<
        (ostream& out, Headers& headers)
        {
            for (auto it = headers.begin();
                      it != headers.end();
                    ++it)
            {
                BString header = it->first;
                BString value = it->second;
                out
                  << header
                  << '\t'
                  << value
                  << endl;
            }
      
            return out;
        }
         
        
        
   
    };
      
    

}

#endif