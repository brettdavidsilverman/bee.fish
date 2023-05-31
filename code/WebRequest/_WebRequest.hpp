#ifndef BEE_FISH_WEB__WEB_REQUEST_H
#define BEE_FISH_WEB__WEB_REQUEST_H

#include <map>
#include <vector>
#include <typeinfo>
#include "../parser/parser.h"
#include "../json/json-parser.h"
#include "../json/json.h"

#ifdef SERVER
#include "../database/path.h"
#endif

#include "headers.h"
#include "content-length.h"
#include "body.h"

using namespace BeeFishParser;
using namespace BeeFishPowerEncoding;
      
namespace BeeFishWeb {

   class WebRequest : public And {
   public:

      class URL : public Match
      {
      public:

         class Hex : public Capture {
         public:
            Hex() : Capture(
               new Or (
                  new Range('a', 'f'),
                  new Range('A', 'F'),
                  new Range('0', '9')
               )
            )
            {

            }
            
         };

         class HexCharacter : public And {
         protected:
            Hex* _hex1 = nullptr;
            Hex* _hex2 = nullptr;

         public:
            HexCharacter() : And()
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
               stream << (uint8_t)_hex1->character() << (uint8_t)_hex2->character();

               uint32_t u32;
               stream >> u32;
               _character = Char(u32);
            }


         };

         class PathCharacter : public Or {
         public:
            PathCharacter() : Or (
               new Range('0', '9'),
               new Range('a', 'z'),
               new Range('A', 'Z'),
               new BeeFishParser::Character('+'),
               new BeeFishParser::Character('.'),
               //new BeeFishParser::Character('='),
               //new BeeFishParser::Character('&'),
               new BeeFishParser::Character('-'),
               new BeeFishParser::Character('_'),
               new BeeFishParser::Character('/'),
               new HexCharacter()
            )
            {

            }

            virtual const Char& character() const {
               return item().character();
            }

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

            virtual BString value(){
               return _value;
            }

            virtual BString value() const {
               return _value;
            }
         };

         class Query;

         class KeyValuePair : public And {
         public:
            friend class Query;

         protected:
            Path* _key;
            Path* _value;

         public:
            KeyValuePair() : And ()
            {
               _inputs = {
                  _key = new Path(),
                  new Optional(
                     new And(
                        new BeeFishParser::Character('='),
                        _value = new Path()   
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
               BString key = item->_key->value();
               BString value = "";
               if (item->_value->matched())
                  value = item->_value->value();
               (*this)[key] = value;
               Repeat<KeyValuePair>::matchedItem(item);
            }

            virtual void success() {
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

         /*
            virtual void success() {
               Path::success();
               vector<BString> keyValuePairs = _value.split('&');
               for (const BString& pair : keyValuePairs) {
                  BString equals("=");
                  
                  auto itEquals = std::find_first_of(
                     pair.cbegin(), pair.cend(),
                     equals.cbegin(), equals.cend()
                  );

                  if (itEquals != pair.cend()) {
                     size_t posEquals = itEquals - pair.cbegin();
                     BString key = pair.substr(0, posEquals);
                     BString value = pair.substr(posEquals + 1);
                     emplace(key, value);
                  }
               }
            }
            */

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
                     new BeeFishParser::
                        Character('?'),
                     _query = new Query()
                  )
               );
               
            _match = new And(_path, query);

         }
         
         const BString path() const
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
         class Method : public Or{
         public:
            Method() : Or(
               new Word("GET"),
//               new Word("PUT"),
               new Word("POST"),
               new Word("DELETE")
//               new Word("OPTIONS")
            )
            {

            }

         };

         class Version : public And {
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
         URL*    _url = nullptr;
         BString _version;
         BString _fullURL;
      public:
         FirstLine() : Match()
         {
            _match = new And(
               new Capture(
                  new Method(),
                  _method
               ),
               new Blanks(),
               new Capture(
                  _url = new URL(),
                  _fullURL
               ),
               new Blanks(),
               new Capture(
                  new Version(),
                  _version
               ),
               new NewLine()
            );

         }
         
         virtual ~FirstLine()
         {
         }
         
         const URL& url() const {
            return *_url;
         }
      
         URL& url() {
            return *_url;
         }

         const BString& fullURL() const {
            return _fullURL;
         }

      };

   /*
      class ObjectPath :
         public _Object,
         protected Path<PowerEncoding>
      {
      
      public:
         ObjectPath(const Path& start) :
            _Object(),
            Path(start)
         {
         }
         
         ObjectPath(const ObjectPath& source) :
            _Object(),
            Path(source)
         {
         }
         
         virtual Match* copy() const
         {
            return new ObjectPath(*this);
         }
         
            
         virtual _JSON* operator[] (const BString& key)
         {
            Path next = Path::operator [] (key);
            _JSON* json = _Object::operator[] (key);
            return json;
         }
         
         virtual _JSON* operator[] (const char* key)
         {
            _JSON* json = ObjectPath::operator[] (BString(key));
            return json;
         }
         
         virtual bool contains(const BString& key)
         {
            bool value = _Object::contains(key);
            return value;
         }
         
         virtual bool contains(const char* key)
         {
            bool value = ObjectPath::contains(BString(key));
            return value;
         }
         
         void emplace(
            const BString& key,
            _JSON* value
         )
         {
            Path::operator [] (key);
            _Object::emplace(key, value);
         }
         
      protected:
         
      };
      */
      
      FirstLine*           _firstLine = nullptr;
      BeeFishWeb::Headers* _headers   = nullptr;
      Body*                _body = nullptr;
      BStream::OnBuffer    _ondata = nullptr;
   public:

      WebRequest() : And()
      {
      }

      virtual void setup(Parser* parser) {
         _firstLine = new FirstLine();
         _headers   = new Headers();
         _body      = nullptr;

         _inputs = {
            _firstLine,
            _headers,
            new NewLine()
         };

         _firstLine->setup(parser);
         _headers->setup(parser);

         _headers->_onsuccess = 
            [this, parser](Match* match) {
               if ( _firstLine->_method == "POST" ) {
                  // Currently we only handle json or image/jpeg
                  if (_headers->contains("content-length") ) {
                     if (_headers->at("content-length") == "0") {
                        _result = true;
                        success();
                        return;
                     }
                  }
                  
                  _body = new Body();
                  _body->setup(parser, _headers);
                  _body->setOnBuffer(_ondata);
                  _inputs.push_back(_body);
               }

            };

         And::setup(parser);
         
      }
    
      virtual ~WebRequest()
      {
      }
      

      virtual void setOnData(BStream::OnBuffer ondata) {
         _ondata = ondata;
         if (_body)
            _body->setOnBuffer(_ondata);
      }

      virtual void flush() {
         if (_body)
            _body->flush();
      }

      virtual bool hasJSON()
      {
         return _body && _body->hasJSON();
      }
   
      virtual BeeFishJSON::Object& json()
      {
         return *(_body->_json);
      }
/*
      virtual const std::vector<Byte>& body() const {
         return _body->body();
      }
*/      
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

      BString query() const {
         return url().query().value();
      }
      
      BString version() const
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
