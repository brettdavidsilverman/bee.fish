#ifndef BEE_FISH_WEB__HEADERS_H
#define BEE_FISH_WEB__HEADERS_H

#include <map>
#include <vector>
#include <typeinfo>
#include "../parser/parser.h"
#include "../json/json-parser.h"
#include "new-line.h"

using namespace BeeFishParser;
      
namespace BeeFishWeb {

      class Header : public Match
      {
      protected:

         class HeaderNameCharacter : public Not {
         public:
            HeaderNameCharacter() : Not(
               new Or(
                  new BeeFishParser::
                     Character(':'),
                  new BlankChar(),
                  new NewLine()
               )
            )
            {

            }
         };

         class HeaderValueCharacter : public Match {
         public:
            HeaderValueCharacter() : Match(
               new Not(
                  new NewLine()
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
               new And(
                  new Optional(
                     new Blanks
                  ),
                  new BeeFishParser::
                     Character(':'),
                  new Optional(
                     new Blanks()
                  )
               );

            Match*
               headerName =
                  new Repeat<HeaderNameCharacter>();
            
            Match*
               headerValue =
                  new Repeat<HeaderValueCharacter>();

            _match = new And(
               new Capture(
                  headerName,
                  this->_name
               ),
               colon,
               new Capture(
                  headerValue,
                  this->_value
               ),
               new NewLine()
            );
            
         }
         
         virtual ~Header()
         {
         }
      };


      class Headers :
         public Repeat<Header>,
         public std::map<BString, BString>
      {
      public:
        
         Headers() :
            Repeat<Header>()
         {
         }

         virtual ~Headers()
         {
         }
         
         virtual void matchedItem(Header* header)
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