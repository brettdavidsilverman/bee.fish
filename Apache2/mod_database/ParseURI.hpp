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
      Path* _path;
   public:
      IdentifierPath(Path* path) :
         _path(path)
      {
      }
      
      void setMatch(Match* match)
      {
         _match = new Invoke(
            match,
            [this](Match* match)
            {
               if (!onmatch())
               {
                  _parser->fail();
                  return false;
               }
               else
                  return true;
               
            }
         );
      }
      
      
      
      bool onmatch()

      {
         BString& key = value();
         JSONPath path = *_path;
         if (path.contains(key))
         {
            (*_path) = path[key];
           
            return true;
            
         }

         std::stringstream stream;
         stream
            << "Invalid property"
            << " "
            << "\"" << escape(key) << "\"";
        
         _parser->setError(stream.str());
         
         return false;
      }
      
      

      
   };
   
   class QuotedIdentifier : public IdentifierPath
   {
   
   public:
      QuotedIdentifier(Path* path) :
         IdentifierPath(path)
      {
         setMatch(new BeeFishJSON::String());
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
      Identifier(Path* path) :
         IdentifierPath(path)
      {
         setMatch(
            new Capture(
               new And(
                  new Repeat<AlphaNumeric>(0)
               )
            )
         );
      }
      
   };
   
   class PropertyPath :
      public Match
   {
   protected:
      Path* _start;
      QuotedIdentifier* _quotedIdentifier;
      Identifier* _identifier;
      
   public:
      PropertyPath() {
         assert(false);
      }
      
      PropertyPath(Path* start) :
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
      Path* _start;
      PropertyPath* _last;
      //BString _error;
      
   public:
      PropertyPaths(Path* start) :
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
      Path* _start;
      
   public:
      Query(Path* start) :
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
            if (_propertyPaths->result() == nullopt)
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

   optional<Path> parseURI(Database& database, BString& error, const BString& clientIP, const BString& uri, const BString& args) {
      Debug debug;
      
      Path root(database);
      Path path = root[HOST][URLS];
      
      BString segment;
      std::stringstream segments(uri);
      while(std::getline(segments, segment, '/'))
      {
         if (segment.size())
         {
            path = path[segment.decodeURI()];
         }
      }

      
      if (args.length()) {
         Query query(&path);
         Parser parser(query);
         parser.read(args.decodeURI());
         parser.eof();
         if (parser.result() == false)
         {
            error = parser.getError();
            if (!error.length())
               error = "Unkown error";
            return nullopt;
         }
      }
      
      return path;
      
   }
   

}

#endif
