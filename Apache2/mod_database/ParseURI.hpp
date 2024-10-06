#ifndef BEE_FISH_WEB_SERVER__PARSE_URI
#define BEE_FISH_WEB_SERVER__PARSE_URI

#include "Database/Database.hpp"

using namespace std;
using namespace BeeFishDatabase;
using namespace BeeFishParser;

namespace BeeFishApache2 {
    
   
   
   class IdentifierPath : public Match
   {
   protected:
      JSONPath* _path;
      BString _error;
   public:
      IdentifierPath(JSONPath* path) :
         _path(path)
      {
      }
      
      virtual void success()
      override
      {
         BString& key = value();

         if (_path->contains(key))
         {
            Index position =
               _path->getObjectKeyPosition(key);
                  
            *_path << Type::OBJECT
                   << position;
                   
            _path->unlock();
            
            Match::success();
            return;
            
           
         }
         
         std::stringstream stream;
         stream
            << "Invalid property"
            << " "
            << "\"" << escape(key) << "\"";
        
         Match::fail(stream.str());
         
                  
      }
      
      

      
   };
   
   class QuotedIdentifier : public IdentifierPath
   {
   
   public:
      QuotedIdentifier(JSONPath* path) :
         IdentifierPath(path)
      {
         _match = new BeeFishJSON::String();
      }
      
   };
   
   class Alpha : public Or
   {
   public:
      Alpha() : Or(
         new Range('a', 'z'),
         new Range('A', 'Z'),
         new Character('_')
      )
      {
      }
   };
   
   class Numeric : public Range
   {
   public:
      Numeric() : Range('0', '9')
      {
      }
   };
   
   class AlphaNumeric : public Not
   {
   public:
      AlphaNumeric() : Not(
         new Or(
            new Character('.'),
            new Character('[')
         )
      )
      {
      }
   };
   
   class Identifier : public IdentifierPath
   {
   public:
      Identifier(JSONPath* path) :
         IdentifierPath(path)
      {
         _match =
            new Capture(
               new And(
                  new Repeat<AlphaNumeric>(0)
               )
            );
      }
      
   };
   
   class PropertyPath :
      public Match
   {
   protected:
      JSONPath* _start;
      QuotedIdentifier* _quotedIdentifier;
      Identifier* _identifier;
      
   public:
      PropertyPath() {
         assert(false);
      }
      
      PropertyPath(JSONPath* start) :
         _start(start)
      {
         _match = new Or(
            new And(
               new Character('['),
               _quotedIdentifier =
                  new QuotedIdentifier(_start),
               new Character(']')
            ),
            new And(
               new Character('.'),
               _identifier =
                  new Identifier(_start)
            )
            
         );
      }
      /*
      virtual void fail()
      override
      {
         if (_quotedIdentifier->getError().length())
            _error = _quotedIdentifier->getError();
         else if (_identifier->getError().length())
            _error = _identifier->getError();
            
         Match::fail();
      }
      
      const BString& getError() const
      {
         return _error;
      }
      */
   };
   
   class PropertyPaths : public Repeat<PropertyPath>
   {
   protected:
      JSONPath* _start;
      PropertyPath* _last;
      //BString _error;
      
   public:
      PropertyPaths(JSONPath* start) :
         Repeat<PropertyPath>(0),
         _start(start)
      {
      }
      
      virtual PropertyPath* createItem()
      override
      {
         PropertyPath* item =
            new PropertyPath(_start);
            
         if (_parser)
            item->setup(_parser);
               
         _last = item;
         
         return item;
      }
      /*
      virtual void fail()
      override
      {
         _error = _last->getError();
      }
      
      const BString& getError() const
      {
         return _error;
      }
      */
     
   };
   
   class Query : public Match
   {
     
   protected:
      Word* _wordThis;
      PropertyPaths* _propertyPaths;
      JSONPath* _start;
      
   public:
      Query(JSONPath* start) :
         _start(start)
      {
         _match =
            new And(
               _wordThis = new Word("this"),
               _propertyPaths =
                  new PropertyPaths(_start)
            );
      }
      
      
      virtual void eof(Parser* parser)
      override
      {
         setup(parser);
         
         if (_wordThis->result() == true)
         {
            if (_propertyPaths->result() != false)
            {
               _propertyPaths->eof(parser);
               if (_propertyPaths->result() == true)
               {
                  success();
                  return;
               }
            }
         }
         
         fail();
       
      }
      /*
      const BString& getError() const
      {
         return _propertyPaths->getError();
      }
      
     */
   };

   Path parseURI(Database& database, const char* clientIP, const char* uri, const char* args) {
      Debug debug;
      
      Path root(database);
      Path path = root[HOST][URLS];
      

      string _uri = uri;
      
      BString segment;
      std::stringstream segments(_uri);
      while(std::getline(segments, segment, '/'))
      {
         if (segment.size())
         {
            path = path[segment.decodeURI()];
         }
      }

      JSONPath jsonPath(path);
      
      
      if (args && strlen(args)) {
         BString _args = args;
         
         Query query(&jsonPath);
         Parser parser(query);
         parser.read(_args.decodeURI());
         parser.eof();
         
         if (parser.result() != true)
            cerr << parser.getError() << endl;
        
      }
      
      return jsonPath;
      
   }
   

}

#endif
